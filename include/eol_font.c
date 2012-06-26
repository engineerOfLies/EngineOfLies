#include "eol_font.h"
#include "eol_logger.h"
#include "eol_graphics.h"

/*local global variables*/
eolBool   _eol_font_initialized = eolFalse;
eolLine   _eol_default_font;
eolFont * _eol_font_list = NULL;
eolUint   _eol_font_max = 0;
eolUint   _eol_font_count = 0;
eolFont * _eol_font_size[eolFontSizeMax];

/*local function prototypes*/
void eol_font_delete(eolFont *font);
void eol_font_cleanup();
void eol_font_clear();

void eol_font_init()
{
  eol_logger_message(EOL_LOG_INFO,"eol_font: initializing\n");
  if (TTF_Init() != 0)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_font:unable to setup SDL_TTF\n");
    return;
  }
  atexit(TTF_Quit);
  atexit(eol_font_close);
  _eol_font_list = (eolFont *)malloc(sizeof(eolFont)*_eol_font_max);
  memset(_eol_font_list,0,sizeof(eolFont)*_eol_font_max);
  _eol_font_initialized = eolTrue;
  _eol_font_size[eolFontTiny] = eol_font_load(_eol_default_font,10);
  _eol_font_size[eolFontSmall] = eol_font_load(_eol_default_font,12);
  _eol_font_size[eolFontMedium] = eol_font_load(_eol_default_font,14);
  _eol_font_size[eolFontLarge] = eol_font_load(_eol_default_font,18);
  _eol_font_size[eolFontHuge] = eol_font_load(_eol_default_font,24);
  eol_logger_message(EOL_LOG_INFO,"eol_font: initialized\n");
}

eolBool eol_font_initialized()
{
  return _eol_font_initialized;
}

void eol_font_config()
{
  /*TODO: load config from file*/
  _eol_font_max = 10;
  strncpy(_eol_default_font,"fonts/default.ttf",EOLLINELEN);
}

void eol_font_close()
{
  int i;
  for ( i = 0; i < _eol_font_max;i++)
  {
    eol_font_delete(&_eol_font_list[i]); 
  }
  free(_eol_font_list);
  _eol_font_list = NULL;
  _eol_font_count = 0;
  _eol_font_initialized = eolFalse;
}

eolFont * eol_font_new()
{
  int i;
  if (!eol_font_initialized())
  {
    return NULL;
  }
  for (i = 0;i < _eol_font_max;i++)
  {
    if (_eol_font_list[i]._refCount == 0)
    {
      eol_font_delete(&_eol_font_list[i]);
      _eol_font_list[i]._refCount = 1;
      _eol_font_list[i].index = i;
      return &_eol_font_list[i];
    }
  }
  return NULL;
}

eolFont * eol_font_get_by_name_size(char *filename,eolInt pointSize)
{
  int i;
  if ((!filename) ||
       (strlen(filename) == 0))
  {
    return NULL;
  }
  for (i = 0; i < _eol_font_max; i++)
  {
    if ((strncmp(filename,_eol_font_list[i].filename,EOLLINELEN) == 0) &&
        (_eol_font_list[i].point == pointSize))
    {
      return &_eol_font_list[i];
    }
  }
  return NULL;
}

eolFont * eol_font_load(
    char   *filename,
    eolInt  pointSize
  )
{
  eolFont *font = NULL;
  font = eol_font_get_by_name_size(filename,pointSize);
  if (font != NULL)
  {
    font->_refCount++;
    return font;
  }
  font = eol_font_new();
  if (font == NULL)return NULL;
  font->_font = TTF_OpenFont(filename,pointSize);
  if(font->_font == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "Couldn't initialize Font: %s\n",
      SDL_GetError());
    eol_font_delete(font);
    return NULL;
  }
  font->point = pointSize;
  strncpy(font->filename,filename,EOLLINELEN);
  return font;
}

