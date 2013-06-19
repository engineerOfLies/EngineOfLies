#include "eol_component_entry.h"
#include "eol_component.h"
#include "eol_logger.h"
#include "eol_input.h"
#include "eol_draw.h"


/*local types*/
/*local global variables*/
/*local function prototypes*/
void eol_component_entry_new(eolComponent *component);
void eol_component_entry_free(eolComponent *component);
void eol_component_entry_draw(eolComponent *component, eolRect bounds);
eolBool eol_component_entry_update(eolComponent *component);

/*definitions*/

eolComponentEntry *eol_component_textinput_get_component(eolComponent *component);

eolComponentEntry *eol_component_get_entry_data(eolComponent *component)
{
  if ((!component)||
    (!component->componentData)||
    (component->type != eolEntryComponent))
  {
    return NULL;
  }
  return (eolComponentEntry*)component->componentData;
}

void eol_entry_get_line(eolComponent *component,eolLine output)
{
  eolComponentEntry * entry = NULL;
  if (!component)return;
  entry = eol_component_get_entry_data(component);
  if (entry == NULL)return;
  eol_line_cpy(output,entry->buffer->str);
}

eolComponent *eol_line_entry_new(
  eolUint       id,
  eolLine       name,
  eolRectFloat  rect,
  eolRect       bounds,
  eolLine       output
)
{
  return eol_entry_new(
    id,
    name,
    rect,
    bounds,
    output,
    EOLLINELEN,
    eolJustifyLeft,
    eolFalse,
    3,
    "",
    eolFalse,
    eol_vec3d(0,1,0),
                       1,
                       eol_vec3d(0.1,0.1,0.1)
  );
}

void eol_component_entry_new(eolComponent *component)
{
  if (component->componentData != NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_component:tried to make a label out of an existing component\n");
    return;
  }
  component->componentData = malloc(sizeof(eolComponentEntry));
  if (component->componentData == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_actor: failed to allocate data for new label\n");
    return;
  }
  memset(component->componentData,0,sizeof(eolComponentEntry));
  component->type = eolEntryComponent;
}

void eol_component_make_entry(
  eolComponent * component,
  eolLine        startingText,
  eolUint        outputLimit,
  eolUint        justify,
  eolUint        fontSize,
  eolBool        wordWrap,
  eolLine        fontName,
  eolVec3D       color,
  eolFloat       alpha,
  eolVec3D       bgcolor
)
{
  eolComponentEntry * entry = NULL;
  if (!component)return;
  eol_component_entry_new(component);
  entry = eol_component_get_entry_data(component);
  if (entry == NULL)
  {
    return;
  }
  
  entry->buffer = g_string_new(startingText);
  if (entry->buffer == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_component:Failed to duplicate string for new entry\n");
    eol_component_entry_free(component);
    return;
  }
  
  entry->bufferLimit = outputLimit;
  entry->justify = justify;
  entry->fontSize = fontSize;
  entry->alpha = alpha;
  entry->wordWrap = wordWrap;
  eol_vec3d_copy(entry->color,color);
  component->data_free = eol_component_entry_free;
  component->data_draw = eol_component_entry_draw;
  component->data_update = eol_component_entry_update;
}


void eol_entry_delete_char(eolComponent *component)
{
  eolComponentEntry *entry;
  if (!component)return;
  entry = eol_component_get_entry_data(component);
  if (entry == NULL)return;
  if (entry->buffer->len <= 0)return;
  entry->buffer = g_string_erase(entry->buffer,(entry->buffer->len - 1),1);
  if (entry->buffer == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_component:failed to delete a character in entry\n");
  }
}

void eol_entry_append_char(eolComponent *component,
                           char          newchar)
{
  eolComponentEntry *entry;
  entry = eol_component_get_entry_data(component);
  if (entry == NULL)return;
  if (entry->buffer->len >= entry->bufferLimit)return;
  entry->buffer = g_string_append_c(entry->buffer,newchar);
}

