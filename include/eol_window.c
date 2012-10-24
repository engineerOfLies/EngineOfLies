#include "eol_window.h"
#include "eol_sprite.h"
#include "eol_component.h"
#include "eol_resource.h"
#include "eol_logger.h"
#include "eol_config.h"
#include "eol_font.h"
#include "eol_input.h"
#include "eol_mouse.h"

/*local global variables*/
eolBool              _eol_window_initialized = eolFalse;
eolResourceManager * _eol_window_manager = NULL;
eolUint              _eol_window_max = 0;
/**<its not really a stack as per the data structure, just a draw order*/
GList              * _eol_window_stack = NULL;
eolLine              _eol_window_sprite_files[9];
eolSprite          * _eol_window_sprites[9];
#define _eol_window_sprite_count  9

/*local function prototypes*/
void eol_window_delete(void *win);
eolBool eol_window_load_data_from_file(char * filename,void *data);
void eol_window_draw_generic(eolWindow *win);

/*definitions*/
void eol_window_init()
{
  int i;
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_window:initializing\n");
  _eol_window_manager = eol_resource_manager_init(
    "eol_window_manager",
    _eol_window_max,
    sizeof(eolWindow),
    eolTrue,
    eol_window_delete,
    eol_window_load_data_from_file
    );
  atexit(eol_window_close);
  for (i = 0; i < _eol_window_sprite_count;i++)
  {
    if (strlen(_eol_window_sprite_files[i]) > 0)
    {
      _eol_window_sprites[i] = eol_sprite_load(_eol_window_sprite_files[i],-1,-1);
    }
  }
  _eol_window_initialized = eolTrue;
  eol_component_config();
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_window:initialized\n");
}

eolBool eol_window_initialized()
{
  return _eol_window_initialized;
}


void eol_window_config()
{
  /*TODO load from config*/
  _eol_window_max = 16;
  strcpy(_eol_window_sprite_files[0],"images/UI/windowbackground1.png");
  strcpy(_eol_window_sprite_files[1],"images/UI/windowborder_corner_UL.png");
  strcpy(_eol_window_sprite_files[2],"images/UI/windowborder_corner_UR.png");
  strcpy(_eol_window_sprite_files[3],"images/UI/windowborder_corner_BL.png");
  strcpy(_eol_window_sprite_files[4],"images/UI/windowborder_corner_BR.png");
  strcpy(_eol_window_sprite_files[5],"images/UI/windowborder_horiz.png");
  strcpy(_eol_window_sprite_files[6],"images/UI/windowborder_vert.png");
  strcpy(_eol_window_sprite_files[7],"images/UI/windowborder_horiz.png");
  strcpy(_eol_window_sprite_files[8],"images/UI/windowborder_vert.png");
}

void eol_window_clean()
{
  if (!eol_window_initialized())
  {
    eol_logger_message(
        EOL_LOG_INFO,
        "eol_window:cannot clean, not initialized\n");
    return;
  }
  if (!_eol_window_manager)return;
  eol_resource_manager_clean(_eol_window_manager);
}

void eol_window_close()
{
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_window:closing\n"
  );
  eol_resource_manager_free(&_eol_window_manager);
  _eol_window_max = 0;
  _eol_window_initialized = eolFalse;
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_window:closed\n"
  );
}

void eol_window_draw_all()
{
  GList *l = NULL;
  GList *c = NULL;
  eolWindow *win;
  if (!eol_window_initialized())return;
  for (l = _eol_window_stack;l != NULL; l = l->next)
  {
    win = (eolWindow*)l->data;
    if (win == NULL)continue;
    if (win->hidden)continue;
    if (win->drawGeneric)
    {
      eol_window_draw_generic(win);
    }
    if (win->draw != NULL)
    {
      win->draw(win);
    }
    for (c = win->components;c != NULL;c = c->next)
    {
      if (c->data == NULL)continue;
      eol_component_draw((eolComponent *)c->data,win->rect);
    }
  }
}

void eol_window_update_all()
{
  GList *l = NULL;
  GList *c = NULL;
  GList *update = NULL;
  eolBool updateHandled = eolFalse;
  eolWindow *win;
  if (!eol_window_initialized())return;
  l = g_list_last(_eol_window_stack);
  for (;l != NULL;l = l->prev)
  {
    win = (eolWindow*)l->data;
    if ((win != NULL)&&(win->update != NULL)&& (!win->sleeping))
    {
      /*update all components*/
      for (c = win->components;c != NULL; c= c->next)
      {
        if (c->data != NULL)
        {
          if (eol_component_update(c->data))
          {
            update = g_list_append(update,c->data);
          }
        }
      }
    }
    /*call update for window*/
    updateHandled = win->update(win,update);
    if (update != NULL)
    {
      g_list_free(update);
      update = NULL;
    }
    if ((updateHandled) || (!win->passesInput))break;
  }
}