void eol_font_free(
    eolFont **font
  )
{
  if ((font == NULL) ||
      (*font == NULL) ||
      ((*font)->_refCount == 0))
  {
    return;
  }
  (*font)->_refCount--;
  *font = NULL;
}

void eol_font_delete(eolFont *font)
{
  if (font == NULL)return;
  if (font->_font != NULL)
  {
    TTF_CloseFont(font->_font);
  }
  memset(font,0,sizeof(eolFont));
}

void eol_font_draw_text_justify_custom(
    char    * text,
    eolInt    x,
    eolInt    y,
    eolVec3D  color,
    eolFloat  alpha,
    eolFont * font,
    eolUint   justify
  )
{
  eolRect r;
  eolInt dx;
  if (!eol_font_initialized())return;
  switch (justify)
  {
    case eolJustifyLeft:
      dx = x;
      break;
    case eolJustifyCenter:
      dx = (x - (r.w/2));
      break;
    case eolJustifyRight:
      dx = (x - r.w);
      break;
  }
  r = eol_font_get_bounds_custom(text,font);
  eol_font_draw_text_custom(
    text,
    dx,
    y,
    color,
    alpha,
    font
  );
}

void eol_font_draw_text_justify(
    char   * text,
    eolInt   x,
    eolInt   y,
    eolVec3D color,
    eolFloat alpha,
    eolUint  size,
    eolUint  justify
  )
{
  eolRect r;
  eolInt dx;
  if (!eol_font_initialized())return;
  r = eol_font_get_bounds(text,size);
  switch (justify)
  {
    case eolJustifyLeft:
      dx = x;
      break;
    case eolJustifyCenter:
      dx = (x - (r.w/2));
      break;
    case eolJustifyRight:
      dx = (x - r.w);
      break;
  }
  eol_font_draw_text(
    text,
    dx,
    y,
    color,
    alpha,
    size
  );
}

void eol_font_draw_text(
    char   * text,
    eolInt   x,
    eolInt   y,
    eolVec3D  color,
    eolFloat alpha,
    eolUint   size
  )
{
  if (!eol_font_initialized())return;
  if (size >= eolFontSizeMax)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_font: passed a font size outside of available range");
    return;
  }
  eol_font_draw_text_custom(
    text,
    x,
    y,
    color,
    alpha,
    _eol_font_size[size]
  );
}
  

void eol_font_draw_text_custom(
    char    *text,
    eolInt   x,
    eolInt   y,
    eolVec3D color,
    eolFloat alpha,
    eolFont *font
  )
{
  eolVec3D pos,pos2;
  int w,h;
  GLuint image;
  SDL_Surface *temp1 = NULL;
  SDL_Surface *temp = NULL;
  SDL_Surface *fontpic = NULL;
  SDL_Color colortype;
  eolGraphicsView view;
  if (!eol_font_initialized())return;
  if((text == NULL) || 
      (strlen(text) <= 0))
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_font:passed empty string for text draw.\n");
    return;
  }
  if (font == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_font: no font provided for draw.\n");
    return;
  }
  if (font->_font == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_font: bad eolFont provided for draw.\n");
    return;
  }
  eol_graphics_get_view(&view);
  colortype.r = 255 * color.x;
  colortype.g = 255 * color.y;
  colortype.b = 255 * color.z;
  colortype.unused = SDL_ALPHA_OPAQUE;
  temp = TTF_RenderText_Blended(font->_font, text,colortype);
  if (temp == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_font: unable to render text from SDL_ttf\n");
      return;
  }
  w = temp->w;
  h = temp->h;
  /*Creates an opengl compatable RGBA surface*/
  fontpic = SDL_CreateRGBSurface(SDL_HWSURFACE,w, h,view.depth,view.rMask,view.gMask,view.bMask,view.aMask);	
  /*Copies pixel data from the image to surface*/
  temp1 = SDL_DisplayFormatAlpha(temp);
  SDL_SetAlpha(temp1, 0, 0 );
  SDL_BlitSurface(temp1, NULL, fontpic, NULL);	
  SDL_FreeSurface(temp1);
  SDL_FreeSurface(temp);

  glGenTextures(1, &image);
  glBindTexture(GL_TEXTURE_2D,image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontpic->pixels); 
  SDL_FreeSurface(fontpic);
  glEnable(GL_TEXTURE_2D);
	
  x=x+w;
  y=y+h;

  eol_opengl_get_gl_coordinate(
    x,
    y,
    EOL_2DPLANE_,
    view.modelView,
    view.projection,
    view.viewPort,
    &pos.x,
    &pos.y,
    &pos.z
  );
  eol_opengl_get_gl_coordinate(
    x-w,
    y-h,
    EOL_2DPLANE_,
    view.modelView,
    view.projection,
    view.viewPort,
    &pos2.x,
    &pos2.y,
    &pos2.z
  );


  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBegin( GL_QUADS );

  glTexCoord2f(0.0,0.0);
  glVertex3f(pos2.x,pos2.y,pos.z);

  glTexCoord2f(0.0,1.0);
  glVertex3f( pos2.x,pos.y,pos.z);
	
  glTexCoord2f(1.0,1.0);
  glVertex3f( pos.x,pos.y,pos.z);
	
  glTexCoord2f(1.0,0.0);
  glVertex3f( pos.x,pos2.y,pos.z);
	
  glEnd( );
  glEnable(GL_DEPTH_TEST);
  
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glDeleteTextures(1, &image);
}

