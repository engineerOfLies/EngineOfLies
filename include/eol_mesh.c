#include "eol_mesh.h"
#include "eol_logger.h"
#include "eol_loader.h"
#include <glib/glist.h>

/*local global variables*/
eolBool   _eol_mesh_initialized = eolFalse;
eolMesh * _eol_mesh_list = NULL;
eolUint   _eol_mesh_max = 0;

/*local function prototypes*/
void eol_mesh_close();
void eol_mesh_load_from_object(char *filename,eolMesh *mesh);
void eol_mesh_get_mesh_from_object_file(eolMesh * model, FILE* file);
void eol_mesh_object_get_counts(eolMesh* model, FILE* file);
void eol_mesh_get_groups(eolMesh *model, FILE *file);
void eol_mesh_normalize_group_weights(eolMesh *mesh);

void eol_mesh_init()
{
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_mesh:initializing\n"
  );
  if (_eol_mesh_list != NULL)return;
  _eol_mesh_list = (eolMesh *)malloc(
      sizeof(eolMesh) * _eol_mesh_max
  );
  memset(
      _eol_mesh_list,
      0,
      sizeof(eolMesh) * _eol_mesh_max
  );
  atexit(eol_mesh_close);
  _eol_mesh_initialized = eolTrue;
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_mesh:initialized\n"
  );
}

eolBool eol_mesh_initialized()
{
  return _eol_mesh_initialized;
}

void eol_mesh_delete(eolMesh *mesh)
{
  int i;
  if (mesh == NULL)return;
  if (mesh->_rest != NULL)
  {
    free(mesh->_rest);
    mesh->_rest = NULL;
  }
  if (mesh->_vertices != NULL)
  {
    free(mesh->_vertices);
    mesh->_vertices = NULL;
  }
  if (mesh->_normals != NULL)
  {
    free(mesh->_normals);
    mesh->_normals = NULL;
  }
  if (mesh->_texels != NULL)
  {
    free(mesh->_texels);
    mesh->_texels = NULL;
  }
  if (mesh->_faces != NULL)
  {
    free(mesh->_faces);
    mesh->_faces = NULL;
  }
  if (mesh->_groups != NULL)
  {
    for (i = 0;i < mesh->_numGroups; i++)
    {
      if (mesh->_groups[i].vertices != NULL)
      {
        free(mesh->_groups[i].vertices);
      }
      if (mesh->_groups[i].weights != NULL)
      {
        free(mesh->_groups[i].weights);
      }
    }
    free(mesh->_groups);
    mesh->_groups = NULL;
  }
  /*
   * TODO: handle deleting the buffer arrays
  eolUint _vertexArray;
  eolUint _normalArray;
  eolUint _texelArray;
   */
  memset(mesh,0,sizeof(eolMesh));
}

void eol_mesh_close()
{
  int i = 0;
    eol_logger_message(
    EOL_LOG_INFO,
    "eol_mesh:closing\n"
  );

  if (_eol_mesh_list != NULL)
  {
    for (i = 0;i < _eol_mesh_max;i++)
    {
      eol_mesh_delete(&_eol_mesh_list[i]);
    }
  }
  _eol_mesh_list = NULL;
  _eol_mesh_initialized = eolFalse;
    eol_logger_message(
    EOL_LOG_INFO,
    "eol_mesh:closed\n"
  );

}

void eol_mesh_load_config()
{
  /*TODO: load config from file or default to:*/
  _eol_mesh_max = 255;
}

eolMesh * eol_mesh_new()
{
  int i;
  if (!eol_mesh_initialized())
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_mesh:not initialized\n");
    return NULL;
  }
  for (i = 0; i < _eol_mesh_max;i++)
  {
    if (_eol_mesh_list[i]._refCount == 0)
    {
      _eol_mesh_list[i]._refCount = 1;
      return &_eol_mesh_list[i];
    }
  }
  eol_logger_message(EOL_LOG_WARN,"eol_mesh: out of space for new mesh data.\n");
  return NULL;
}

