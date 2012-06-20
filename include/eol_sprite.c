#include "eol_sprite.h"
#include "eol_opengl.h"
#include "eol_logger.h"
#include "eol_loader.h"


/* internal globals. DO NOT EXTERN OUTSIDE OF THIS FILE!*/
eolUint     _max_sprites = 0;  /*this is going to be configurable*/
eolSprite * _sprite_list = NULL;
eolUint     _num_sprites = 0;
eolBool      _sprite_init = eolFalse;
eolGraphicsView _graphics_view;

/* internal private functions */
SDL_Surface *_eol_load_image(char *filename);
void eol_sprite_set_gl_coordinates(eolSprite *sprite);
void eol_sprite_resync_graphics_view(eolGraphicsView info);
void eol_sprite_close();

void eol_sprite_load_config()
{
	/*TODO: load config file and set defaults.
	 * otherwise:*/
	_max_sprites = 255;
}

eolBool eol_sprite_initialized()
{
  return _sprite_init;
}

void eol_sprite_init()
{
  eol_logger_message(EOL_LOG_INFO,"eol_sprites: initializing\n");
  _num_sprites = 0;
  _sprite_list = (eolSprite *)malloc(sizeof(eolSprite)*_max_sprites);
  eol_graphics_register_resize(eol_sprite_resync_graphics_view);
  atexit(eol_sprite_close);
  _sprite_init = eolTrue;
  eol_logger_message(EOL_LOG_INFO,"eol_sprites: initialized\n");
}

eolSprite *eol_sprite_get_loaded_by_filename(eolLine filename)
{
	int i;
	if ((_sprite_list == NULL)||(!_sprite_init))
  {
  	eol_logger_message(
  	  EOL_LOG_ERROR,
  	  "eol_sprite:sprite system initialized\n"
  	);
  	return NULL;
  }
	for (i = 0;i < _max_sprites;i++)
  {
  	if (_sprite_list[i]._refCount)
    {
    	if (strcmp(_sprite_list[i].filename,filename)==0)
    		return &_sprite_list[i];
    }
  }
  return NULL;
}

eolSprite *eol_sprite_new()
{
  int i;
  /*makesure we have the room for a new sprite*/
  if(_num_sprites >= _max_sprites)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_sprite: Maximum Sprites Reached.\n"
    );
    return NULL;
  }
  for(i = 0;i < _max_sprites;i++)
  {
    if(_sprite_list[i]._refCount == 0)break;
  }
  if (i == _max_sprites)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
  	    "sprite count error!");
  	return NULL;
  }
  memset(&_sprite_list[i],0,sizeof(eolSprite));
  _sprite_list[i]._refCount = 1;
  _num_sprites++;
  return &_sprite_list[i];
}

eolSprite *eol_sprite_load(
    eolLine filename,
    eolUint frameWidth,
    eolUint frameHeight
  )
{
  int k,j;
  SDL_Surface *temp;
  SDL_Surface *temp1;
  int w, h;
  Uint32 clearColor;
  Uint8 r,g,b,a;
  /*first see if the sprite is already in memory*/
  eolSprite *newSprite = NULL;
  newSprite = eol_sprite_get_loaded_by_filename(filename);
  if (newSprite != NULL)
  {
  	newSprite->_refCount++;
  	return newSprite;
  }
  /*now get a new blank sprite*/
  newSprite = eol_sprite_new();
  if (newSprite == NULL)
  {
  	/*out of space for new sprites!*/
    eol_logger_message(
        EOL_LOG_ERROR,
  	    "eol_sprite: out of sprite slots.!\n");
  	return NULL;
  }
  /*begin new sprite loading and construction*/
  temp = _eol_load_image(filename);
  /*temp = IMG_Load(filename);
  */
  if(temp == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "unable to load a sprite\n");
    return NULL;
  }
  temp1 = SDL_DisplayFormatAlpha(temp);
  strncpy(newSprite->filename,filename,80);
  
  newSprite->framesPerLine = 16;
  /*if width is not specified, assume whole image is a single frame*/
  if(frameWidth != -1)
  {
    newSprite->frameWidth = frameWidth;
    newSprite->frameHeight = frameHeight;
  }
  else
  {
    newSprite->frameWidth = temp1->w;
    newSprite->frameHeight = temp1->h;
  }
  /*makes sure the width and height of the entire surface is opengl compatable*/
  newSprite->imageWidth = w = temp1->w;
  newSprite->imageHeight = h = temp1->h;

  newSprite->_sdlSurface = SDL_CreateRGBSurface(
    SDL_HWSURFACE|SDL_SRCALPHA,
    w,
    h,
    _graphics_view.depth,
    _graphics_view.rMask,
    _graphics_view.gMask,
    _graphics_view.bMask,
    _graphics_view.aMask
  );

  /*Sets the clear color on the surface*/
  clearColor = SDL_MapRGBA(newSprite->_sdlSurface->format, 0, 0, 0,255);
  /* 
   * This goes through the pixel data copying it and checking each pixel for the
   * clear color, when found sets the alpha to 0
   */
  for(k=0;k < newSprite->_sdlSurface->h; k++)
  {
    for(j=0;j < newSprite->_sdlSurface->w; j++)
    {
      SDL_GetRGBA(eol_graphics_get_pixel(temp1, j, k), temp1->format, &r, &g, &b, &a);
      eol_graphics_put_pixel(newSprite->_sdlSurface, j, k, SDL_MapRGBA(newSprite->_sdlSurface->format, r, g, b, a));
      
    }
  }
  if (_graphics_view.openGLOn)
  {
    newSprite->frameTextureWidth = (float)newSprite->frameWidth/(float)w;
    newSprite->frameTextureHeight= (float)newSprite->frameHeight/(float)h;

    eol_sprite_set_gl_coordinates(newSprite);

    glGenTextures(1, &newSprite->_glImage);
    glBindTexture(GL_TEXTURE_2D,newSprite->_glImage);

    if(_graphics_view.mipmapping == 1)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
      gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE,
          newSprite->_sdlSurface->pixels);
    }
    else
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
          newSprite->_sdlSurface->pixels);
    }

    SDL_FreeSurface(newSprite->_sdlSurface);
    newSprite->_sdlSurface = NULL;
  }
  SDL_FreeSurface(temp1);
  temp1 = NULL;
  SDL_FreeSurface(temp);
  temp = NULL;
  return newSprite;
}