void eol_font_crop_string(char *text,int length,int strl)
{
  int i;
  for(i = 0;i < strl - length;i++)
  {
    text[i] = text[i + length];
  }
  text[i - 1] = '\0';/*null terminate in case its overwritten*/
}


void eol_font_draw_text_block(
    char   * text,
    eolInt   x,
    eolInt   y,
    eolInt   w,
    eolInt   h,
    eolVec3D color,
    eolFloat alpha,
    eolInt   size
  )
{
  if (!eol_font_initialized())return;
  if (size >= eolFontSizeMax)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_font: passed a font size outside of available range");
    return;
  }
  eol_font_draw_text_block_custom(
    text,
    eol_rect(x,y,w,h),
    color,
    alpha,
    _eol_font_size[size]
  );
}

void eol_font_draw_text_block_custom(
    char   * thetext,
    eolRect  block,
    eolVec3D color,
    eolFloat alpha,
    eolFont *font
  )
{
  eolText textline;
  eolText temptextline;
  eolText text;
  eolLine word;
  int drawheight = block.y;
  int w,h;
  int i;
  int space;
  int lindex = 0;
  if (!eol_font_initialized())return;
  if ((thetext == NULL)||(thetext[0] == '\0'))
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_font: no text provided for draw.\n");
    return;
  }
  if (font == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_font: no font provided for draw.\n");
    return;
  }
  if (font->_font == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_font: bad eolFont provided for draw.\n");
    return;
  }
  strncpy(text,thetext,EOLTEXTLEN);
  temptextline[0] = '\0';
  do
  {
    space = 0;
    i = 0;
    do
    {
      if(sscanf(&text[i],"%c",&word[0]) == EOF)break;
      if(word[0] == ' ')space++;
      i++;
    }while (word[0] == ' ');
    if (sscanf(text,"%s",word) == EOF)
    {
      block.y=drawheight+h;
      eol_font_draw_text_custom(temptextline,block.x,block.y, color, alpha, font);
      return;
    }

    eol_font_crop_string(text,strlen(word) + space,EOLTEXTLEN);
    strncpy(textline,temptextline,EOLTEXTLEN);/*keep the last line that worked*/
    for (i = 0;i < (space - 1);i++)
    {
      sprintf(temptextline,"%s%c",temptextline,' '); /*add spaces*/
    }
    sprintf(temptextline,"%s %s",temptextline,word); /*add a word*/
    TTF_SizeText(font->_font, temptextline, &w, &h); /*see how big it is now*/
    lindex += strlen(word);
    if(w > block.w)         /*see if we have gone over*/
    {
      block.y=drawheight+h;
      eol_font_draw_text_custom(textline,block.x,block.y, color, alpha, font);

      /*draw the line and get ready for the next line*/
      drawheight += h;
      if (block.h != 0)
      {
        if ((drawheight + h) > block.h)
        {
          break;
        }
      }
      sprintf(temptextline,"%s",word); /*add a word*/
    }
  }while(1);
}

