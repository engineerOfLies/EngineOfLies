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
void eol_component_button_set_activation(eolComponent* comp,
                                         eolBool release,
                                         eolBool press,
                                         eolBool hold);

/*local global variables*/
eolSprite * _eol_component_stock_button[eolButtonStateMax] = {NULL,NULL,NULL,NULL};
eolVec3D    _eol_component_button_color[eolButtonStateMax];
eolInt      _eol_component_button_offset_x = 0;
eolInt      _eol_component_button_offset_y = 0;
eolBool     _eol_component_button_text_outline = eolTrue;
eolFloat    _eol_component_button_alpha = 1;
eolBool     _eol_component_button_press_active = eolFalse;
eolBool     _eol_component_button_hold_active = eolFalse;
eolBool     _eol_component_button_release_active = eolTrue;
eolInt      _eol_component_button_font_size = 3;
/*function definitions*/

void eol_button_configure(eolConfig *conf)
{
  eolLine buf;
  eolLine buttonfile,buttonhitfile,buttonhighfile,buttonsleepfile;
  _eol_component_button_offset_x = 0;
  _eol_component_button_offset_y = 0;
  _eol_component_button_press_active = eolFalse;
  _eol_component_button_hold_active = eolFalse;
  _eol_component_button_release_active = eolTrue;
  _eol_component_button_font_size = 3;
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
    eol_config_get_bool_by_tag(&_eol_component_button_press_active,conf,"active_on_press");
    eol_config_get_bool_by_tag(&_eol_component_button_hold_active,conf,"active_on_hold");
    eol_config_get_bool_by_tag(&_eol_component_button_release_active,conf,"active_on_release");

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
    eol_config_get_int_by_tag(&_eol_component_button_font_size,conf,"button_font_size");
  }
  _eol_component_stock_button[0] = eol_sprite_load(buttonfile,-1,-1);
  _eol_component_stock_button[1] = eol_sprite_load(buttonhighfile,-1,-1);
  _eol_component_stock_button[2] = eol_sprite_load(buttonhitfile,-1,-1);
  _eol_component_stock_button[3] = eol_sprite_load(buttonsleepfile,-1,-1);
}

eolUint eol_button_get_state(eolComponent *button)
{
  if (!eol_component_get_button_data(button))return 0;
  return button->state;
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

void eol_button_get_text(eolLine outText,eolComponent *button)
{
  eolComponentButton *buttonData = NULL;
  buttonData = eol_component_get_button_data(button);
  if (!buttonData)return;
  eol_line_cpy(outText,buttonData->buttonText);
}

void eol_button_get_size(eolComponent *button,eolUint *w, eolUint *h)
{
  eolRect fontRect = {0,0,0,0};
  eolUint imageW = 0, imageH = 0;
  eolComponentButton *buttonData = NULL;
  buttonData = eol_component_get_button_data(button);
  if (!buttonData)return;
  if (buttonData->button[0] != NULL)
  {
    imageW = buttonData->button[0]->frameWidth;
    imageH = buttonData->button[0]->frameHeight;
  }
  if (strlen(buttonData->buttonText)> 0)
  {
    fontRect = eol_font_get_bounds(
      buttonData->buttonText,
      buttonData->fontSize
    );    
  }
  if (w)
  {
    *w = MAX(imageW,fontRect.w);
  }
  if (h)
  {
    *h = MAX(imageH,fontRect.h);
  }
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
      return button->activeOnPress;
    }
    if (eol_input_is_key_held(button->input))
    {
      component->state = eolButtonPressed;
      return button->activeOnHold;
    }
    if (eol_input_is_key_released(button->input))
    {
      component->state = eolButtonIdle;
      return button->activeOnRelease;
    }
  }
  if (eol_vec_in_rect(v,component->bounds))
  {
    component->state = eolButtonHighlight;
    if (eol_mouse_input_held(eolMouseLeft))
    {
      component->state = eolButtonPressed;
      return button->activeOnHold;
    }
    if (eol_mouse_input_pressed(eolMouseLeft))
    {
      component->state = eolButtonPressed;
      return button->activeOnPress;
    }
    else if (eol_mouse_input_released(eolMouseLeft))
    {
      component->state = eolButtonIdle;
      return button->activeOnRelease;
    }
    return eolFalse;
  }
  component->state = eolButtonIdle;
  return eolFalse;
}

void eol_component_button_draw_rect(eolComponent *component)
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

