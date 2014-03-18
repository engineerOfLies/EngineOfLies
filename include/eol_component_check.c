#include "eol_component_check.h"
#include "eol_logger.h"
#include "eol_draw.h"
#include "eol_font.h"
#include "eol_mouse.h"
#include "eol_input.h"

/*local function prototypes*/
void eol_component_check_new(eolComponent *component);
void eol_component_check_draw(eolComponent *component);
void eol_component_check_move(eolComponent *component,eolRect newbounds);
eolBool eol_component_check_update(eolComponent *component);
void eol_component_check_free(eolComponent *component);

/*local globals*/
eolSprite * _eol_component_check_button_sprites[eolButtonStateMax] = {NULL,NULL,NULL,NULL};
eolSprite * _eol_component_uncheck_button_sprites[eolButtonStateMax] = {NULL,NULL,NULL,NULL};
eolVec3D    _eol_component_check_color[eolButtonStateMax];
eolLine buttonfile,buttonhitfile,buttonhighfile,buttonsleepfile;
eolLine unbuttonfile,unbuttonhitfile,unbuttonhighfile,unbuttonsleepfile;

/*function definitions*/

void eol_check_configure(eolConfig *conf)
{
  eol_vec3d_set(_eol_component_check_color[0],0.8,0.8,0.8);
  eol_vec3d_set(_eol_component_check_color[1],1,1,0);
  eol_vec3d_set(_eol_component_check_color[2],0.6,0.6,0.6);
  eol_vec3d_set(_eol_component_check_color[3],0.3,0.2,0.3);
  
  eol_line_cpy(buttonfile,"images/UI/checked.png");
  eol_line_cpy(buttonhitfile,"images/UI/checked_hit.png");
  eol_line_cpy(buttonhighfile,"images/UI/checked_high.png");
  eol_line_cpy(buttonsleepfile,"images/UI/checked_sleep.png");
  
  eol_line_cpy(unbuttonfile,"images/UI/unchecked.png");
  eol_line_cpy(unbuttonhitfile,"images/UI/unchecked_hit.png");
  eol_line_cpy(unbuttonhighfile,"images/UI/unchecked_high.png");
  eol_line_cpy(unbuttonsleepfile,"images/UI/unchecked_sleep.png");
  if (conf != NULL)
  {
    eol_config_get_line_by_tag(buttonfile,conf,"check_button_file");
    eol_config_get_line_by_tag(buttonhighfile,conf,"check_button_high_file");
    eol_config_get_line_by_tag(buttonhitfile,conf,"check_button_hit_file");
    eol_config_get_line_by_tag(buttonsleepfile,conf,"check_button_sleep_file");
    
    eol_config_get_line_by_tag(unbuttonfile,conf,"uncheck_button_file");
    eol_config_get_line_by_tag(unbuttonhighfile,conf,"uncheck_button_high_file");
    eol_config_get_line_by_tag(unbuttonhitfile,conf,"uncheck_button_hit_file");
    eol_config_get_line_by_tag(unbuttonsleepfile,conf,"uncheck_button_sleep_file");

    eol_config_get_vec3d_by_tag(&_eol_component_check_color[0],conf,"button_text_color");
    eol_config_get_vec3d_by_tag(&_eol_component_check_color[1],conf,"button_high_text_color");
    eol_config_get_vec3d_by_tag(&_eol_component_check_color[2],conf,"button_hit_text_color");
    eol_config_get_vec3d_by_tag(&_eol_component_check_color[3],conf,"button_sleep_text_color");
  }
  
  _eol_component_check_button_sprites[0] = eol_sprite_load(buttonfile,-1,-1);
  _eol_component_check_button_sprites[1] = eol_sprite_load(buttonhighfile,-1,-1);
  _eol_component_check_button_sprites[2] = eol_sprite_load(buttonhitfile,-1,-1);
  _eol_component_check_button_sprites[3] = eol_sprite_load(buttonsleepfile,-1,-1);
  
  _eol_component_uncheck_button_sprites[0] = eol_sprite_load(unbuttonfile,-1,-1);
  _eol_component_uncheck_button_sprites[1] = eol_sprite_load(unbuttonhighfile,-1,-1);
  _eol_component_uncheck_button_sprites[2] = eol_sprite_load(unbuttonhitfile,-1,-1);
  _eol_component_uncheck_button_sprites[3] = eol_sprite_load(unbuttonsleepfile,-1,-1);
}

