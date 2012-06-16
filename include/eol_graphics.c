/*
    Copyright 2012 Donald Kehoe
    This file is part of the Engine of Lies game engine library

    The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EOL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the EOL game engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "eol_graphics.h"
#include "eol_logger.h"
#include <glib/glist.h>

/*
 *  Local Types
 */

typedef struct
{
  void (*callback)(eolGraphicsView info);
}eolGraphicsCallback;

typedef struct
{
  eolBool           trackFPS;     /**if 1, outputs frame data to terminal*/
  eolBool           fullScreen;   /**1 for full screen, 0 for window mode*/
  eolDouble         scaleFactor;  /**all rendering will be scaled by this
                                    factor.  default to 1*/
  eolUint           frameDelay;   /**minimum number of milliseconds to wait for
                                    drawing the next frame*/
  eolGraphicsView   graphicsView; /**graphics view data*/
}eolGraphicsConfig;

/* internal globals. DO NOT EXTERN OUTSIDE OF THIS FILE!*/
eolBool             _eolGraphicsInitialized = eolFalse;
eolGraphicsConfig   _eolGraphicsConfig;
SDL_Surface       * _eol_videobuffer = NULL;
SDL_PixelFormat   * _eol_pixelFormat = NULL; 
GList             * _eol_resize_callbacks = NULL;
eolFloat            _eol_graphics_FPS = 0;   /*calculated Frames per second*/

/*local function prototypes*/
void eol_graphics_setup_resize_callbacks();

void eol_graphics_load_config()
{
	/*TODO handle config file stuff*/
  _eolGraphicsConfig.graphicsView.depth = 32;
  _eolGraphicsConfig.trackFPS = eolTrue;
  _eolGraphicsConfig.fullScreen = 0; 
  _eolGraphicsConfig.graphicsView.openGLOn = 1;
  _eolGraphicsConfig.scaleFactor = 1;
  _eolGraphicsConfig.frameDelay = 16;
  _eolGraphicsConfig.graphicsView.xRes = 640;
  _eolGraphicsConfig.graphicsView.yRes = 480;
  _eolGraphicsConfig.graphicsView.mipmapping = eolFalse;
  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
  _eolGraphicsConfig.graphicsView.rMask = 0xff000000;
  _eolGraphicsConfig.graphicsView.gMask = 0x00ff0000;
  _eolGraphicsConfig.graphicsView.bMask = 0x0000ff00;
  _eolGraphicsConfig.graphicsView.aMask = 0x000000ff;
  #else
  _eolGraphicsConfig.graphicsView.rMask = 0x000000ff;
  _eolGraphicsConfig.graphicsView.gMask = 0x0000ff00;
  _eolGraphicsConfig.graphicsView.bMask = 0x00ff0000;
  _eolGraphicsConfig.graphicsView.aMask = 0xff000000;
  #endif
}

void setup_default_lighting()
{

	float fogcolor[] = {0.75,0.50,4,1};
glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION, 0.0f);
glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION, 0.02f);
glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION, 0.000002f);
glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION, 0.001f);
glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION, 0.02f);
glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION, 0.000002f);
glEnable(GL_LIGHT0);
glEnable(GL_LIGHT1);
glEnable(GL_FOG);
glFogfv(GL_FOG_COLOR,fogcolor);
glFogi(GL_FOG_MODE, GL_EXP);
glFogf(GL_FOG_DENSITY, 0.001f);
glFogf(GL_FOG_START, 1.0f);
glFogf(GL_FOG_END, 2000.0f);
}