void eol_component_button_draw(eolComponent *component)
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
    eolUint        justify,
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
  button->justify = justify;
  switch(buttonType)
  {
    case eolButtonCustom:
      button->button[eolButtonIdle] = eol_sprite_load(buttonUpFile,-1,-1);
      button->button[eolButtonHighlight] = eol_sprite_load(buttonHighFile,-1,-1);
      button->button[eolButtonPressed] = eol_sprite_load(buttonDownFile,-1,-1);
      if (button->justify == eolJustifyNone)
      {
        button->justify = eolJustifyCenter;
      }
      break;
    case eolButtonText:
      button->button[eolButtonIdle] = NULL;
      button->button[eolButtonHighlight] = NULL;
      button->button[eolButtonPressed] = NULL;
      if (button->justify == eolJustifyNone)
      {
        button->justify = eolJustifyLeft;
      }
      break;
    case eolButtonStock:
      button->button[eolButtonIdle] = _eol_component_stock_button[eolButtonIdle];
      button->button[eolButtonHighlight] = _eol_component_stock_button[eolButtonHighlight];
      button->button[eolButtonPressed] = _eol_component_stock_button[eolButtonPressed];
      if (button->justify == eolJustifyNone)
      {
        button->justify = eolJustifyCenter;
      }
      break;
    case eolButtonRect:
      component->data_draw = eol_component_button_draw_rect;
      eol_vec3d_copy(button->backgroundColor,backgroundColor);
      button->backgroundAlpha = backgroundAlpha;
      if (button->justify == eolJustifyNone)
      {
        button->justify = eolJustifyLeft;
      }
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
    eolInt         justify,
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
    justify,
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
    eolInt         justify,
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
    justify,
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
    component->bounds.y -= component->bounds.h/2;
  }
  if (button->justify == eolJustifyCenter)
  {
    component->bounds.x -= component->bounds.w/2;
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
    eolInt         justify,
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
    justify,
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
  component->type = eolButtonComponent;
  component->data_move = eol_button_move;
  for (i = 0; i < eolButtonStateMax;i++)
  {
    eol_vec3d_copy(button->textColor[i],_eol_component_button_color[i]);
  }
  eol_button_move(component,bounds);
  eol_component_button_set_activation(component,
                                      _eol_component_button_release_active,
                                      _eol_component_button_press_active,
                                      _eol_component_button_hold_active);
  return component;
}

void eol_component_button_set_activation(eolComponent* comp,
                                         eolBool release,
                                         eolBool press,
                                         eolBool hold)
{
  eolComponentButton *button = NULL;
  button = eol_component_get_button_data(comp);
  if (button == NULL)return;
  button->activeOnRelease = release;
  button->activeOnPress = press;
  button->activeOnHold = hold;
}

eolComponent *eol_component_button_load(eolRect winrect,eolKeychain *def)
{
  eolComponent *comp = NULL;
  eolLine buttonType;
  eolUint id;
  eolUint fontSize = _eol_component_button_font_size;
  eolRectFloat rect;
  eolLine justify = "NONE";
  eolLine buttonText;
  eolLine name;
  eolLine hotkey;
  eolLine hotmod;
  eolInt hotkeybutton = 0;
  eolInt hotkeymod = 0;
  eolLine buttonFile;
  eolLine buttonHighFile;
  eolLine buttonHitFile;
  eolBool active_on_hold;
  eolBool active_on_press;
  eolBool active_on_release;
  eolVec3D backgroundColor;
  eolFloat backgroundAlpha;
  eolVec3D highlightColor;
  eolVec3D pressColor;
  eolBool  center = eolFalse;

  if (!def)return NULL;
  active_on_hold = _eol_component_button_hold_active;
  active_on_press = _eol_component_button_press_active;
  active_on_release = _eol_component_button_release_active;

  eol_keychain_get_hash_value_as_bool(&active_on_release, def, "active_on_release");
  eol_keychain_get_hash_value_as_bool(&active_on_press, def, "active_on_press");
  eol_keychain_get_hash_value_as_bool(&active_on_hold, def, "active_on_hold");
  
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
      eol_font_justify_from_string(justify),
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
      eol_font_justify_from_string(justify),
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
      eol_font_justify_from_string(justify),
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
      eol_font_justify_from_string(justify),
      buttonFile,
      buttonHighFile,
      buttonHitFile,
      backgroundColor,
      backgroundAlpha,
      highlightColor,
      pressColor
    );
  }
  eol_component_button_set_activation(comp,active_on_release,active_on_press,active_on_hold);
  return comp;
}

/*eol@eof*/
