#include "eol_armature.h"
#include "eol_mesh.h"
#include "eol_logger.h"
#include "eol_loader.h"

/* internal globals. DO NOT EXTERN OUTSIDE OF THIS FILE!*/
eolBool   _eol_armature_initialized = eolFalse;
eolUint   _eol_armature_max = 0;
eolArmature * _eol_armature_list = NULL;

/* internal private functions */
void eol_armature_delete(eolArmature *arm);
eolArmature * eol_armature_get_by_filename(char *filename);
int eol_armature_load_from_obj(char *filename,eolArmature *arm);
int eol_armature_load_poses(FILE *file,eolArmature *arm);
void eol_armature_close();
eolBool
eol_armature_descendant_of(
    eolArmature *arm,
    eolBone *ancestor,
    eolBone *descendant
  );

void eol_armature_load_config()
{
  _eol_armature_max = 255;
}

void eol_armature_init()
{
  eol_logger_message(EOL_LOG_INFO,"eol_armature: initializing\n");
  _eol_armature_list = (eolArmature *)malloc(
          sizeof(eolArmature)*_eol_armature_max);
  if (_eol_armature_list == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_armature: cannot allocate armature list!\n");
    return;
  }
  atexit(eol_armature_close);
  _eol_armature_initialized = eolTrue;
  eol_logger_message(EOL_LOG_INFO,"eol_armature: initialized\n");
}

eolBool eol_armature_initialized()
{
  return _eol_armature_initialized;
}

eolArmature * eol_armature_new()
{
  int i;
  if (!eol_armature_initialized())
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_armature: not initialized\n");
    return NULL;
  }
  for (i = 0; i < _eol_armature_max;i++)
  {
    if (_eol_armature_list[i]._refCount == 0)
    {
      _eol_armature_list[i]._refCount = 1;
      return &_eol_armature_list[i];
    }
  }
  eol_logger_message(EOL_LOG_WARN,"eol_armature: out of space for new mesh data.\n");
  return NULL;
}

eolArmature * eol_armature_load(char *filename)
{
  eolArmature *arm = NULL;
  if (!eol_armature_initialized())
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_armature: used uninitialized!\n");
    return NULL;
  }
  arm = eol_armature_get_by_filename(filename);
  if (arm != NULL)
    return arm;
  arm = eol_armature_new();
  if (arm == NULL)
    return NULL;
  strncpy(arm->filename,filename,EOLLINELEN);
  if (eol_armature_load_from_obj(filename,arm) != 0)
  {
    eol_armature_delete(arm);
    return NULL;
  }
  return arm;
}

void eol_armature_get_bone_count(FILE *file,eolArmature *arm)
{
  eolLine buf;
  int numbones = 0;
  if ((!file) ||
      (!arm))
  {
    return;
  }
  rewind(file);
  while(fscanf(file, "%s", buf) != EOF)
  {
    if(strcmp(buf,"bonecount") == 0)
    {
      fscanf(file, "%i",&numbones);
      break;
    }
  }
  arm->boneCount = numbones;
}

void eol_armature_get_frame_count(FILE *file,eolArmature *arm)
{
  eolLine buf;
  int numframes = 0;
  if ((!file) ||
      (!arm))
  {
    return;
  }

  rewind(file);
  while(fscanf(file, "%s", buf) != EOF)
  {
    if(strcmp(buf,"framecount") == 0)
    {
      fscanf(file, "%i",&numframes);
      break;
    }
  }
  arm->frameCount = numframes;
}

eolBone *eol_armature_add_bone(eolArmature *arm,eolLine name,eolInt *index)
{
  int i;
  if (index != NULL)
  {
    *index = -1;
  }
  if (!arm)return NULL;
  for (i = 0;i < arm->boneCount; i++)
  {
    if (strlen(arm->_bones[i].name)==0)
    {
      if (index != NULL)
      {
        *index = i;
      }
      arm->_bones[i].index = i;
      arm->_bones[i].parentIndex = -1;
      arm->_bones[i].hasParent = eolFalse;
      strncpy(arm->_bones[i].name,name,EOLLINELEN);
      return &arm->_bones[i];
    }
  }
  return NULL;
}

eolBone *eol_armature_get_bone_by_name(
    char *bonename,
    eolArmature *arm,
    eolInt *index)
{
  int i;
  if ((!bonename) ||
      (!arm))
  {
    if (index != NULL)
    {
      *index = -1;
    }
    return NULL;
  }
  if (index != NULL)
  {
    *index = -1;
  }

  for (i = 0;i < arm->boneCount; i++)
  {
    if (strcmp(bonename,arm->_bones[i].name) == 0)
    {
      if (index != NULL)
        *index = i;
      return &arm->_bones[i];
    }
  }
  return NULL;
}

