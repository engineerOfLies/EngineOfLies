#include "eol_component_button.h"
#include "eol_font.h"
#include "eol_logger.h"
#include "eol_draw.h"
#include "eol_mouse.h"
#include "eol_input.h"

/*local function prototypes*/
eolComponentButton *eol_component_get_button_data(eolComponent *component);
void eol_component_button_free(eolComponent *component);
void eol_component_button_new(eolComponent *component);

/*local global variables*/
eolSprite * _eol_component_stock_button[eolButtonStateMax] = {NULL,NULL,NULL,NULL};
eolVec3D    _eol_component_button_color[eolButtonStateMax];
eolInt      _eol_component_button_offset_x = 0;
eolInt      _eol_component_button_offset_y = 0;
eolBool     _eol_component_button_text_outline = eolTrue;
eolFloat    _eol_component_button_alpha = 1;

/*function definitions*/

void eol_button_configure(eolConfig *conf)
{
  eolLine buf;
  eolLine buttonfile,buttonhitfile,buttonhighfile,buttonsleepfile;
  _eol_component_button_offset_x = 0;
  _eol_component_button_offset_y = 0;
  eol_vec3d_set(_eol_component_button_color[0],0.8,0.8,0.8);
  eol_vec3d_set(_eol_component_button_color[1],1,1,0);
  eol_vec3d_set(_eol_component_button_color[2],0.6,0.6,0.6);
  eol_vec3d_set(_eol_component_button_color[3],0.3,0.2,0.3);
  eol_line_cpy(buttonfile,"images/UI/btn.png");
  eol_line_cpy(buttonhitfile,"images/UI/btn_hit.png");
  eol_line_cpy(buttonhighfile,"images/UI/btn_high.png");
  eol_line_cpy(buttonsleepfile,"images/UI/btn_sleep.png");
  if (conf != NULL)
  {
    eol_config_get_int_by_tag(&_eol_component_button_offset_x,conf,"button_x_offset");
    eol_config_get_int_by_tag(&_eol_component_button_offset_y,
                              conf,
                              "button_y_offset");
    eol_config_get_line_by_tag(buf,conf,"button_file");
    if (strlen(buf) > 0)
    {
      eol_line_cpy(buttonfile,buf);
    }
    eol_config_get_line_by_tag(buf,conf,"button_high_file");
    if (strlen(buf) > 0)
    {
      eol_line_cpy(buttonhighfile,buf);
    }
    eol_config_get_line_by_tag(buf,conf,"button_hit_file");
    if (strlen(buf) > 0)
    {
      eol_line_cpy(buttonhitfile,buf);
    }
    eol_config_get_line_by_tag(buf,conf,"button_sleep_file");
    if (strlen(buf) > 0)
    {
      eol_line_cpy(buttonsleepfile,buf);
    }
    eol_config_get_vec3d_by_tag(&_eol_component_button_color[0],conf,"button_text_color");
    eol_config_get_vec3d_by_tag(&_eol_component_button_color[1],conf,"button_high_text_color");
    eol_config_get_vec3d_by_tag(&_eol_component_button_color[2],conf,"button_hit_text_color");
    eol_config_get_vec3d_by_tag(&_eol_component_button_color[3],conf,"button_sleep_text_color");
    eol_config_get_bool_by_tag(&_eol_component_button_text_outline,conf,"button_text_outline");
    eol_config_get_float_by_tag(&_eol_component_button_alpha,conf,"button_alpha");
  }
  _eol_component_stock_button[0] = eol_sprite_load(buttonfile,-1,-1);
  _eol_component_stock_button[1] = eol_sprite_load(buttonhighfile,-1,-1);
  _eol_component_stock_button[2] = eol_sprite_load(buttonhitfile,-1,-1);
  _eol_component_stock_button[3] = eol_sprite_load(buttonsleepfile,-1,-1);
}

void eol_button_set_inactive(eolComponent *button)
{
  if (!eol_component_get_button_data(button))return;
  button->state = eolButtonSleep;
}