eolBool eol_component_entry_update(eolComponent *component)
{
  eolUI8 key;
  char let;
  eolComponentEntry *entry;
  
  if (!component)return eolFalse;
  if (!component->hasFocus)return eolFalse;
  entry = eol_component_get_entry_data(component);
  if (entry == NULL)return eolFalse;
  if (eol_input_is_key_released(SDLK_BACKSPACE))
  {
    eol_entry_delete_char(component);
    return eolFalse;
  }
  if (eol_input_is_key_released(SDLK_PERIOD))
  {
    eol_entry_append_char(component,'.');
    return eolFalse;
  }
  if (eol_input_is_key_released(SDLK_MINUS))
  {
    eol_entry_append_char(component,'-');
    return eolFalse;
  }
  if (eol_input_is_key_released(SDLK_SLASH))
  {
    eol_entry_append_char(component,'/');
    return eolFalse;
  }
  for (key = SDLK_0;key <= SDLK_9;key++)
  {
    if (eol_input_is_key_released(key))
    {
      let = key - SDLK_0 + '0';
      eol_entry_append_char(component,let);
      return eolFalse;
    }
  }
  if (entry->number)return eolFalse;
  if (eol_input_is_key_released(SDLK_SPACE))
  {
    eol_entry_append_char(component,' ');
    return eolFalse;
  }
  if (eol_input_is_key_released(SDLK_COMMA))
  {
    eol_entry_append_char(component,',');
    return eolFalse;
  }
  if (eol_input_is_key_released(SDLK_COLON))
  {
    eol_entry_append_char(component,':');
    return eolFalse;
  }
  for (key = SDLK_a;key <= SDLK_z;key++)
  {
    if (eol_input_is_key_released(key))
    {
      if (eol_input_is_mod_down(KMOD_SHIFT))
      {
        let = key - SDLK_a + 'A';
      }
      else
      {
        let = key - SDLK_a + 'a';
      }
      eol_entry_append_char(component,let);
      return eolFalse;
    }
  }
  return eolFalse;
}

void eol_entry_assign_callback(eolComponent *component,void *data, void (*callback)(void *data, eolLine output))
{
  eolComponentEntry * entry = NULL;
  entry = eol_component_get_entry_data(component);
  if (entry == NULL)
  {
    eol_logger_message(EOL_LOG_WARN,"eol_component:unable to assign entry output\n");
    return;
  }
  entry->entry_callback = callback;
  entry->callback_data = data;
}

void eol_entry_assign_output(eolComponent *component)
{
  eolComponentEntry * entry = NULL;
  entry = eol_component_get_entry_data(component);
  if (entry == NULL)
  {
    eol_logger_message(EOL_LOG_WARN,"eol_component:unable to assign entry output\n");
    return;
  }
  if (entry->entry_callback == NULL)
  {
    eol_logger_message(EOL_LOG_INFO,"eol_component:unable to assign output.  No callback specified\n");
    return;
  }
  entry->entry_callback(entry->callback_data,entry->buffer->str);
}

eolComponent *eol_entry_create_from_config(eolKeychain *def, eolRect parentRect)
{
  eolUint       id;
  eolLine       text;
  eolLine       name;
  eolLine       justify = "";
  eolBool       wordWrap = eolFalse;
  eolBool       limitNumbers = eolFalse;
  eolUint       fontSize;
  eolVec3D      color = {1,1,1};
  eolVec3D      bgcolor = {0,0,0};
  eolFloat      alpha = 1;
  eolLine       fontName = "";
  eolRectFloat  rect;
  
  if (!def)
  {
    eol_logger_message(EOL_LOG_WARN,"eol_component: Passed bad def parameter!");
    return NULL;
  }
  
  eol_keychain_get_hash_value_as_uint(&id, def, "id");
  eol_keychain_get_hash_value_as_line(name, def, "name");
  eol_keychain_get_hash_value_as_uint(&fontSize, def, "fontSize");
  eol_keychain_get_hash_value_as_rectfloat(&rect, def, "rect");
  eol_keychain_get_hash_value_as_line(text, def, "text");
  eol_keychain_get_hash_value_as_line(justify, def, "justify");
  eol_keychain_get_hash_value_as_line(fontName, def, "fontName");
  eol_keychain_get_hash_value_as_bool(&wordWrap, def, "wordWrap");
  eol_keychain_get_hash_value_as_vec3d(&color, def, "color");
  eol_keychain_get_hash_value_as_vec3d(&bgcolor, def, "bgcolor");
  eol_keychain_get_hash_value_as_float(&alpha, def, "alpha");
  eol_keychain_get_hash_value_as_bool(&limitNumbers, def, "limitNumbers");
  
  return eol_entry_new(
    id,
    name,
    rect,
    parentRect,
    NULL,
    EOLLINELEN,
    eol_font_justify_from_string(justify),
                       wordWrap,
                       fontSize,
                       fontName,
                       limitNumbers,
                       color,
                       alpha,
                       bgcolor
  );
}