eolWindow *eol_window_new()
{
  eolWindow *win = NULL;
  if (!eol_window_initialized())
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_window:used uninitialized\n");
    return NULL;
  }
  win = (eolWindow *)eol_resource_new_element(_eol_window_manager);
  if (win == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_window:failed to get new resource\n");
    return NULL;
  }
  /*new windows are added at the top of the stack, ie: last to draw*/
  win->id = eol_resource_element_get_id(_eol_window_manager,win);
  _eol_window_stack = g_list_append(_eol_window_stack,win);
  return win;
}

void eol_window_free(eolWindow **win)
{
  if (!eol_window_initialized())return;
  _eol_window_stack = g_list_remove(_eol_window_stack,*win);
  eol_resource_free_element(_eol_window_manager,(void **)win);
}

void eol_window_delete(void *window)
{
  GList *l = NULL;
  eolWindow *win;
  if (!eol_window_initialized())return;
  if (window)return;
  win = (eolWindow *)window;
  if (win->customData != NULL)
  {
    if (win->custom_delete != NULL)
    {
      win->custom_delete(win->customData);
    }
  }
  if (win->components != NULL)
  {
    for (l = win->components;l != NULL;l = l->next)
    {
      if (l->data != NULL)
      {
        eol_component_free((eolComponent**)&l->data);
      }
    }
    g_list_free(win->components);
    win->components = NULL;
  }
  if (win->callbacks != NULL)
  {
    free(win->callbacks);
  }
  _eol_window_stack = g_list_remove(_eol_window_stack,win);
  memset(win, 0, sizeof(eolWindow));
}

void eol_window_load_label(eolWindow *win,eolKeychain *def)
{
  eolUint       id;
  eolLine       text;
  eolLine       name;
  eolLine       justify;
  eolLine       wordWrap;
  eolUint       fontSize;
  eolVec3D      color = {1,1,1};
  eolFloat      alpha = 1;
  eolLine       fontfile; 
  eolRectFloat  rect;
  eolComponent *comp;
  char        * font = NULL;
  
  eol_line_cpy(fontfile,"\0");
  eol_line_cpy(justify,"\0");
  eol_line_cpy(wordWrap,"\0");
  if ((!win) || (!def))return;
  
  eol_keychain_get_hash_value_as_uint(&id, def, "id");
  eol_keychain_get_hash_value_as_uint(&fontSize, def, "fontSize");
  eol_keychain_get_hash_value_as_rectfloat(&rect, def, "rect");
  eol_keychain_get_hash_value_as_line(text, def, "text");
  eol_keychain_get_hash_value_as_line(name, def, "name");
  eol_keychain_get_hash_value_as_line(justify, def, "justify");
  eol_keychain_get_hash_value_as_line(fontfile, def, "fontName");
  eol_keychain_get_hash_value_as_line(wordWrap, def, "wordWrap");
  eol_keychain_get_hash_value_as_vec3d(&color, def, "color");
  eol_keychain_get_hash_value_as_float(&alpha, def, "alpha");
  
  if (strlen(fontfile) > 0)font = fontfile;
  comp = eol_label_new(
    id,
    name,
    rect,
    win->rect,
    eolTrue,
    text,
    eol_font_justify_from_string(justify),
    eol_true_from_string(wordWrap),
    fontSize,
    font,
    color,
    alpha
  );
  eol_window_add_component(win,comp);
}

void eol_window_load_percent_bar(eolWindow *win,eolKeychain *def)
{
  eolComponent *comp = NULL;
  eolUint       id;
  eolLine       name;
  eolRectFloat  rect;
  eolBool     vertical = eolFalse;
  eolBool     ascending = eolFalse;
  eolFloat    percent = 0;
  eolVec3D    statusColor = {1,1,1};
  eolVec3D    backgroundColor = {0,0,0};
  eolFloat    statusAlpha = 1;
  eolFloat    backgroundAlpha = 1;

  if ((!win) || (!def))return;

  eol_keychain_get_hash_value_as_line(name, def, "name");
  eol_keychain_get_hash_value_as_uint(&id, def, "id");
  eol_keychain_get_hash_value_as_rectfloat(&rect, def, "rect");
  eol_keychain_get_hash_value_as_bool(&vertical, def, "vertical");
  eol_keychain_get_hash_value_as_bool(&ascending, def, "ascending");
  eol_keychain_get_hash_value_as_float(&percent, def, "percent");
  eol_keychain_get_hash_value_as_vec3d(&statusColor, def, "statusColor");
  eol_keychain_get_hash_value_as_vec3d(&backgroundColor, def, "backgroundColor");
  eol_keychain_get_hash_value_as_float(&statusAlpha, def, "statusAlpha");
  eol_keychain_get_hash_value_as_float(&backgroundAlpha, def, "backgroundAlpha");

  comp = eol_percent_bar_new(
    id,
    name,
    rect,
    win->rect,
    vertical,
    ascending,
    percent,
    statusColor,
    backgroundColor,
    statusAlpha,
    backgroundAlpha
  );
  
  eol_window_add_component(win,comp);
}