void eol_button_set_active(eolComponent *button)
{
  if (!eol_component_get_button_data(button))return;
  if (button->state == eolButtonSleep)
    button->state = eolButtonIdle;
}

void eol_button_set_text(eolComponent *button,eolLine newText)
{
  eolComponentButton *buttonData = NULL;
  buttonData = eol_component_get_button_data(button);
  if (!buttonData)return;
  eol_line_cpy(buttonData->buttonText,newText);
}

void eol_button_get_stock_size(eolUint *w, eolUint *h)
{
  if (_eol_component_stock_button[0] == NULL)return;
  if (w)*w = _eol_component_stock_button[0]->frameWidth;
  if (h)*h = _eol_component_stock_button[0]->frameHeight;
}

eolComponentButton *eol_component_get_button_data(eolComponent *component)
{
  if ((!component)||
    (!component->componentData)||
    (component->type != eolButtonComponent))
  {
    return NULL;
  }
  return (eolComponentButton*)component->componentData;
}

void eol_component_button_free(eolComponent *component)
{
  eolComponentButton *button;
  int i;
  button = eol_component_get_button_data(component);
  if (button)return;
  for (i = 0; i < eolButtonStateMax; i++)
  {
    if (button->button[i] != NULL)
    {
      eol_sprite_free(&button->button[i]);
    }
  }
  free(button);
  button = NULL;
  component->componentData = NULL;
}

eolBool eol_component_button_update(eolComponent *component)
{
  eolVec2D v;
  eolInt x,y;
  eolBool mod = eolTrue;
  eolComponentButton *button = NULL;
  if (!component)return eolFalse;
  button = eol_component_get_button_data(component);
  if (!button)return eolFalse;
  if (component->state == eolButtonSleep)return eolFalse;

  component->oldState = component->state;
  eol_mouse_get_position(&x,&y);
  v.x = x;
  v.y = y;
  if (button->hotkeymod)
  {
    mod = eol_input_is_mod_held(button->hotkeymod);
  }
  if ((button->input > 0) && (mod))
  {
    if (eol_input_is_key_pressed(button->input))
    {
      component->state = eolButtonPressed;
      return eolFalse;
    }
    if (eol_input_is_key_held(button->input))
    {
      component->state = eolButtonPressed;
      return eolFalse;
    }
    if (eol_input_is_key_released(button->input))
    {
      component->state = eolButtonIdle;
      return eolTrue;
    }
  }
  if (eol_vec_in_rect(v,component->bounds))
  {
    component->state = eolButtonHighlight;
    if (eol_mouse_input_held(eolMouseLeft))
    {
      component->state = eolButtonPressed;
      return eolFalse;
    }
    if (eol_mouse_input_pressed(eolMouseLeft))
    {
      component->state = eolButtonPressed;
      return eolFalse;
    }
    else if (eol_mouse_input_released(eolMouseLeft))
    {
      component->state = eolButtonIdle;
      return eolTrue;
    }
    return eolFalse;
  }
  component->state = eolButtonIdle;
  return eolFalse;
}

void eol_component_button_draw_rect(eolComponent *component,eolRect bounds)
{
  eolRect r;
  eolComponentButton *button = NULL;
  button = eol_component_get_button_data(component);
  if (!button)return;
  eol_draw_solid_rect(component->bounds,button->backgroundColor,button->backgroundAlpha);
  switch (component->state)
  {
    case eolButtonIdle:
      break;
    case eolButtonHighlight:
      eol_draw_solid_rect(component->bounds,button->highlightColor,0.3);
      break;
    case eolButtonPressed:
      eol_draw_solid_rect(component->bounds,button->pressColor,0.3);
      break;
  }
  
  eol_font_draw_text_justify(
    button->buttonText,
    component->bounds.x + 2,
    component->bounds.y + 2,
    button->textColor[component->state],
    button->alpha,
    button->fontSize,
    button->justify
  );

  eol_rect_copy(&r,component->bounds);
  r.x++;
  r.y++;
  r.h-=2;
  r.w-=2;
  eol_draw_rect(component->bounds,eol_vec3d(1,1,1),1);
  eol_draw_rect(r,eol_vec3d(1,1,1),1);
}