void eol_graphics_init()
{
  eolInt x,y;

  eolUint Vflags = SDL_ANYFORMAT | SDL_SRCALPHA;
  eolUint HWflag = 0;
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_graphics: initializing\n");
  x = _eolGraphicsConfig.graphicsView.xRes;
  y = _eolGraphicsConfig.graphicsView.yRes;
  if(_eolGraphicsConfig.fullScreen)Vflags |= SDL_FULLSCREEN;
  if(_eolGraphicsConfig.graphicsView.openGLOn)Vflags |= SDL_OPENGL;
  
  
  #if defined(_WIN32)
  stream = freopen("stdout.txt", "w", stdout);
  stream = freopen("stderr.txt", "w", stderr);
  #endif
  
  if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_DOUBLEBUF) < 0 )
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_graphics: Unable to init SDL: %s\n", SDL_GetError());
    exit(1);
  }
  atexit(SDL_Quit);
  
  if(SDL_VideoModeOK(x, y, 32, Vflags | SDL_HWSURFACE))
  {
    _eolGraphicsConfig.graphicsView.xRes = x;
    _eolGraphicsConfig.graphicsView.yRes = y;
    _eolGraphicsConfig.graphicsView.depth = 32;
    HWflag = SDL_HWSURFACE;
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
  }
  else if(SDL_VideoModeOK(x + 6, y, 32, Vflags | SDL_HWSURFACE))
  {
    _eolGraphicsConfig.graphicsView.xRes = x + 6;
    _eolGraphicsConfig.graphicsView.yRes = y;
    _eolGraphicsConfig.graphicsView.depth = 32;
    HWflag = SDL_HWSURFACE;
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
  }
  else if(SDL_VideoModeOK(x, y, 32, Vflags))
  {
    _eolGraphicsConfig.graphicsView.xRes = x;
    _eolGraphicsConfig.graphicsView.yRes = y;
    _eolGraphicsConfig.graphicsView.depth = 32;
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  }
  else if(SDL_VideoModeOK(x + 6, y, 32, Vflags))
  {
    _eolGraphicsConfig.graphicsView.xRes = x + 6;
    _eolGraphicsConfig.graphicsView.yRes = y;
    _eolGraphicsConfig.graphicsView.depth = 32;
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  }
  else if(SDL_VideoModeOK(x + 6, y, 32, Vflags))
  {
    _eolGraphicsConfig.graphicsView.xRes = x + 6;
    _eolGraphicsConfig.graphicsView.yRes = y;
    _eolGraphicsConfig.graphicsView.depth = 32;
    Vflags &= ~SDL_FULLSCREEN;
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  }
  else
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "Unable to use your screen: %s\n Deepest appologies \n", SDL_GetError());
    exit(1);
  }
  _eol_videobuffer = SDL_SetVideoMode(
      _eolGraphicsConfig.graphicsView.xRes,
      _eolGraphicsConfig.graphicsView.yRes,
      _eolGraphicsConfig.graphicsView.depth,
      Vflags | HWflag);
  if ( _eol_videobuffer == NULL )
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "Unable to set video mode: %s\n", SDL_GetError());
    exit(1);
  }
  _eol_pixelFormat = _eol_videobuffer->format;
  /*sets the viewing port to the size of the videobuffer*/
  glViewport(0,0,_eolGraphicsConfig.graphicsView.xRes, _eolGraphicsConfig.graphicsView.yRes);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /*view angle, aspect ratio, near clip distance, far clip distance*/
  gluPerspective( 40, (eolDouble)x / (eolDouble)y, .01, 2000.0f);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  /*Clear color for the buffer*/
  glClearColor(0,0,0,0);
  /*Enables drawing pixels according to thier depth*/
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  /*Enables alpha testing*/
  glAlphaFunc(GL_GREATER,0);
  glEnable(GL_ALPHA_TEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
  /*now record the matrix view data*/
  glGetIntegerv(GL_VIEWPORT,_eolGraphicsConfig.graphicsView.viewPort);
  glGetDoublev(GL_MODELVIEW_MATRIX, _eolGraphicsConfig.graphicsView.modelView);
  glGetDoublev(GL_PROJECTION_MATRIX, _eolGraphicsConfig.graphicsView.projection);

  
  _eolGraphicsInitialized = eolTrue;
  setup_default_lighting();
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_graphics: initialized\n"
  );
  
}

eolBool eol_graphics_initialized()
{
	return _eolGraphicsInitialized;
}

void eol_resize_graphics(eolUint w, eolUint h, eolBool fs);
int	 eol_register_resize_callback(void (*callback)(eolGraphicsView view));
void eol_save_screen_shot(const char *file);

eolUint eol_index_color(eolUint color)
{
  switch(color)
  {
    case Eol_Red:
      return SDL_MapRGB(_eol_videobuffer->format,138,0,0);;
    case Eol_Green:
      return SDL_MapRGB(_eol_videobuffer->format,0,138,0);;
    case Eol_Blue:
      return SDL_MapRGB(_eol_videobuffer->format,0,0,138);;
    case Eol_Yellow:
      return SDL_MapRGB(_eol_videobuffer->format,196,196,0);;
    case Eol_Orange:
      return SDL_MapRGB(_eol_videobuffer->format,148,118,0);;
    case Eol_Violet:
      return SDL_MapRGB(_eol_videobuffer->format,128,0,128);
    case Eol_Brown:
      return SDL_MapRGB(_eol_videobuffer->format,100,64,4);
    case Eol_Grey:
      return SDL_MapRGB(_eol_videobuffer->format,128,128,128);
    case Eol_DarkRed:
      return SDL_MapRGB(_eol_videobuffer->format,64,0,0);
    case Eol_DarkGreen:
      return SDL_MapRGB(_eol_videobuffer->format,0,64,0);
    case Eol_DarkBlue:
      return SDL_MapRGB(_eol_videobuffer->format,0,0,64);
    case Eol_DarkYellow:
      return SDL_MapRGB(_eol_videobuffer->format,60,60,0);
    case Eol_DarkOrange:
      return SDL_MapRGB(_eol_videobuffer->format,64,56,0);
    case Eol_DarkViolet:
      return SDL_MapRGB(_eol_videobuffer->format,60,0,60);
    case Eol_DarkBrown:
      return SDL_MapRGB(_eol_videobuffer->format,56,32,2);
    case Eol_DarkGrey:
      return SDL_MapRGB(_eol_videobuffer->format,64,64,64);
    case Eol_LightRed:
      return SDL_MapRGB(_eol_videobuffer->format,255,32,32);
    case Eol_LightGreen:
      return SDL_MapRGB(_eol_videobuffer->format,32,255,32);
    case Eol_LightBlue:
      return SDL_MapRGB(_eol_videobuffer->format,32,32,255);
    case Eol_LightYellow:
      return SDL_MapRGB(_eol_videobuffer->format,250,250,60);
    case Eol_LightOrange:
      return SDL_MapRGB(_eol_videobuffer->format,255,234,30);
    case Eol_LightViolet:
      return SDL_MapRGB(_eol_videobuffer->format,250,30,250);
    case Eol_LightBrown:
      return SDL_MapRGB(_eol_videobuffer->format,200,100,32);
    case Eol_LightGrey:
      return SDL_MapRGB(_eol_videobuffer->format,196,196,196);
    case Eol_Black:
      return SDL_MapRGB(_eol_videobuffer->format,0,0,0);
    case Eol_White:
      return SDL_MapRGB(_eol_videobuffer->format,255,255,255);
    case Eol_Tan:
      return SDL_MapRGB(_eol_videobuffer->format,255,128,64);
    case Eol_Gold:
      return SDL_MapRGB(_eol_videobuffer->format,255,245,30);
    case Eol_Silver:
      return SDL_MapRGB(_eol_videobuffer->format,206,206,226);
    case Eol_YellowGreen:
      return SDL_MapRGB(_eol_videobuffer->format,196,255,30);
    case Eol_Cyan:
      return SDL_MapRGB(_eol_videobuffer->format,0,255,255);;
    case Eol_Magenta:
      return SDL_MapRGB(_eol_videobuffer->format,255,0,255);
  }
  return SDL_MapRGB(_eol_videobuffer->format,0,0,0);
}