void eol_window_load_list(eolWindow *win,eolKeychain *def)
{
  eolComponent *comp = NULL;

  if (!win)return;
  comp = eol_list_create_from_config(win->rect,def);
  if (!comp)return;
  eol_window_add_component(win,comp);
}

void eol_window_load_slider(eolWindow *win,eolKeychain *def)
{
  eolComponent *comp = NULL;
  eolUint        id;
  eolLine        name;
  eolRectFloat   rect;
  eolLine        vertical;
  eolVec3D       barColor;
  eolLine        sliderType;
  
  if ((!win) || (!def))return;
  eol_keychain_get_hash_value_as_line(name, def, "name");
  eol_keychain_get_hash_value_as_uint(&id, def, "id");
  eol_keychain_get_hash_value_as_rectfloat(&rect, def, "rect");
  eol_keychain_get_hash_value_as_line(vertical, def, "vertical");
  eol_keychain_get_hash_value_as_vec3d(&barColor, def, "barColor");
  eol_keychain_get_hash_value_as_line(sliderType, def, "sliderType");
  
  if (eol_line_cmp(sliderType,"COMMON") == 0)
  {
    comp = eol_slider_common_new(
      id,
      name,
      rect,
      win->rect,
      eol_true_from_string(vertical),
      barColor,
      0
    );
    eol_window_add_component(win,comp);
  }
}

void eol_window_load_button(eolWindow *win,eolKeychain *def)
{
  eolComponent *comp;
  comp = eol_component_button_load(win->rect,def);
  eol_window_add_component(win,comp);
  return;
}

eolBool eol_window_load_data_from_file(char * filename,void *data)
{
  eolWindow *window;
  eolConfig *conf;
  eolKeychain *chain, *item;
  int i;
  eolLine typecheck;
  eolRectFloat tempr;
  eolUint sw,sh;
  if (!data)return eolFalse;
  window = (eolWindow *)data;
  conf = eol_config_load(filename);
  if (!conf)return eolFalse;
  eol_graphics_get_size(&sw,&sh);
  eol_config_get_line_by_tag(window->name,conf,"name");
  eol_config_get_rectfloat_by_tag(&tempr,conf,"rect");
  if (tempr.x < 0)
  {
    tempr.x = sw + tempr.x;
  }
  if (tempr.y < 0)
  {
    tempr.y = sh + tempr.y;
  }
  if (tempr.w <= 0)
  {
    tempr.w = sw - tempr.w - tempr.x;
  }
  if (tempr.h <= 0)
  {
    tempr.h = sh - tempr.h - tempr.y;
  }
  window->rect.x = tempr.x;
  window->rect.y = tempr.y;
  window->rect.w = tempr.w;
  window->rect.h = tempr.h;
  eol_config_get_bool_by_tag(&window->canHasFocus,conf,"canHasFocus");
  eol_config_get_bool_by_tag(&window->drawGeneric ,conf,"drawGeneric");
  eol_config_get_bool_by_tag(&window->passesInput ,conf,"passesInput");
  
  /*add components*/
  if (eol_config_get_keychain_by_tag(&chain,conf,"components"))
  {
    if (chain != NULL)
    {
      if (chain->keyType == eolKeychainList)
      {
        for (i = 0; i < chain->itemCount; i++)
        {
          item = eol_keychain_get_list_nth(chain, i);
          if (item != NULL)
          {
            eol_keychain_get_hash_value_as_line(typecheck, item, "type");
            if (eol_line_cmp(typecheck,"BUTTON") == 0)
            {
              eol_window_load_button(window,item);
              continue;
            }
            if (eol_line_cmp(typecheck,"LABEL") == 0)
            {
              eol_window_load_label(window,item);
              continue;
            }
            if (eol_line_cmp(typecheck,"SLIDER") == 0)
            {
              eol_window_load_slider(window,item);
              continue;
            }
            if (eol_line_cmp(typecheck,"PERCENT") == 0)
            {
              eol_window_load_percent_bar(window,item);
              continue;
            }
            if (eol_line_cmp(typecheck,"LIST") == 0)
            {
              eol_window_load_list(window,item);
              continue;
            }
          }
        }        
      }
    }
  }                                      
  
  eol_config_free(&conf);
  return eolTrue;
}