void eol_component_button_draw(eolComponent *component,eolRect bounds)
{
  eolRect r;
  eolComponentButton *button = NULL;
  eolSprite *img = NULL;
  eolInt x,y;
  eolUint ofx = 0, ofy = 0;
  button = eol_component_get_button_data(component);
  if (button == NULL)return;
  x = component->bounds.x;
  y = component->bounds.y;
  r = eol_font_get_bounds(
    button->buttonText,
    button->fontSize
  );
  if ((component->state >= 0) &&
    (component->state < eolButtonStateMax))
  {
    img = button->button[component->state];

    if (img != NULL)
    {
      eol_sprite_draw(img,
                      0,
                      x,
                      y);
      x = x + (img->frameWidth/2);
      y = y + (img->frameHeight/2) - (r.h*0.5);
      ofx = _eol_component_button_offset_x;
      ofy = _eol_component_button_offset_y;
    }
    r.x = x + ofx;
    r.y = y + ofy;
    eol_font_draw_text_justify(
      button->buttonText,
      r.x,
      r.y,
      button->textColor[component->state],
      button->alpha,
      button->fontSize,
      button->justify
    );
    if ((button->buttonType == eolButtonText) &&
        (component->state == eolButtonHighlight) &&
        (_eol_component_button_text_outline))
    {
      eol_draw_rect(component->bounds,_eol_component_button_color[component->state],button->alpha);
    }
  }
}

void eol_component_make_button(
    eolComponent * component,
    char         * buttonText,
    eolUint        buttonType,
    eolInt         buttonHotkey,
    eolUint        buttonHotkeymod,
    eolLine        buttonUpFile,
    eolLine        buttonHighFile,
    eolLine        buttonDownFile,
    eolVec3D       backgroundColor,
    eolFloat       backgroundAlpha,
    eolVec3D       highlightColor,
    eolVec3D       pressColor,
    eolUint        fontSize
  )
{
  eolComponentButton * button = NULL;
  if (!component)return;
  eol_component_button_new(component);

  button = eol_component_get_button_data(component);
  if (button == NULL)
  {
    return;
  }
  button->alpha = 1;
  button->fontSize = fontSize;
  button->input = buttonHotkey;
  button->hotkeymod = buttonHotkeymod;

  strncpy(button->buttonText,buttonText,EOLLINELEN);
  button->buttonType = buttonType;
  component->data_update = eol_component_button_update;
  component->data_free = eol_component_button_free;
  component->data_draw = eol_component_button_draw;
  switch(buttonType)
  {
    case eolButtonCustom:
      button->button[eolButtonIdle] = eol_sprite_load(buttonUpFile,-1,-1);
      button->button[eolButtonHighlight] = eol_sprite_load(buttonHighFile,-1,-1);
      button->button[eolButtonPressed] = eol_sprite_load(buttonDownFile,-1,-1);
      button->justify = eolJustifyCenter;
      break;
    case eolButtonText:
      button->button[eolButtonIdle] = NULL;
      button->button[eolButtonHighlight] = NULL;
      button->button[eolButtonPressed] = NULL;
      button->justify = eolJustifyLeft;
      break;
    case eolButtonStock:
      button->button[eolButtonIdle] = _eol_component_stock_button[eolButtonIdle];
      button->button[eolButtonHighlight] = _eol_component_stock_button[eolButtonHighlight];
      button->button[eolButtonPressed] = _eol_component_stock_button[eolButtonPressed];
      button->justify = eolJustifyCenter;
      break;
    case eolButtonRect:
      component->data_draw = eol_component_button_draw_rect;
      eol_vec3d_copy(button->backgroundColor,backgroundColor);
      button->backgroundAlpha = backgroundAlpha;
      button->justify = eolJustifyLeft;
      eol_vec3d_copy(button->highlightColor,highlightColor);
      eol_vec3d_copy(button->pressColor,pressColor);
      break;
  }
}