int eol_armature_load_bones(FILE *file,eolArmature *arm)
{
  eolLine buf;
  eolLine bonename;
  int count = 0;
  int index = -1;
  int childindex = 0;
  int mindex = 0;
  int armindex = 0;
  float m1,m2,m3,m4;
  eolBone *bone = NULL;/* active bone for adding*/
  eolBone *temp = NULL;
  if ((!file) ||
      (!arm))
  {
    return -1;
  }
  rewind(file);
  while(fscanf(file, "%s", buf) != EOF)
  {
    if(strcmp(buf,"armature") == 0)
    {
      fscanf(file, "%s",arm->name);
      armindex = 0;
      continue;
    }
    if(strcmp(buf,"bone:") == 0)
    {
      childindex = 0;
      mindex = 0;
      fscanf(file, "%s",bonename);
      if (strlen(bonename) <= 0)
      {
        continue;
      }
      bone = eol_armature_get_bone_by_name(bonename,arm,NULL);
      if (bone != NULL)
      {
        /*bone must have been added by parent, but it is now active bone*/
        continue;
      }
      bone = eol_armature_add_bone(arm,bonename,NULL);
      if (bone == NULL)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:not enough space allocated for bones in armature %s\n",
            arm->filename
          );
        return -1;
      }
      continue;
    }
    if(strcmp(buf,"parent:") == 0)
    {
      fscanf(file, "%s",bonename);
      if (bone == NULL)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:parent specified before bone in armature %s\n",
            arm->filename
          );
        continue;
      }
      temp = eol_armature_get_bone_by_name(bonename,arm,&index);
      if (temp == NULL)
      { /*parent not yet added, so lets add it*/
        temp = eol_armature_add_bone(arm,bonename,&index);
      }
      if (temp == NULL)
      {
        /*at this point, the bone could not be added*/
        return -1;
      }
      bone->hasParent = eolTrue;
      bone->parentIndex = index;
      continue;
    }
    if(strcmp(buf,"childcount") == 0)
    {
      if (bone == NULL)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:childcount specified before bone in armature %s\n",
            arm->filename
          );
        continue;
      }
      fscanf(file, "%i",&count);
      if (count <= 0)
      {
        continue;
      }
      if ((bone->numChildren > 0) ||
          (bone->children != NULL))
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:childcount specified twice for a bone in armature %s\n",
            arm->filename
          );
        continue;
      }
      bone->numChildren = count;
      bone->children = (eolInt *)malloc(sizeof(eolInt)*count);
      if (bone->children == NULL)
      {
        eol_logger_message(
            EOL_LOG_ERROR,
            "eol_armature:unable to allocate children for bone in armature %s\n",
            arm->filename
          );
        return -1;
      }
      memset(bone->children,0,sizeof(eolInt)*count);
      continue;
    }
    if(strcmp(buf,"child:") == 0)
    {
      if (bone == NULL)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:child specified before bone in armature %s\n",
            arm->filename
          );
        continue;
      }
      if (bone->children == NULL)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:child specified before allocation for bone in armature %s\n",
            arm->filename
          );
        continue;
      }
      fscanf(file, "%s",bonename);
      if (strlen(bonename) <= 0)
      {
        continue;
      }
      temp = eol_armature_get_bone_by_name(bonename,arm,&index);
      if (temp == NULL)
      { /*child not yet added, so lets add it*/
        temp = eol_armature_add_bone(arm,bonename,&index);
      }
      if (temp == NULL)
      {
        /*at this point, the bone could not be added*/
        return -1;
      }
      if (index < 0)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:child has bad index for armature %s\n",
            arm->filename
          );
        continue;
      }
      bone->children[childindex] = index;
      childindex++;
      continue;
    }
    if(strcmp(buf,"m:") == 0)
    {
      if (bone == NULL)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:matrix specified before bone in armature %s\n",
            arm->filename
          );
        continue;
      }
      fscanf(
          file,
          "%f %f %f %f",
          &m1,
          &m2,
          &m3,
          &m4
          );

      bone->matrix[mindex][0] = m1;
      bone->matrix[mindex][1] = m2;
      bone->matrix[mindex][2] = m3;
      bone->matrix[mindex][3] = m4;
      mindex++;
      continue;
    }
    if(strcmp(buf,"armmat:") == 0)
    {
      if (armindex >= 4)
      {
        eol_logger_warn(
          "eol_armature: too many armature matrix rows specified in armature %s\n",
          arm->filename
        );
      }
      fscanf(
          file,
          "%f %f %f %f",
          &m1,
          &m2,
          &m3,
          &m4
          );
      arm->matrix[armindex][0] = m1;
      arm->matrix[armindex][1] = m2;
      arm->matrix[armindex][2] = m3;
      arm->matrix[armindex][3] = m4;
      armindex++;
      continue;
    }
  }
  return 0;
}