void eol_window_free_if_outside_click(eolWindow **win)
{
  if ((!win) || (!*win))return;
  if ((eol_mouse_input_state(eolMouseLeft)) ||
    (eol_mouse_input_state(eolMouseRight)))
  {
    if (!eol_mouse_in_rect((*win)->rect))
    {
      eol_window_free(win);
    }
  }
}

void eol_window_draw_generic(eolWindow *win)
{
  eolFloat scaleX = 1,scaleY = 1;
  if (!win)return;
  if (!eol_window_initialized())return;
  /*draw the background*/
  if ((_eol_window_sprites[0] != NULL) &&
    (_eol_window_sprites[0]->frameWidth != 0) &&
    (_eol_window_sprites[0]->frameHeight != 0))
  {
    scaleX = win->rect.w / (float)_eol_window_sprites[0]->frameWidth;
    scaleY = win->rect.h / (float)_eol_window_sprites[0]->frameHeight;
    eol_sprite_draw_transformed(_eol_window_sprites[0],0,win->rect.x,win->rect.y,
                                scaleX,scaleY,0,0,0,
                                eol_vec3d(1,1,1),1);
  }
  /*draw the borders*/
  /*top*/
  if ((_eol_window_sprites[5] != NULL) &&
    (_eol_window_sprites[5]->frameWidth != 0) &&
    (_eol_window_sprites[5]->frameHeight != 0))
  {
    scaleX = win->rect.w / (float)_eol_window_sprites[5]->frameWidth;
    eol_sprite_draw_transformed(_eol_window_sprites[5],0,win->rect.x,
                                win->rect.y - _eol_window_sprites[5]->frameHeight,
                                scaleX,1,0,0,0,
                                eol_vec3d(1,1,1),1);
  }
  /*left*/
  if ((_eol_window_sprites[6] != NULL) &&
    (_eol_window_sprites[6]->frameWidth != 0) &&
    (_eol_window_sprites[6]->frameHeight != 0))
  {
    scaleY = win->rect.h / (float)_eol_window_sprites[6]->frameHeight;
    eol_sprite_draw_transformed(_eol_window_sprites[6],0,
                                win->rect.x - _eol_window_sprites[6]->frameWidth,
                                win->rect.y,
                                1,scaleY,0,0,0,
                                eol_vec3d(1,1,1),1);
  }
  /*bottom*/
  if ((_eol_window_sprites[7] != NULL) &&
    (_eol_window_sprites[7]->frameWidth != 0) &&
    (_eol_window_sprites[7]->frameHeight != 0))
  {
    scaleX = win->rect.w / (float)_eol_window_sprites[7]->frameWidth;
    eol_sprite_draw_transformed(_eol_window_sprites[7],0,win->rect.x,
                                win->rect.y + win->rect.h,
                                scaleX,1,0,0,0,
                                eol_vec3d(1,1,1),1);
  }
  /*right*/
  if ((_eol_window_sprites[8] != NULL) &&
    (_eol_window_sprites[8]->frameWidth != 0) &&
    (_eol_window_sprites[8]->frameHeight != 0))
  {
    scaleY = win->rect.h / (float)_eol_window_sprites[8]->frameHeight;
    eol_sprite_draw_transformed(_eol_window_sprites[8],0,
                                win->rect.x + win->rect.w,
                                win->rect.y,
                                1,scaleY,0,0,0,
                                eol_vec3d(1,1,1),1);
  }
  
  /*draw the corner stones*/
  /*UL*/
  if ((_eol_window_sprites[1] != NULL) &&
    (_eol_window_sprites[1]->frameWidth != 0) &&
    (_eol_window_sprites[1]->frameHeight != 0))
  {
    eol_sprite_draw(_eol_window_sprites[1],
                    0,
                    (win->rect.x - _eol_window_sprites[1]->frameWidth),
                    (win->rect.y - _eol_window_sprites[1]->frameHeight));
  }
  /*UR*/
  if ((_eol_window_sprites[2] != NULL) &&
    (_eol_window_sprites[2]->frameWidth != 0) &&
    (_eol_window_sprites[2]->frameHeight != 0))
  {
    eol_sprite_draw(_eol_window_sprites[2],
                    0,
                    (win->rect.x + win->rect.w),
                    (win->rect.y - _eol_window_sprites[2]->frameHeight));
  }
  /*BL*/
  if ((_eol_window_sprites[3] != NULL) &&
    (_eol_window_sprites[3]->frameWidth != 0) &&
    (_eol_window_sprites[3]->frameHeight != 0))
  {
    eol_sprite_draw(_eol_window_sprites[3],
                    0,
                    (win->rect.x - _eol_window_sprites[3]->frameWidth),
                    (win->rect.y + win->rect.h));
  }
  /*BR*/
  if ((_eol_window_sprites[4] != NULL) &&
    (_eol_window_sprites[4]->frameWidth != 0) &&
    (_eol_window_sprites[4]->frameHeight != 0))
  {
    eol_sprite_draw(_eol_window_sprites[4],
                    0,
                    (win->rect.x + win->rect.w),
                    (win->rect.y + win->rect.h));
  }
}