void eol_mesh_free(eolMesh **mesh)
{
  if (!eol_mesh_initialized())return;
  if ((mesh == NULL) ||
     (*mesh == NULL) ||
     ((*mesh)->_refCount == 0))return;
  (*mesh)->_refCount--;
  *mesh = NULL;
}

/*Load mesh data from object file and store the information in the provided mesh
 * structure*/
void eol_mesh_load_from_object(char *filename,eolMesh *model)
{
  eolFile *file;
  file = eol_loader_read_file(filename);

  if (file == NULL)
    return;

  strcpy(model->filename,filename);

  eol_mesh_object_get_counts(model,file->file);
/*TODO add more memory checks for allocation*/
  if(model->_numVertices != 0)
  {
    model->_vertices = (eolVec3D*)malloc(sizeof(eolVec3D)*(model->_numVertices));
    memset(model->_vertices,0,sizeof(eolVec3D)*(model->_numVertices));
    model->_rest = (eolVec3D*)malloc(sizeof(eolVec3D)*(model->_numVertices));
    memset(model->_rest,0,sizeof(eolVec3D)*(model->_numVertices));
  }

  if(model->_numNormals != 0)
  {
    model->_normals = (eolVec3D*)malloc(sizeof(eolVec3D)*(model->_numNormals));
    memset(model->_normals,0,sizeof(eolVec3D)*(model->_numNormals));
  }

  if(model->_numTexels != 0)
  {
    model->_texels = (eolVert2D*)malloc(sizeof(eolVert2D)*(model->_numTexels));
    memset(model->_texels,0,sizeof(eolVert2D)*(model->_numTexels));
  }

  if(model->_numFaces != 0)
  {
    model->_faces = (eolFace*)malloc(sizeof(eolFace)*(model->_numFaces));
    memset(model->_faces,0,sizeof(eolFace)*(model->_numFaces));
  }

  eol_mesh_get_mesh_from_object_file(model,file->file);

  memcpy(model->_rest,model->_vertices,sizeof(eolVec3D)*model->_numVertices);
  model->_curFrame = -1;
  eol_mesh_get_groups(model, file->file);

  eol_loader_close_file(&file);
}

void eol_mesh_object_get_counts(eolMesh* model, FILE* file)
{
  char buf[128];
  int  numvertices = 0;
  int  numnormals = 0;
  int  numtexcoords = 0;
  int  numfaces = 0;

  if ((file == NULL) ||
     (model == NULL))
  {
    return;
  }

  while(fscanf(file, "%s", buf) != EOF)
  {
    switch(buf[0])
    {
      case 'v':
        switch(buf[1])
        {
          case '\0':
            fgets(buf, sizeof(buf), file);
            numvertices++;
            break;
          case 'n':
            fgets(buf, sizeof(buf), file);
            numnormals++;
            break;
          case 't':
            fgets(buf, sizeof(buf), file);
            numtexcoords++;
            break;
          default:
            break;
        }
        break;
      case 'f':
        fgets(buf, sizeof(buf), file);
        numfaces++;
        break;
      default:
        fgets(buf, sizeof(buf), file);
        break;
    }
  }
  model->_numVertices  = numvertices;
  model->_numNormals   = numnormals;
  model->_numTexels = numtexcoords;
  model->_numFaces = numfaces;
}