eolComponentCheck *eol_component_get_check_data(eolComponent *component)
{
  if ((!component)||
    (!component->componentData)||
    (component->type != eolCheckComponent))
  {
    return NULL;
  }
  return (eolComponentCheck*)component->componentData;
}


void eol_component_make_check(
    eolComponent  * component
  )
{
  eolComponentCheck * check = NULL;
  if (!component)return;
  eol_component_check_new(component);
  check = eol_component_get_check_data(component);
  if (check == NULL)
  {
    return;
  }
  component->data_free = eol_component_check_free;
  component->data_draw = eol_component_check_draw;
  component->data_update = eol_component_check_update;
  component->data_move = eol_component_check_move;
}

eolComponent *eol_check_new(
    eolUint       id,
    eolWord       name,
    eolRectFloat  rect,
    eolRect       bounds,
    eolBool       leftAlign,
    eolBool       packed,
    eolInt        padding,
    eolLine       checkText,
    eolLine       uncheckText,
    eolBool       state,
    eolBool       drawLabel,
    eolComponent *checkButton,
    eolComponent *uncheckButton,
    eolComponent *label
)
{
  eolComponent *component = NULL;
  eolComponentCheck *check = NULL;
  component = eol_component_new();
  if (!component)return NULL;
  eol_rectf_copy(&component->rect,rect);
  eol_rect_copy(&component->canvas,bounds);
  eol_component_get_rect_from_bounds(&component->bounds,bounds, component->rect);

  eol_component_make_check(component);

  check = eol_component_get_check_data(component);
  if (!check)
  {
    eol_component_free(&component);
    return NULL;
  }
  component->id = id;
  eol_line_cpy(component->name,name);
  if (checkButton)
  {
    check->checkButton = checkButton;
  }
  else
  {
    check->checkButton = eol_button_new(
      0,
      "checkButton",
      eol_rectf(0,0,1,1),
      component->bounds,
      "",
      0,
      eolButtonCustom,
      0,
      0,
      0,
      0,
      buttonfile,
      buttonhighfile,
      buttonhitfile,
      eol_vec3d(1,1,1),
      0,
      eol_vec3d(0,0,0),
      eol_vec3d(1,1,1)
    );
  }

  if (uncheckButton)
  {
    check->uncheckButton = uncheckButton;
  }
  else
  {
    check->uncheckButton = eol_button_new(
      1,
      "uncheckButton",
      eol_rectf(0,0,1,1),
      component->bounds,
      "",
      0,
      eolButtonCustom,
      0,
      0,
      0,
      0,
      unbuttonfile,
      unbuttonhighfile,
      unbuttonhitfile,
      eol_vec3d(1,1,1),
      0,
      eol_vec3d(0,0,0),
      eol_vec3d(1,1,1)
    );
  }

  if (label)
  {
    check->label = label;
  }
  else
  {
    check->label = eol_label_new_default(
      3,
      "label",
      eol_rectf(0,0,1,1),
      component->bounds,
      "default checkbox text",
      eolFalse
    );
  }

  eol_line_cpy(check->checkText,checkText);
  eol_line_cpy(check->uncheckText,uncheckText);
  
  eol_component_check_set_state(component,state);
  
  eol_component_move(component,bounds);
  
  return component;
}

void eol_component_check_free(eolComponent *component)
{
  eolComponentCheck * check = eol_component_get_check_data(component);
  if (check == NULL)return;
  eol_component_free(&check->uncheckButton);
  eol_component_free(&check->checkButton);
  eol_component_free(&check->label);
  free(check);
  component->componentData = NULL;
}