void eol_component_button_new(eolComponent *component)
{
  if (component->componentData != NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_component:tried to make a button out of an existing component\n");
    return;
  }
  component->componentData = malloc(sizeof(eolComponentButton));
  if (component->componentData == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_actor: failed to allocate data for new button\n");
    return;
  }
  memset(component->componentData,0,sizeof(eolComponentButton));
  component->type = eolButtonComponent;
}

eolComponent *eol_button_stock_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    char         * buttonText,
    eolInt         buttonHotkey,
    eolUint        buttonHotkeymod,
    eolBool        center
  )
{
  return eol_button_new(
    id,
    name,
    rect,
    bounds,
    buttonText,
    3,
    eolButtonStock,
    buttonHotkey,
    buttonHotkeymod,
    center,
    NULL,
    NULL,
    NULL,
    eol_vec3d(0,0,0),
    0,
    eol_vec3d(0,0,0),
    eol_vec3d(0,0,0)
  );
}

eolComponent *eol_button_text_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    char         * buttonText,
    eolUint        fontSize,
    eolInt         buttonHotkey,
    eolUint        buttonHotkeymod,
    eolBool        center
  )
{
  return eol_button_new(
    id,
    name,
    rect,
    bounds,
    buttonText,
    fontSize,
    eolButtonText,
    buttonHotkey,
    buttonHotkeymod,
    center,
    NULL,
    NULL,
    NULL,
    eol_vec3d(0,0,0),
    0,
    eol_vec3d(0,0,0),
    eol_vec3d(0,0,0)
  );
}

void eol_button_move(eolComponent *component,eolRect newbounds)
{
  eolRect r;
  eolComponentButton *button = NULL;
  button = eol_component_get_button_data(component);
  if (!button)return;
  eol_component_get_rect_from_bounds(&component->bounds,newbounds,component->rect);
  switch (button->buttonType)
  {
    case eolButtonCustom:
      if (button->button[eolButtonIdle] != NULL)
      {
        component->bounds.w = button->button[eolButtonIdle]->frameWidth;
        component->bounds.h = button->button[eolButtonIdle]->frameHeight;
      }
      break;
    case eolButtonText:
      r = eol_font_get_bounds(button->buttonText,button->fontSize);
      component->bounds.w = r.w;
      component->bounds.h = r.h;
      break;
    case eolButtonStock:
      if (button->button[eolButtonIdle] != NULL)
      {
        component->bounds.w = button->button[eolButtonIdle]->frameWidth;
        component->bounds.h = button->button[eolButtonIdle]->frameHeight;
      }
      break;
    case eolButtonRect:
      break;
  }
  if (button->centered)
  {
    component->bounds.x -= component->bounds.w/2;
    component->bounds.y -= component->bounds.h/2;
  }
}

eolComponent *eol_button_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    char         * buttonText,
    eolUint        fontSize,
    eolUint        buttonType,
    eolInt         buttonHotkey,
    eolUint        buttonHotkeymod,
    eolBool        center,
    char         * buttonFileUp,
    char         * buttonFileHigh,
    char         * buttonFileDown,
    eolVec3D       backgroundColor,
    eolFloat       backgroundAlpha,
    eolVec3D       highlightColor,
    eolVec3D       pressColor
  )
{
  int i;
  eolComponentButton *button = NULL;
  eolComponent *component = NULL;
  component = eol_component_new();
  if (!component)return NULL;
  eol_component_make_button(
    component,
    buttonText,
    buttonType,
    buttonHotkey,
    buttonHotkeymod,
    buttonFileUp,
    buttonFileHigh,
    buttonFileDown,
    backgroundColor,
    backgroundAlpha,
    highlightColor,
    pressColor,
    fontSize
  );
  eol_rectf_copy(&component->rect,rect);
  button = eol_component_get_button_data(component);
  if (button == NULL)
  {
    eol_component_free(&component);
    return NULL;
  }
  button->centered = center;
  component->id = id;
  strncpy(component->name,name,EOLWORDLEN);
  component->canHasFocus = eolTrue;
  component->type = eolButtonComponent;
  component->data_move = eol_button_move;
  for (i = 0; i < eolButtonStateMax;i++)
  {
    eol_vec3d_copy(button->textColor[i],_eol_component_button_color[i]);
  }
  eol_button_move(component,bounds);
  return component;
}