int eol_armature_load_from_obj(char *filename,eolArmature *arm)
{
  int i;
  eolFile *efile = NULL;
  if (arm == NULL)return -1;
  efile = eol_loader_read_file(filename);
  if (efile == NULL)return -1;

  eol_armature_get_bone_count(efile->file,arm);

  if (arm->boneCount <= 0)
  {
    eol_logger_message(
        EOL_LOG_WARN,
        "eol_armature: armature %s has no bones!\n",
        filename);
    eol_loader_close_file(&efile);
    return -1;
  }

  arm->_bones = (eolBone *)malloc(sizeof(eolBone)*arm->boneCount);
  if (arm->_bones == NULL)
  {
    eol_logger_message(
        EOL_LOG_WARN,
        "eol_armature: cannot allocate bones!\n",
        filename);
    eol_loader_close_file(&efile);
    return -1;
  }
  memset(arm->_bones,0,sizeof(eolBone)*arm->boneCount);

  if (eol_armature_load_bones(efile->file,arm) != 0)
  {
    eol_loader_close_file(&efile);
    return -1;
  }

  eol_armature_get_frame_count(efile->file,arm);
  if (arm->frameCount > 0)
  {
    arm->_poses = (eolPose *)malloc(sizeof(eolPose)*arm->frameCount);
    if (arm->_poses == NULL)
    {
      eol_logger_message(
          EOL_LOG_WARN,
          "eol_armature: cannot allocate poses!\n",
          filename);
      eol_loader_close_file(&efile);
      return -1;
    }
    memset(arm->_poses,0,sizeof(eolPose)*arm->frameCount);

    for (i = 0; i < arm->frameCount; i++)
    {
      arm->_poses[i].bones =
        (eolPoseBone*)malloc(sizeof(eolPoseBone)*arm->boneCount);
      if (arm->_poses[i].bones == NULL)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature: cannot allocate pose bones!\n",
            filename);
        eol_loader_close_file(&efile);
        return -1;
      }
      memset(arm->_poses[i].bones,0,sizeof(eolPoseBone)*arm->boneCount);
    }
    if (eol_armature_load_poses(efile->file,arm) == -1)
    {
      eol_loader_close_file(&efile);
      return -1;
    }
  }
  eol_loader_close_file(&efile);
  return 0;
}

int eol_armature_load_poses(FILE *file,eolArmature *arm)
{
  int index = -1;
  int boneindex = -1;
  int mindex = 0;
  eolLine bonename;
  eolLine buf;
  eolInt frame = -1;
  float m1,m2,m3,m4;
  if ((!file) ||
      (!arm))
  {
    return -1;
  }
  rewind(file);
  while(fscanf(file, "%s", buf) != EOF)
  {
    if(strcmp(buf,"frame:") == 0)
    {
      fscanf(file, "%i",&frame);
      frame--;
      boneindex = -1;
      /*we index from 0, the file indexes from 1*/
      continue;
    }
    if(strcmp(buf,"posebone:") == 0)
    {
      if (frame <= -1)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:posebone specified before frame for armature %s\n",
            bonename,
            arm->filename);
        continue;
      }
      fscanf(file, "%s",bonename);
      if (strlen(bonename) <= 0)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:bad bonepose for armature %s\n",
            arm->filename);
        continue;
      }
      eol_armature_get_bone_by_name(
          bonename,
          arm,
          &index);
      if (index == -1)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:cannot find bone %s for armature %s\n",
            bonename,
            arm->filename);
        continue;
      }
      mindex = 0;
      boneindex = index;
      arm->_poses[frame].bones[boneindex].boneIndex = boneindex;
      strncpy(arm->_poses[frame].bones[boneindex].name,bonename,EOLLINELEN);
      continue;
    }
    if(strcmp(buf,"scale:") == 0)
    {
      if (frame == -1)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:scale specified before frame in armature %s\n",
            arm->filename
          );
        continue;
      }
      fscanf(
          file,
          "%f %f %f",
          &m1,
          &m2,
          &m3
          );
      arm->_poses[frame].bones[boneindex].scale.x = m1;
      arm->_poses[frame].bones[boneindex].scale.y = m2;
      arm->_poses[frame].bones[boneindex].scale.z = m3;
    }
    if(strcmp(buf,"lm:") == 0)
    {
      if (frame <= -1)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:matrix specified before frame in armature %s\n",
            arm->filename
          );
        continue;
      }

      if (boneindex == -1)
      {
        eol_logger_message(
            EOL_LOG_WARN,
            "eol_armature:matrix specified before posebone in armature %s\n",
            arm->filename
          );
        continue;
      }
      fscanf(
          file,
          "%f %f %f %f",
          &m1,
          &m2,
          &m3,
          &m4
          );
      arm->_poses[frame].bones[boneindex].matrix[mindex][0] = m1;
      arm->_poses[frame].bones[boneindex].matrix[mindex][1] = m2;
      arm->_poses[frame].bones[boneindex].matrix[mindex][2] = m3;
      arm->_poses[frame].bones[boneindex].matrix[mindex][3] = m4;
      mindex++;
      continue;
    }
  }
  return 0;
}