eolBool eol_component_check_update(eolComponent *component)
{
  eolComponentCheck *check = eol_component_get_check_data(component);
  if (check == NULL)return eolFalse;
  switch(check->state)
  {
    case eolCheckChecked:
      if ((eol_component_update(check->checkButton)) ||
        (eol_mouse_in_rect(component->bounds) && eol_mouse_input_released(eolMouseLeft)))
      {
        eol_component_check_set_state(component,eolCheckUnchecked);
        /*move in place*/
        eol_component_check_move(component,component->canvas);
        return eolTrue;
      }
      break;
    case eolCheckUnchecked:
      if ((eol_component_update(check->uncheckButton)) ||
        (eol_mouse_in_rect(component->bounds) && eol_mouse_input_released(eolMouseLeft)))
      {
        eol_component_check_set_state(component,eolCheckChecked);
        /*move in place*/
        eol_component_check_move(component,component->canvas);
        return eolTrue;
      }
      break;
  }
  return eolFalse;
}

void eol_component_check_move(eolComponent *component,eolRect newbounds)
{
  eolRect lr,br;
  eolUint buttonW = 0,buttonH = 0;
  eolUint labelW = 0,labelH = 0;
  eolComponentCheck *check = eol_component_get_check_data(component);
  
  if (check == NULL)return;
  eol_component_get_rect_from_bounds(&component->bounds,newbounds,component->rect);
  
  /*TODO: setup the correct bounds for the components*/
  eol_button_get_size(check->checkButton,&buttonW, &buttonH);
  eol_label_get_text_size(check->label,&labelW,&labelH);
  component->bounds.w = buttonW + check->padding + labelW;
  component->bounds.h = MAX(buttonH,labelH);

  eol_rect_copy(&lr,component->bounds);
  eol_rect_copy(&br,component->bounds);
  
  /*center the shorter by the larger*/
  if (buttonH > labelH)
  {
    lr.y = component->bounds.y + (buttonH - labelH)/2;
  }
  else
  {
    br.y = component->bounds.y + (labelH - buttonH)/2;
  }

  /*TODO handle padding and order*/
  lr.x += buttonW + check->padding;
  
  eol_component_move(check->checkButton,br);
  eol_component_move(check->uncheckButton,br);
  eol_component_move(check->label,lr);
}

void eol_component_check_draw(eolComponent *component)
{
  eolComponentCheck *check = eol_component_get_check_data(component);
  if (check == NULL)return;
  
  switch(check->state)
  {
    case eolCheckChecked:
      eol_component_draw(check->checkButton);
      break;
    case eolCheckUnchecked:
      eol_component_draw(check->uncheckButton);
      break;
    default:
      eol_logger_message(EOL_LOG_ERROR,"eol_component_check_draw: checkbox in bad state!");
      break;
  }
  eol_component_draw(check->label);
}

void eol_component_check_new(eolComponent *component)
{
  if (component->componentData != NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_component_check:tried to make a percent check out of an existing component\n");
    return;
  }
  component->componentData = malloc(sizeof(eolComponentCheck));
  if (component->componentData == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_component_check: failed to allocate data for new percent check\n");
    return;
  }
  memset(component->componentData,0,sizeof(eolComponentCheck));
  component->type = eolCheckComponent;
}

void eol_component_check_assign_text(eolComponent *component,eolLine onChecked,eolLine onUnchecked)
{
  eolComponentCheck *check = eol_component_get_check_data(component);
  if (check == NULL)return;
  eol_line_cpy(check->checkText,onChecked);
  eol_line_cpy(check->uncheckText,onUnchecked);
}

eolInt eol_component_check_get_state(eolComponent *component)
{
  eolComponentCheck *check = eol_component_get_check_data(component);
  if (check == NULL)return -1;
  return check->state;
}

void eol_component_check_set_state(eolComponent *component,eolInt state)
{
  eolComponentCheck *check = eol_component_get_check_data(component);
  if (check == NULL)return;
  if (state == eolCheckChecked)
  {
    eol_label_set_text(check->label,check->checkText);
  }
  else if (state == eolCheckUnchecked)
  {
    eol_label_set_text(check->label,check->uncheckText);
  }
  else
  {
    eol_logger_message(EOL_LOG_WARN,"eol_component_check_set_state: state provided is bad");
    return;
  }
  check->state = state;  
}