eolComponent *eol_component_button_load(eolRect winrect,eolKeychain *def)
{
  eolComponent *comp = NULL;
  eolLine buttonType;
  eolUint id;
  eolUint fontSize = 3;
  eolRectFloat rect;
  eolLine justify;
  eolLine buttonText;
  eolLine name;
  eolLine hotkey;
  eolLine hotmod;
  eolInt hotkeybutton = 0;
  eolInt hotkeymod = 0;
  eolLine buttonFile;
  eolLine buttonHighFile;
  eolLine buttonHitFile;
  eolVec3D backgroundColor;
  eolFloat backgroundAlpha;
  eolVec3D highlightColor;
  eolVec3D pressColor;
  eolBool  center = eolFalse;

  if (!def)return NULL;
  
  eol_keychain_get_hash_value_as_line(buttonType, def, "buttonType");
  eol_keychain_get_hash_value_as_line(name, def, "name");
  eol_keychain_get_hash_value_as_uint(&id, def, "id");
  eol_keychain_get_hash_value_as_uint(&fontSize, def, "fontSize");
  eol_keychain_get_hash_value_as_rectfloat(&rect, def, "rect");
  eol_keychain_get_hash_value_as_line(justify, def, "justify");
  eol_keychain_get_hash_value_as_line(buttonText, def, "buttonText");

  if (eol_keychain_get_hash_value_as_line(hotkey, def, "hotkey"))
  {
    hotkeybutton = eol_input_parse("key", hotkey);
  }
  if (eol_keychain_get_hash_value_as_line(hotmod, def, "hotkeymod"))
  {
    hotkeymod = eol_input_parse("mod",hotmod);
  }

  eol_keychain_get_hash_value_as_line(buttonFile, def, "buttonImage");
  eol_keychain_get_hash_value_as_line(buttonHighFile, def, "buttonHigh");
  eol_keychain_get_hash_value_as_line(buttonHitFile, def, "buttonHit");

  eol_keychain_get_hash_value_as_vec3d(&backgroundColor, def, "backgroundColor");
  eol_keychain_get_hash_value_as_float(&backgroundAlpha, def, "backgroundAlpha");
  eol_keychain_get_hash_value_as_vec3d(&highlightColor, def, "highlightColor");
  eol_keychain_get_hash_value_as_vec3d(&pressColor, def, "pressColor");

  if (eol_line_cmp(justify,"CENTER") == 0)center = eolTrue;
  if (eol_line_cmp(buttonType,"STOCK") == 0)
  {
    comp = eol_button_stock_new(
      id,
      name,
      rect,
      winrect,
      buttonText,
      hotkeybutton,
      hotkeymod,
      eolFalse
    );
  }
  else if (eol_line_cmp(buttonType,"TEXT") == 0)
  {
    comp = eol_button_text_new(
      id,
      name,
      rect,
      winrect,
      buttonText,
      fontSize,
      hotkeybutton,
      hotkeymod,
      eolFalse
    );
  }
  else if (eol_line_cmp(buttonType,"RECT") == 0)
  {
    comp = eol_button_new(
      id,
      name,
      rect,
      winrect,
      buttonText,
      fontSize,
      eolButtonRect,
      hotkeybutton,
      hotkeymod,
      center,
      buttonFile,
      buttonHighFile,
      buttonHitFile,
      backgroundColor,
      backgroundAlpha,
      highlightColor,
      pressColor
    );
  }
  else if (eol_line_cmp(buttonType,"CUSTOM") == 0)
  {
    comp = eol_button_new(
      id,
      name,
      rect,
      winrect,
      buttonText,
      fontSize,
      eolButtonCustom,
      hotkeybutton,
      hotkeymod,
      center,
      buttonFile,
      buttonHighFile,
      buttonHitFile,
      backgroundColor,
      backgroundAlpha,
      highlightColor,
      pressColor
    );
  }
  return comp;
}

/*eol@eof*/