void eol_sprite_resync_graphics_view(eolGraphicsView info)
{
  int i;
	memcpy(&_graphics_view,&info,sizeof(eolGraphicsView));
  if (_sprite_list == NULL)
  {
  	return;
  }
  for (i = 0; i < _max_sprites; i++)
  {
    if (_sprite_list[i]._refCount <= 0)
      continue;
    eol_sprite_set_gl_coordinates(&_sprite_list[i]);
  }
}

void eol_sprite_draw_3d(
    eolSprite *sprite,
    eolUint frame,
    eolVec3D position,
    eolVec3D scale,
    eolFloat alpha
  )
{
  eol_sprite_draw_transformed_3d(
    sprite,
    frame,
    position,
    scale,
    eol_vec3d(0,0,0),
    eol_vec3d(1,1,1),
    alpha
  );
}

void eol_sprite_draw_transformed_3d(
    eolSprite *sprite,
    eolUint frame,
    eolVec3D position,
    eolVec3D scale,
    eolVec3D rotation,
    eolVec3D color,
    eolFloat alpha
  )
{
  eolRectFloat src;
  float left,right,top,bottom;
  if(sprite == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
  	    "passed in a NULL sprite to draw.\n");
    return;
  }
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glColor4f(color.x,color.y,color.z,alpha);
  
  src.x = frame%sprite->framesPerLine * sprite->frameTextureWidth;
  src.y = frame/sprite->framesPerLine * sprite->frameTextureHeight;
  src.w = sprite->frameTextureWidth + src.x;
  src.h = sprite->frameTextureHeight + src.y;

  left = src.x;
  right = src.w;
  top = src.y;
  bottom = src.h;
  
  glBindTexture(GL_TEXTURE_2D,sprite->_glImage);
  glPushMatrix();
  glTranslatef(position.x,position.y,position.z);
  glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
  glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
  glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);
  glScalef(scale.x,scale.y,scale.z);
  glTranslatef(sprite->x3D *  0.5f,sprite->y3D *  0.5f,0.0f);
  glTranslatef(sprite->x3D * -0.5f,sprite->y3D * -0.5f,0.0f);
  glBegin( GL_QUADS );
  
  glTexCoord2f(left,top);
  glVertex3f(-sprite->x3D/2,-sprite->y3D/2,0.0f);
  
  glTexCoord2f(left,bottom);
  glVertex3f(-sprite->x3D/2,sprite->y3D/2,0.0f);
  
  glTexCoord2f(right,bottom);
  glVertex3f(sprite->x3D/2,sprite->y3D/2,0.0f);
  
  glTexCoord2f(right,top);
  glVertex3f(sprite->x3D/2,-sprite->y3D/2,0.0f);
  
  glEnd( );
  glPopMatrix();
  glColor4f(1,1,1,1);
  glDisable(GL_BLEND);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);  
}

void eol_sprite_draw(
  eolSprite *sprite,
  eolUint    frame,
  eolInt     x,
  eolInt     y
)
{
  eol_sprite_draw_transformed(
    sprite,
    frame,
    x,
    y,
    1,
    1,
    0,
    eolFalse,
    eolFalse,
    eol_vec3d(1,1,1),
    1
  );
}