eolComponent *eol_entry_new(
  eolUint       id,
  eolLine       name,
  eolRectFloat  rect,
  eolRect       bounds,
  char        * output,
  eolInt        outputLimit,
  eolUint       justify,
  eolBool       wordWrap,
  eolUint       fontSize,
  eolLine       fontName,
  eolBool       number,
  eolVec3D      color,
  eolFloat      alpha,
  eolVec3D      bgcolor
)
{
  eolComponent *component = NULL;
  component = eol_component_new();
  if (!component)return NULL;
  eol_component_make_entry(
    component,
    output,
    outputLimit,
    justify,
    fontSize,
    wordWrap,
    fontName,
    color,
    alpha,
    bgcolor
  );
  if (component->componentData == NULL)
  {
    eol_component_free(&component);
    return NULL;
  }
  
  component->id = id;
  strncpy(component->name,name,EOLWORDLEN);
  eol_rectf_copy(&component->rect,rect);
  component->type = eolEntryComponent;
  eol_component_get_rect_from_bounds(&component->bounds,bounds, rect);
  return component;
}

void eol_component_entry_draw(eolComponent *component, eolRect bounds)
{
  eolRect r;
  eolText buffer;
  eolComponentEntry *entry = eol_component_get_entry_data(component);
  if (entry == NULL)return;
  r.x = component->bounds.x - 1;
  r.y = component->bounds.y - 1;
  r.w = component->bounds.w + 2;
  r.h = component->bounds.h + 2;
  eol_draw_solid_rect(r,eol_vec3d(1,1,1),1);
  eol_draw_solid_rect(component->bounds,entry->bgcolor,1);
  eol_text_cpy(buffer,entry->buffer->str);
  if (component->hasFocus)
  {
    if (entry->cursorBlinkDirection <= 0)
    {
      entry->cursorBlink -= 0.1;
      if (entry->cursorBlink < -1)
      {
        entry->cursorBlinkDirection = 1;
      }
    }
    else
    {
      snprintf(buffer,EOLTEXTLEN,"%s|",entry->buffer->str);
      entry->cursorBlink += 0.1;
      if (entry->cursorBlink > 1)
      {
        entry->cursorBlinkDirection = -1;
      }
    }
  }
  if (strlen(buffer)<= 0)return;
  if (entry->font == NULL)
  {
    if (entry->wordWrap)
    {
      eol_font_draw_text_block(
        buffer,
        component->bounds.x,
        component->bounds.y,
        component->bounds.w,
        0,
        entry->color,
        entry->alpha,
        entry->fontSize
      );
    }
    else
    {
      eol_font_draw_text_justify(
        buffer,
        component->bounds.x,
        component->bounds.y,
        entry->color,
        entry->alpha,
        entry->fontSize,
        entry->justify
      );
    }
  }
  else
  {
    if (entry->wordWrap)
    {
      eol_font_draw_text_block_custom(
        buffer,
        component->bounds,
        entry->color,
        entry->alpha,
        entry->font
      );
    }
    else
    {
      eol_font_draw_text_justify_custom(
        buffer,
        component->bounds.x,
        component->bounds.y,
        entry->color,
        entry->alpha,
        entry->font,
        entry->justify
      );
    }
  }
}


void eol_component_entry_free(eolComponent *component)
{
  eolComponentEntry *input = eol_component_get_entry_data(component);
  if (input == NULL)return;
  if (input->buffer != NULL)free(input->buffer);
  if (input->font != NULL)eol_font_free(&input->font);
  free(input);
  input = NULL;
  component->componentData = NULL;
}

/*EOL@EOF*/