void eol_mesh_get_mesh_from_object_file(eolMesh * model, FILE* file)
{
  int  numvertices = 0;
  int  numnormals = 0;
  int  numtexcoords = 0;
  int  numfaces = 0;
  char buf[128];
  float x,y,z;

  if (file == NULL)
    return;
  rewind(file);
  while(fscanf(file, "%s", buf) != EOF)
  {
    switch(buf[0])
    {
      case 'v':
        switch(buf[1])
        {
          case '\0':
            fscanf(
                file,
                "%f %f %f",
                &x,
                &y,
                &z
              );
            model->_vertices[numvertices].x = x;
            model->_vertices[numvertices].y = y;
            model->_vertices[numvertices].z = z;
            numvertices++;
            break;
          case 'n':
            fscanf(
                file,
                "%f %f %f",
                &x,
                &y,
                &z
            );
            model->_normals[numnormals].x = x;
            model->_normals[numnormals].y = y;
            model->_normals[numnormals].z = z;
            numnormals++;
            break;
          case 't':
            fscanf(
                file,
                "%f %f",
                &model->_texels[numtexcoords][0],
                &model->_texels[numtexcoords][1]
              );
            model->_texels[numtexcoords][1] = 1 - model->_texels[numtexcoords][1];
            numtexcoords++;
            break;
          default:
            break;
        }
        break;
      case 'f':
        fscanf(
            file,
            "%d/%d/%d %d/%d/%d %d/%d/%d",
            &model->_faces[numfaces].vertices[0],
            &model->_faces[numfaces].texels[0],
            &model->_faces[numfaces].normals[0],
            &model->_faces[numfaces].vertices[1],
            &model->_faces[numfaces].texels[1],
            &model->_faces[numfaces].normals[1],
            &model->_faces[numfaces].vertices[2],
            &model->_faces[numfaces].texels[2],
            &model->_faces[numfaces].normals[2]);
        model->_faces[numfaces].vertices[0]--;
        model->_faces[numfaces].texels[0]--;
        model->_faces[numfaces].normals[0]--;
        model->_faces[numfaces].vertices[1]--;
        model->_faces[numfaces].texels[1]--;
        model->_faces[numfaces].normals[1]--;
        model->_faces[numfaces].vertices[2]--;
        model->_faces[numfaces].texels[2]--;
        model->_faces[numfaces].normals[2]--;
        numfaces++;
        break;
      default:
        fgets(buf, sizeof(buf), file);
        break;
    }
  }
}

void eol_mesh_get_groups(eolMesh *model, FILE *file)
{
  eolBool badload = eolFalse;
  eolLine buf;
  eolLine name;
  eolInt numgroups = 0;
  eolInt i = 0,g = 0;
  eolFloat weight = 0;
  eolGroup * group = NULL;

  if ((model == NULL) ||
      (file == NULL))
  {
    return;
  }
  rewind(file);
  while(fscanf(file, "%s", buf) != EOF)
  {
    if(strcmp(buf,"vgroupcount:") == 0)
    {
      fscanf(file, "%i",&numgroups);
      break;
    }
  }
  /*check if this model has any bones to handle.*/
  if (numgroups == 0)
    return;
  model->_numGroups = numgroups;
  model->_groups = (eolGroup *)malloc(sizeof(eolGroup)*numgroups);
  if (model->_groups == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_mesh: unable to allocate memory for mesh groups.\n"
    );
    free(model->_groups);
    model->_groups = NULL;
    model->_numGroups = 0;
    return;
  }
  memset(model->_groups,0,sizeof(eolGroup)*numgroups);

  /*now we name all of the groups*/
  rewind(file);
  g = 0;
  while(fscanf(file, "%s", buf) != EOF)
  {
    if(strcmp(buf,"vgroup:") == 0)
    {
      fscanf(file, "%s",model->_groups[g].name);
      g++;
      continue;
    }
    if(strcmp(buf,"GroupName:") == 0)
    {
      fscanf(file, "%s",name);
      group = eol_mesh_get_group_by_name(name,model);
      if (group != NULL)
      {
        group->numVerts++;
      }
    }
  }
  /*now allocate vertex and weight information for each group*/
  for (i = 0; i < g;i++)
  {
    model->_groups[i].vertices =
       (eolInt *)malloc(sizeof(eolInt)*model->_groups[i].numVerts);
    model->_groups[i].weights =
       (eolFloat *)malloc(sizeof(eolFloat)*model->_groups[i].numVerts);
    if ((model->_groups[i].vertices == NULL) ||
        (model->_groups[i].weights == NULL))
    {
      badload = eolTrue;
      break;
    }
    memset(model->_groups[i].vertices,0,sizeof(eolInt)*model->_groups[i].numVerts);
    memset(model->_groups[i].weights,0,sizeof(eolFloat)*model->_groups[i].numVerts);
  }
  if (badload)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_mesh: unable to allocate group vertices/weights\n");
    for (i = 0; i < g;i++)
    {
      if (model->_groups[i].vertices != NULL)
      {
          free(model->_groups[i].vertices);
      }
      if (model->_groups[i].weights != NULL)
      {
        free(model->_groups[i].weights);
      }
    }
    free (model->_groups);
    model->_groups = NULL;
    model->_numGroups = 0;
  }

  /* now to go through and assign the information*/
  rewind(file);
  g = 0;
  i =  -1;
  group = NULL;
  while(fscanf(file, "%s", buf) != EOF)
  {
    if(strcmp(buf,"GroupName:") == 0)/*select active bone for addition*/
    {
      fscanf(file, "%s",name);
      group = eol_mesh_get_group_by_name(name,model);
      if (group != NULL)
      {
        group->vertices[group->_assignedV] = i;
        group->_assignedV++;
      }
      continue;
    }
    if(strcmp(buf,"Weight:") == 0)/*select active bone for addition*/
    {
      if (group != NULL)
      {
        fscanf(file,"%f",&weight);
        group->weights[group->_assignedW] = weight;
        group->_assignedW++;
      }
      continue;
    }
    if((buf[0] == 'v') && (buf[1] == '\0'))
    {
      i++;
      continue;
    }
  }