eolInt eol_check_get_state_by_line(eolLine stateText)
{
  if ((eol_line_cmp(stateText,"checked") == 0)
    || (eol_line_cmp(stateText,"Checked") == 0)
    || (eol_line_cmp(stateText,"CHECKED") == 0))
  {
    return eolCheckChecked;
  }
  if ((eol_line_cmp(stateText,"unchecked") == 0)
    || (eol_line_cmp(stateText,"Unchecked") == 0)
    || (eol_line_cmp(stateText,"UNCHECKED") == 0))
  {
    return eolCheckUnchecked;
  }
  return -1;
}
eolComponent *eol_check_create_from_config(eolKeychain *def,eolRect parentRect)
{
  eolComponent *checkbox = NULL;
  eolUint       id;
  eolLine       name;
  eolRectFloat  rect;
  eolKeychain * key;
  eolBool       leftAlign = eolTrue;
  eolBool       packed = eolTrue;
  eolInt        padding = 2;
  eolBool       drawLabel = eolTrue;
  eolLine       checkText = "checked";
  eolLine       uncheckText = "unchecked";
  eolLine       stateText = "";
  eolLine       hotkey = "";
  eolLine       hotmod = "";
  eolInt        hotkeybutton = 0;
  eolInt        hotkeymodbutton = 0;
  eolInt        state = eolCheckUnchecked;
  eolComponent *checkButton= NULL;
  eolComponent *uncheckButton= NULL;
  eolComponent *label = NULL;
  eolComponentCheck * check;
  
  if (!def)
  {
    eol_logger_message(EOL_LOG_WARN,"eol_check_create_from_config: passed bad def parameter");
    return NULL;
  }

  eol_keychain_get_hash_value_as_line(name, def, "name");
  eol_keychain_get_hash_value_as_uint(&id, def, "id");
  eol_keychain_get_hash_value_as_rectfloat(&rect, def, "rect");
  eol_keychain_get_hash_value_as_bool(&drawLabel,def,"drawLabel");
  eol_keychain_get_hash_value_as_line(checkText, def, "checkText");
  eol_keychain_get_hash_value_as_line(uncheckText, def, "uncheckText");
  if (eol_keychain_get_hash_value_as_line(stateText, def, "state"))
  {
    state = eol_check_get_state_by_line(stateText);
  }
  if (eol_keychain_get_hash_value_as_line(hotkey, def, "hotkey"))
  {
    hotkeybutton = eol_input_parse("key", hotkey);
  }
  if (eol_keychain_get_hash_value_as_line(hotmod, def, "hotkeymod"))
  {
    hotkeymodbutton = eol_input_parse("mod",hotmod);
  }
  
  key = eol_keychain_get_hash_value(def,"checkButton");
  if (key)
  {
    checkButton = eol_component_button_load(parentRect,key);
  }

  key = eol_keychain_get_hash_value(def,"uncheckButton");
  if (key)
  {
    uncheckButton = eol_component_button_load(parentRect,key);
  }

  key = eol_keychain_get_hash_value(def,"label");
  if (key)
  {
    label = eol_component_create_label_from_config(key,parentRect);
  }

  
  checkbox = eol_check_new(
    id,
    name,
    rect,
    parentRect,
    leftAlign,
    packed,
    padding,
    checkText,
    uncheckText,
    state,
    drawLabel,
    checkButton,
    uncheckButton,
    label);

  check = eol_component_get_check_data(checkbox);
  if (!check)
  {
    eol_logger_message(EOL_LOG_WARN,"eol_check_create_from_config: failed to configure a checkbox!");
    eol_component_free(&checkButton);
    eol_component_free(&uncheckButton);
    eol_component_free(&label);
    return NULL;
  }

  check->hotkey = hotkeybutton;
  check->hotkeymod = hotkeymodbutton;
  
  return checkbox;
}
/*eol@eof*/