void eol_sprite_draw_transformed(
    eolSprite *sprite,
    eolUint frame,
    eolInt x,
    eolInt y,
    eolFloat scaleX,
    eolFloat scaleY,
    eolFloat rotation,
    eolBool hFlip,
    eolBool vFlip,
    eolVec3D color,
    eolFloat alpha
  )
{
  eolRectFloat src;
  eolVec3D pos;
  float left,right,top,bottom;
  if(sprite == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
  	    "passed in a NULL sprite to draw.\n");
    return;
  }
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glColor4f(color.x,color.y,color.z,alpha);
  
  src.x = frame%sprite->framesPerLine * sprite->frameTextureWidth;
  src.y = frame/sprite->framesPerLine * sprite->frameTextureHeight;
  src.w = sprite->frameTextureWidth + src.x;
  src.h = sprite->frameTextureHeight + src.y;
  
    eol_opengl_get_gl_coordinate(
        x,
        y,
        EOL_2DPLANE_,
        _graphics_view.modelView,
        _graphics_view.projection,
        _graphics_view.viewPort,
        &pos.x,
        &pos.y,
        &pos.z
    );
  
  glBindTexture(GL_TEXTURE_2D,sprite->_glImage);
  if (!hFlip)
  {
    left = src.x;
    right = src.w;
  }
  else
  {
    left = src.w;
    right = src.x;
  }
  if (!vFlip)
  {
    top = src.y;
    bottom = src.h;
  }
  else
  {
    top = src.h;
    bottom = src.y;
  }
  glPushMatrix();
  glTranslatef(pos.x,pos.y,pos.z);
  glRotatef(rotation, 0.0f, 0.0f, 1.0f);
  glScalef(scaleX,scaleY,1);
  glTranslatef(sprite->x3D *  0.5f,sprite->y3D *  0.5f,0.0f);
  glTranslatef(sprite->x3D * -0.5f,sprite->y3D * -0.5f,0.0f);
  glBegin( GL_QUADS );
  
  glTexCoord2f(left,top);
  glVertex3f(-sprite->x3D/2,-sprite->y3D/2,0.0f);
  
  glTexCoord2f(left,bottom);
  glVertex3f(-sprite->x3D/2,sprite->y3D/2,0.0f);
  
  glTexCoord2f(right,bottom);
  glVertex3f(sprite->x3D/2,sprite->y3D/2,0.0f);
  
  glTexCoord2f(right,top);
  glVertex3f(sprite->x3D/2,-sprite->y3D/2,0.0f);
  
  glEnd( );
  glPopMatrix();
  glColor4f(1,1,1,1);
  glDisable(GL_BLEND);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
}

void eol_sprite_free(eolSprite **sprite)
{
  if (!eol_sprite_initialized())return;
  if (!sprite)return;
  if (!*sprite)return;
  (*sprite)->_refCount--;
  *sprite = NULL;
}

void eol_sprite_clear_all()
{
  int i;
  for(i = 0;i < _max_sprites;i++)
  {
    if(_sprite_list[i]._sdlSurface != NULL)
    {
      SDL_FreeSurface(_sprite_list[i]._sdlSurface);
    }
    if(glIsTexture(_sprite_list[i]._glImage) == GL_TRUE)
    {
      glDeleteTextures(
        1,
        &_sprite_list[i]._glImage
        );
    }
  }
  memset(_sprite_list,0,sizeof(eolSprite) * _max_sprites);
}

void eol_sprite_close()
{
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_sprite: closing\n");
  eol_sprite_clear_all();
  free(_sprite_list);
  _sprite_list = NULL;
  _num_sprites = 0;
  _sprite_init = eolFalse;
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_sprite: closed\n");
}

/**

Load an Image through PhysFS and SDL_Image

**/
SDL_Surface *_eol_load_image(char *filename)
{
  SDL_Surface *image = NULL;
  SDL_RWops *rw;
  eolFile *efile = NULL;
  
  efile = eol_loader_read_file(filename);

  if (efile == NULL)
  	return NULL;

  rw = SDL_RWFromMem(efile->_buffer, efile->size);
  if(rw == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_sprite: Unable to create RW reference to data loaded for %s\n",
        filename
      );
    eol_loader_close_file(&efile);
    return NULL;
  }
  image = IMG_Load_RW(rw,0);
  SDL_FreeRW(rw);
  eol_loader_close_file(&efile);
  return image;
}

void eol_sprite_set_gl_coordinates(eolSprite *sprite)
{
    eol_opengl_get_gl_coordinate(
        ((_graphics_view.xRes>>1) + sprite->frameWidth),
        ((_graphics_view.yRes>>1) + sprite->frameHeight),
        EOL_2DPLANE_,
        _graphics_view.modelView,
        _graphics_view.projection,
        _graphics_view.viewPort,
        &sprite->x3D,
        &sprite->y3D,
        &sprite->z3D
    );
}


/*eol@eof*/