//  eol_mesh_normalize_group_weights(model);
}

eolGroup * eol_mesh_get_group_by_name(char *name,eolMesh *model)
{
  int g;
  if ((name == NULL) ||
      (model == NULL))
  {
    return NULL;
  }
  for (g = 0; g < model->_numGroups;g++)
  {
    if (strcmp(model->_groups[g].name,name) == 0)
    {
      return &model->_groups[g];
    }
  }
  eol_logger_message(
    EOL_LOG_WARN,
    "eol_mesh:unable to find group: %s in mesh: %s\n",name,model->filename);
  return NULL;
}

eolMesh * eol_mesh_get_by_filename(char *filename)
{
  int i;
  if (!eol_mesh_initialized())
    return NULL;
  if (filename == NULL)
    return NULL;
  for (i = 0;i < _eol_mesh_max;i++)
  {
    if ((_eol_mesh_list[i]._refCount > 0) &&
        (strcmp(_eol_mesh_list[i].filename,filename)==0))
    {
      _eol_mesh_list[i]._refCount++;
      return &_eol_mesh_list[i];
    }
  }
  return NULL;
}

eolMesh * eol_mesh_load(char *filename)
{
  eolMesh *mesh = NULL;
  if (!eol_mesh_initialized())
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_mesh: used uninitialized\n"
    );
    return NULL;
  }
  mesh = eol_mesh_get_by_filename(filename);
  if (mesh != NULL)
    return mesh;
  mesh = eol_mesh_new();
  if (mesh == NULL)return NULL;
  /*TODO: support loading from binary file*/
  eol_mesh_load_from_object(filename,mesh);
  return mesh;
}

eolGroup *
eol_mesh_get_next_group_for_vert(
    eolFloat **weight,
    eolMesh  *mesh,
    eolGroup *group,
    int       vindex
  )
{
  int i;
  if (mesh == NULL)
  {
    eol_logger_warn(
      "eol_mesh: passed null parameter in get_group_for_vert\n");
    return NULL;
  }
  if (group == NULL)
  {
    group = &mesh->_groups[0];
  }
  else
  {
    group++;
  }
  while (group < &mesh->_groups[mesh->_numGroups])
  {
    if (group == NULL)
    {
      eol_logger_message(
        EOL_LOG_ERROR,
        "eol_mesh:bad group for mesh %s\n",
        mesh->filename
        );
      break;
    }
    for (i = 0;i < group->numVerts;i++)
    {
      if (group->vertices[i] == vindex)
      {
        if (weight != NULL)
          *weight = &group->weights[i];
        return group;
      }
    }
    group++;
  }
  return NULL;
}