void eol_window_add_component(eolWindow *win,eolComponent *comp)
{
  if ((win == NULL)||(comp == NULL)||(!eol_window_initialized()))
  {
    return;
  }
  win->components = g_list_append(win->components,comp);
  if (win->components == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_window:unable to add new component!\n");
  }
}

eolComponent *eol_window_get_component_by_id(eolWindow *win,eolUint id)
{
  GList *c = NULL;
  eolComponent *comp;
  if (!win)return NULL;
  for (c = win->components;c != NULL;c = c->next)
  {
    if (c->data != NULL)
    {
      comp = (eolComponent *)c->data;
      if (comp->id == id)return comp;
    }
  }
  return NULL;
}

eolComponent *eol_window_get_component_by_name(eolWindow *win,eolLine name)
{
  GList *c = NULL;
  eolComponent *comp;
  if (!win)return NULL;
  for (c = win->components;c != NULL;c = c->next)
  {
    if (c->data != NULL)
    {
      comp = (eolComponent *)c->data;
      if (eol_line_cmp(comp->name,name) == 0)return comp;
    }
  }
  return NULL;
}

void eol_window_allocat_callbacks(eolWindow *win,eolUint count)
{
  if (!win)return;
  if (win->callbacks != NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_window:attempted to re-allocated window callbacks for window %s\n",
      win->name);
    return;
  }
  win->callbacks = (eolWindowCallback*)malloc(sizeof(eolWindowCallback)*count);
  if (win->callbacks == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_window:cannot allocate callbacks for window %s\n",
      win->name);
    return;
  }
  memset(win->callbacks,0,sizeof(eolWindowCallback)*count);
  win->callbackCount = count;
}

eolFloat eol_window_get_relative_position(eolInt position,eolUint range)
{
  if (range == 0)return 0;
  return (eolFloat)position/(eolFloat)range;
}

eolUint eol_window_get_refcount(eolWindow * window)
{
  return eol_resource_element_get_refcount(_eol_window_manager,window);
}

void eol_window_move_to(eolWindow *win,eolVec2D position)
{
  GList *c = NULL;
  if (!win)return;
  win->rect.x = position.x;
  win->rect.y = position.y;
  for (c = win->components;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    eol_component_move((eolComponent *)c->data,win->rect);
  }
}

eolWindow * eol_window_load_from_file_to(eolLine file,eolVec2D position)
{
  eolWindow * win;
  win = eol_window_load_from_file(file);
  eol_window_move_to(win,position);
  return win;
}

eolWindow * eol_window_load_from_file(eolLine file)
{
  eolWindow *window;
  eolLine filename;
  if (!eol_window_initialized())
  {
    return NULL;
  }
  window =
  (eolWindow *)eol_resource_manager_load_resource(_eol_window_manager,file);
  if (window == NULL)return NULL;
  window->id = eol_resource_element_get_id(_eol_window_manager,window);
  eol_resource_element_get_filename(filename, _eol_window_manager,window);
  _eol_window_stack = g_list_append(_eol_window_stack,window);
  return window;
}

void eol_window_hide(eolWindow *win)
{
  if (!win)return;
  win->hidden = eolTrue;
}

void eol_window_show(eolWindow *win)
{
  if (!win)return;
  win->hidden = eolFalse;
}

void eol_window_sleep(eolWindow *win)
{
  if (!win)return;
  win->sleeping = eolTrue;
}

void eol_window_wakeup(eolWindow *win)
{
  if (!win)return;
  win->sleeping = eolFalse;
}


/*eol@eof*/