eolRect eol_font_get_bounds(
    char * text,
    eolInt size
  )
{
  eolRect r = {0,0,0,0};
  if (!eol_font_initialized())return r;
  if (size >= eolFontSizeMax)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_font: passed a font size outside of available range");
    return r;
  }
  return eol_font_get_bounds_custom(
    text,
    _eol_font_size[size]
  );
}

eolRect eol_font_get_bounds_custom(
    char    * text,
    eolFont * font
  )
{
  eolRect r = {0,0,0,0};
  if (!eol_font_initialized())return r;
  if ((text == NULL)||(text[0] == '\0'))
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_font: no text provided for draw.\n");
    return r;
  }
  if (font == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_font: no font provided for draw.\n");
    return r;
  }
  TTF_SizeText(font->_font,text, (int *)&r.w, (int *)&r.h);
  return r;
}

eolRect eol_font_get_block_bounds(
    char * text,
    eolInt size,
    eolUint w,
    eolUint h
  )
{
  eolRect r = {0,0,0,0};
  if (!eol_font_initialized())return r;
  if (size >= eolFontSizeMax)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_font: passed a font size outside of available range");
    return r;
  }
  return eol_font_get_block_bounds_custom(
    text,
    _eol_font_size[size],
    w,
    h
  );
}
  


eolRect eol_font_get_block_bounds_custom(
    char    * thetext,
    eolFont * font,
    eolUint   w,
    eolUint   h
  )
{
  eolRect r = {0,0,0,0};
  eolText textline;
  eolText temptextline;
  eolText text;
  eolLine word;
  int tw = 0, th = 0;
  int drawheight = 0;
  int i = 0;
  int space = 0;
  int lindex = 0;
  if (!eol_font_initialized())return r;
  if((thetext == NULL)||(thetext[0] == '\0'))
  {
    return r;
  }
  if (font == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_font: no font provided for draw.\n");
    return r;
  }

  strncpy(text,thetext,EOLTEXTLEN);
  temptextline[0] = '\0';
  do
  {
    space = 0;
    i = 0;
    do
    {
      if(sscanf(&text[i],"%c",&word[0]) == EOF)break;
      if(word[0] == ' ')space++;
      i++;
    }while(word[0] == ' ');
    
    if(sscanf(text,"%s",word) == EOF)
    {
      break;
    }
    eol_font_crop_string(text,strlen(word) + 1,EOLTEXTLEN);
    strncpy(textline,temptextline,EOLTEXTLEN);/*keep the last line that worked*/
    for(i = 0;i < (space - 1);i++)
    {
      sprintf(temptextline,"%s%c",temptextline,' '); /*add spaces*/
    }
    sprintf(temptextline,"%s %s",temptextline,word); /*add a word*/
    TTF_SizeText(font->_font, temptextline, &tw, &th); /*see how big it is now*/
    lindex += strlen(word);
    if(tw > w)         /*see if we have gone over*/
    {
      drawheight += th;
      if (h != 0)
      {
        if ((drawheight + th) > h)
        {
          break;
        }
      }
      sprintf(temptextline,"%s",word); /*add a word*/
    }
    else if (tw > r.w)
    {
      r.w = tw;
    }
  }while(1);
  r.h = drawheight + h;
  return r;
}


/*eol@eof*/