void eol_armature_free(eolArmature **arm)
{
  if (!eol_armature_initialized())return;
  if ((arm == NULL) || (*arm == NULL))return;
  if ((*arm)->_refCount == 0)return;
  (*arm)->_refCount--;
  *arm = NULL;
  return;
}

void eol_armature_delete(eolArmature *arm)
{
  if (!arm)return;
  int i;
  for (i = 0; i < arm->boneCount;i++)
  {
    if (arm->_bones[i].children != NULL)
    {
      free(arm->_bones[i].children);
    }
  }
  free(arm->_bones);

  for (i = 0; i < arm->frameCount;i++)
  {
    if (arm->_poses[i].bones != NULL)
    {
      free(arm->_poses[i].bones);
    }
  }
  free (arm->_poses);
  memset(arm,0,sizeof(eolArmature));
}

eolArmature * eol_armature_get_by_filename(char *filename)
{
  int i;
  if (!eol_armature_initialized())return NULL;
  for (i = 0; i < _eol_armature_max; i++)
  {
    if ((_eol_armature_list[i]._refCount > 0) &&
        (strncmp(_eol_armature_list[i].filename,filename,EOLLINELEN)==0))
    {
      _eol_armature_list[i]._refCount++;
      return &_eol_armature_list[i];
    }
  }
  return NULL;
}

void eol_armature_cleanup()
{
  int i;
  if (!eol_armature_initialized())return;
  for (i = 0; i < _eol_armature_max; i++)
  {
    if (_eol_armature_list[i]._refCount <= 0)
    {
      eol_armature_delete(&_eol_armature_list[i]);
    }
  }
}

void eol_armature_close()
{
  int i;
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_armature: closing down\n"
    );
  for (i = 0;i < _eol_armature_max;i++)
  {
    eol_armature_delete(&_eol_armature_list[i]);
  }
  free(_eol_armature_list);
  _eol_armature_list = NULL;
  _eol_armature_initialized = eolFalse;
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_armature: closed\n"
    );
}

void
eol_armature_link_mesh(
    eolArmature   *arm,
    eolMesh       *mesh
  )
{
  int i;
  eolMat4 rot90matrix;
 
  if ((arm == NULL) || 
      (mesh == NULL))
  {
    eol_logger_warn(
      "eol_armature: passed null parameter to link_mesh\n"
      );
    return;
  }
  for (i = 0; i < arm->boneCount; i++)
  {
    arm->_bones[i].meshGroup =
        eol_mesh_get_group_by_name(arm->_bones[i].name,mesh);
  }
  rot90matrix[0][0]=1.0f;
  rot90matrix[1][0]=0.0f;
  rot90matrix[2][0]=0.0f;
  rot90matrix[3][0]=0.0f;
  
  rot90matrix[0][1]=0.0f;
  rot90matrix[1][1]=0.0f;
  rot90matrix[2][1]=1.0f;
  rot90matrix[3][1]=0.0f;
  
  rot90matrix[0][2]=0.0f;
  rot90matrix[1][2]=-1.0f;
  rot90matrix[2][2]=0.0f;
  rot90matrix[3][2]=0.0f;
  
  rot90matrix[0][3]=0.0f;
  rot90matrix[1][3]=0.0f;
  rot90matrix[2][3]=0.0f;
  rot90matrix[3][3]=0.0f;

  for (i = 0; i < mesh->_numVertices; i++)
  {
      eol_mat4_mult_vert(
          &mesh->_rest[i],
          arm->matrix,
          mesh->_rest[i]
      );
      eol_mat4_mult_vert(
          &mesh->_rest[i],
          rot90matrix,
          mesh->_rest[i]
      );
  }
  arm->_isLinked = eolTrue;
}