void eol_frame_delay(eolUint delay)
{
    static eolUint pass = 100;
    eolUint dif;
    dif = SDL_GetTicks() - pass;
    if(dif < delay)SDL_Delay( delay - dif);
    pass = SDL_GetTicks();
}

void eol_graphics_frame_begin()
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glPushMatrix();
}

void eol_graphics_frame_end()
{
  eolUint Then;
  eolUint delay;
  static eolUint NOW = 0;

  glPopMatrix();
  SDL_GL_SwapBuffers(); 
  eol_frame_delay(_eolGraphicsConfig.frameDelay);
  Then = NOW;
  NOW = SDL_GetTicks();
  if(_eolGraphicsConfig.trackFPS)
  {
    delay = NOW - Then;
    if (delay != 0)
    {
      _eol_graphics_FPS = 1000.0 / delay;
    }
  }
}

eolUint eol_graphics_get_pixel(SDL_Surface *surface, eolUint x, eolUint y)
{
  /* Here p is the address to the pixel we want to retrieve*/
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;
  if((x < 0)||(x >= surface->w)||(y < 0)||(y >= surface->h))return -1;
  switch(surface->format->BytesPerPixel)
  {
    case 1:
      return *p;

    case 2:
      return *(Uint16 *)p;

    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        return p[0] << 16 | p[1] << 8 | p[2];
      else
        return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
      return *(Uint32 *)p;

  }
  return 0;       /*shouldn't happen, but avoids warnings*/
}

void eol_graphics_put_pixel(SDL_Surface *surface, eolUint x, eolUint y, eolUint pixel)
{
  SDL_Rect point = {0,0,1,1};
  point.x = x;
  point.y = y;
  SDL_FillRect(surface,&point,pixel);
}

/**
 * @brief destructor for the graphics component.  called at exit.
 * */
void eol_graphics_exit(void)
{
  _eolGraphicsInitialized = eolFalse;
  g_list_foreach(_eol_resize_callbacks, (GFunc)free, NULL);
  g_list_free(_eol_resize_callbacks);
}

void eol_graphics_register_resize(void (*callback)(eolGraphicsView info))
{
	if ((callback == NULL) ||
		 (_eolGraphicsInitialized == eolFalse))
  {
  	return;
  }
  eolGraphicsCallback *newCallback = 
    (eolGraphicsCallback *)malloc(sizeof(eolGraphicsCallback));
    newCallback->callback = callback;
  _eol_resize_callbacks = g_list_append(_eol_resize_callbacks,(void *)newCallback);
  callback(_eolGraphicsConfig.graphicsView);
}

SDL_PixelFormat *eol_graphics_get_pixel_format()
{
  if (!eol_graphics_initialized())
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_graphics:cannot get pixel format, graphics not initialized\n");
    return NULL;
  }
  return _eol_pixelFormat;
}

void eol_graphics_get_view(eolGraphicsView *view)
{
  if (!eol_graphics_initialized())
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_graphics:cannot get view, graphics not initialized\n");
    return;
  }
  if (view == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_graphics:cannot get view, passed a NULL view pointer\n");
    return;
  }
  /*TODO: make this thread safe*/
  memcpy(view,&_eolGraphicsConfig.graphicsView,sizeof(eolGraphicsView));
}

eolFloat eol_graphics_get_FPS()
{
  return _eol_graphics_FPS;
}

/* eol @ eof*/