void eol_mesh_normalize_group_weights(eolMesh *mesh)
{
  int v;
  int count = 0;
  eolDouble m = 0;
  eolFloat *weight = NULL;
  eolGroup *gtemp = NULL;
  GList *grouplist = NULL;
  GList *glIt = NULL;
  if (mesh == NULL)
  {
    eol_logger_warn(
      "eol_mesh: passed null parameter in normalize_group_weights\n");
    return;
  }
  for (v = 0; v < mesh->_numVertices; v++)
  {
    m = 0;
    count = 0;
    gtemp = eol_mesh_get_next_group_for_vert(&weight,mesh,NULL,v);
    while (gtemp != NULL)
    {
      grouplist = g_list_append(grouplist,weight);
      m += (*weight * *weight);
      count++;
      gtemp = eol_mesh_get_next_group_for_vert(&weight,mesh,gtemp,v);
    }
    if ((count > 1) && (m != 0))
    {
      m = 1/sqrt(m);
      glIt = grouplist;
      while (glIt != NULL)
      {
        weight = (eolFloat *)glIt->data;
        *weight = *weight * m;
        glIt = g_list_next(glIt);
      }
    }
    g_list_free(grouplist);
    grouplist = NULL;
  }
}

void eol_mesh_draw(
    eolMesh *mesh,
    eolVec3D position,
    eolVec3D rotation,
    eolVec3D scale,
    eolVec3D color,
    eolFloat alpha,
    eolSprite *skin
  )
{
  int i;
  eolFace* triangle;
  float trans[4];

  if (mesh == NULL)
  {
    return;
  }
  glPushMatrix();
  trans[3] = alpha;
  trans[0] = color.x;
  trans[1] = color.y;
  trans[2] = color.z;

  glColorMaterial(GL_FRONT,GL_DIFFUSE);
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glDepthFunc(GL_LEQUAL);

  if(skin != NULL)
  {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,skin->_glImage);
  }

  glMaterialfv(GL_FRONT,GL_DIFFUSE,trans);

  glTranslatef(position.x,position.y,position.z);
  glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
  glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
  glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);
  glScalef(scale.x,scale.y,scale.z);

  glColor3f(color.x,color.y,color.z);
  glBegin(GL_TRIANGLES);
  for (i = 0; i < mesh->_numFaces; i++)
  {
    triangle = &mesh->_faces[i];
    glNormal3f(mesh->_normals[triangle->normals[0]].x,
               mesh->_normals[triangle->normals[0]].y,
               mesh->_normals[triangle->normals[0]].z);
    glTexCoord2fv(mesh->_texels[triangle->texels[0]]);
    glVertex3f(mesh->_vertices[triangle->vertices[0]].x,
               mesh->_vertices[triangle->vertices[0]].y,
               mesh->_vertices[triangle->vertices[0]].z);

    glNormal3f(mesh->_normals[triangle->normals[1]].x,
               mesh->_normals[triangle->normals[1]].y,
               mesh->_normals[triangle->normals[1]].z);
    glTexCoord2fv(mesh->_texels[triangle->texels[1]]);
    glVertex3f(mesh->_vertices[triangle->vertices[1]].x,
               mesh->_vertices[triangle->vertices[1]].y,
               mesh->_vertices[triangle->vertices[1]].z);

    glNormal3f(mesh->_normals[triangle->normals[2]].x,
               mesh->_normals[triangle->normals[2]].y,
               mesh->_normals[triangle->normals[2]].z);
    glTexCoord2fv(mesh->_texels[triangle->texels[2]]);
    glVertex3f(mesh->_vertices[triangle->vertices[2]].x,
               mesh->_vertices[triangle->vertices[2]].y,
               mesh->_vertices[triangle->vertices[2]].z);
  }
  glEnd();

  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_BLEND);
  if(skin != NULL)
  {
    glDisable(GL_TEXTURE_2D);
  }
  glDisable(GL_NORMALIZE);
  glDepthFunc(GL_LESS);
  glPopMatrix();
}
/*eol@eof*/