void
eol_armature_deform_mesh(eolArmature *arm,eolMesh *mesh, eolUint frame)
{
  eolPoseBone *tempframe = NULL;
  eolGroup *tempGroup = NULL;
  int i,j,v;

  if((mesh == NULL) ||
     (arm == NULL))
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_armature: passed null information to deform mesh.\n"
      );
    return;
  }
  if (!arm->_isLinked)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_armature: armature %s is not linked.\n",
      arm->filename
    );
    return;
  }
  if(frame >= arm->frameCount)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_armature: passed in frame %i, but armature only has %i frames.\n",
      frame,
      arm->frameCount
      );
  }
  if (frame == mesh->_curFrame)return;
  mesh->_curFrame = frame;
  tempframe = arm->_poses[frame].bones;
  for(j = 0; j < arm->boneCount; j++)
  {
    tempGroup = arm->_bones[tempframe[j].boneIndex].meshGroup;
    if (tempGroup == NULL)
    {
      continue;
    }
    if (strcmp(tempframe[j].name,tempGroup->name) != 0)
    {
      eol_logger_message(
        EOL_LOG_WARN,
        "eol_armature: posebone/group mismatch - posebone: %s, group %s\n",
        tempframe[j].name,
        arm->_bones[tempframe[j].boneIndex].name
        );
    }
    if (strcmp(tempGroup->name,arm->_bones[tempframe[j].boneIndex].name) != 0)
    {
      eol_logger_message(
        EOL_LOG_WARN,
        "eol_armature: bone/group mismatch - bone: %s, group %s\n",
        tempGroup->name,
        arm->_bones[tempframe[j].boneIndex].name
        );
    }
    for(i = 0; i < tempGroup->numVerts; i++)
    {
      v = tempGroup->vertices[i];
      if (v < 0)
      {
        eol_logger_error(
          "eol_armature: underseeking in group %s\n",
          tempGroup->name);
        continue;
      }
      if (v >= mesh->_numVertices)
      {
        eol_logger_error(
          "eol_armature: overseeking in group %s\n",
          tempGroup->name);
        continue;
      }
      eol_mat4_mult_vert_factor(
          &mesh->_vertices[tempGroup->vertices[i]],
          tempframe[j].matrix,
          mesh->_rest[tempGroup->vertices[i]],
          tempGroup->weights[i]
      );
    }
  }
}

eolBool
eol_armature_descendant_of(
    eolArmature *arm,
    eolBone *ancestor,
    eolBone *descendant
  )
{
  int p;
  if ((descendant == NULL) ||
      (arm == NULL) ||
      (ancestor == NULL))
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_armature: descendant_of passed NULL parameter\n"
    );
    return eolFalse;
  }
  if (descendant->hasParent == eolFalse)
  {
    return eolFalse;
  }
  p = descendant->parentIndex;
  while ((p != -1) && (p < arm->boneCount))
  {
    if (&arm->_bones[p] == ancestor)
    {
      return eolTrue;
    }
    if (!arm->_bones[p].hasParent)break;
    p = arm->_bones[p].parentIndex;
  }
  return eolFalse;
}

void
eol_armature_print_bone_hierarchy(
  eolArmature *arm
  )
{
  int b,c;
  if (arm == NULL)return;
  eol_logger_message(
  EOL_LOG_INFO,
  "eol_armature: Bone Hierarchy for armature %s\n",
  arm->filename);
  for (b = 0; b < arm->boneCount; b++)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "bone %s has\n",
      arm->_bones[b].name);
    if (arm->_bones[b].parentIndex != -1)
    {
      eol_logger_message(
        EOL_LOG_INFO,
        "\tparent: %s\n",
        arm->_bones[arm->_bones[b].parentIndex].name);
    }
    else
    {
      eol_logger_message(
        EOL_LOG_INFO,
        "\t<no parent>\n");
    }
    if (arm->_bones[b].numChildren <= 0)
    {
      eol_logger_message(
        EOL_LOG_INFO,
        "\t\t<no children>\n");
      continue;
    }
    for (c = 0; c < arm->_bones[b].numChildren; c++)
    {
      eol_logger_message(
        EOL_LOG_INFO,
        "\t\tchild %s\n",
        arm->_bones[arm->_bones[b].children[c]].name);
      continue;         
    }
  }
}
/*eol@eof*/

