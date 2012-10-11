#include "eol_component_button.h"
#include "eol_component_list.h"
#include "eol_component_percent_bar.h"
#include "eol_component.h"
#include "eol_logger.h"
#include "eol_font.h"
#include "eol_sprite.h"
#include "eol_actor.h"
#include "eol_mouse.h"
#include "eol_draw.h"
#include "eol_input.h"
#include "eol_config.h"
#include <glib/glist.h>
#include <glib/gstring.h>

/*local types*/
typedef struct
{
  GString  * buffer;
  eolUint    justify;
  eolBool    wordWrap;
  eolUint    fontSize;
  eolVec3D   color;
  eolFloat   alpha;
  eolFont  * font;    /**<if defined, it will use the custom font to draw text*/
}eolComponentLabel;

typedef struct
{
  char    * output;      /**<only on confirmation will the output be changed.*/
  GString * buffer;
  eolInt    bufferLimit; /**<if -1 no limit, otherwise its the maximum character
  that will be added to buffer*/
  eolUint   justify;
  eolBool   wordWrap;
  eolUint   fontSize;    /**<size of the font to use when displaying the text*/
  eolFont * font;    /**<if defined, it will use the custom font to draw text*/
  eolBool   number; /**<if true, limits input to 0-9 - and .*/
  eolVec3D  color;
  eolVec3D  bgcolor;
  eolFloat  alpha;
}eolComponentEntry;

typedef struct
{
  eolActor * actor;
  eolVec3D   position;
  eolVec3D   rotation;
  eolVec3D   scale;
  eolVec3D   color;
  eolFloat   alpha;
}eolComponentActor;

typedef struct
{
  eolSprite *image;
  eolUint    frame;
  eolBool    scaleToBounds;
}eolComponentImage;

typedef struct
{
  eolUint    sliderType;
  eolSprite *slider;
  eolSprite *sliderHigh;
  eolSprite *bar;
  eolSprite *cap1;
  eolSprite *cap2;
  eolVec3D   barColor;
  eolVec3D   sliderColor;
  eolBool    vertical;
  eolFloat   position;
  eolFloat   oldPosition;
  eolUint    sliderState;
}eolComponentSlider;

/*local global variables*/
eolSprite * _eol_component_slider[4] = {NULL,NULL,NULL,NULL};
eolLine     _eol_component_slider_files[4];

/*local function prototypes*/
void eol_component_label_new(eolComponent *component);
void eol_component_label_free(eolComponent *component);
void eol_component_entry_new(eolComponent *component);
void eol_component_slider_new(eolComponent *component);

eolBool eol_component_has_changed(eolComponent *component);
eolInt eol_component_get_state(eolComponent *component);

void eol_component_textinput_new(eolComponent *component);
void eol_component_textinput_get_text(eolComponent *component,char *text);
eolComponentEntry *eol_component_textinput_get_component(eolComponent *component);

void eol_component_actor_new(eolComponent *component);
void eol_component_actor_load(eolComponent *component,char * filename);

void eol_component_get_rect_from_bounds(eolRect *rect,eolRect canvas, eolRectFloat bounds);

void eol_component_label_free(eolComponent *component);
void eol_component_entry_free(eolComponent *component);
void eol_component_actor_free(eolComponent *component);
void eol_component_slider_free(eolComponent *component);
void eol_component_image_free(eolComponent *component);

void eol_component_make_label(
    eolComponent * component,
    char         * text,
    eolUint        justify,
    eolInt         fontSize,
    eolBool        wordWrap,
    char         * fontName,
    eolVec3D       color,
    eolFloat       alpha
  );

/*definitions*/
void eol_component_config()
{
  eolLine buf;
  eolLine sliderVfile,sliderVhighfile,sliderHfile,sliderHhighfile;
  eolConfig *conf = NULL;
  
  eol_line_cpy(sliderVfile,"images/UI/slider_v.png");
  eol_line_cpy(sliderVhighfile,"images/UI/slider_v_high.png");
  eol_line_cpy(sliderHfile,"images/UI/slider.png");
  eol_line_cpy(sliderHhighfile,"images/UI/slider_high.png");
  
  conf = eol_config_load("system/component.cfg");
  if (conf != NULL)
  {
    eol_button_configure(conf);
    eol_config_get_line_by_tag(buf,conf,"slider_verticle");
    if (strlen(buf) > 0)
    {
      eol_line_cpy(sliderVfile,buf);
    }
    eol_config_get_line_by_tag(buf,conf,"slider_verticle_high");
    if (strlen(buf) > 0)
    {
      eol_line_cpy(sliderVhighfile,buf);
    }
    eol_config_get_line_by_tag(buf,conf,"slider_horizontal");
    if (strlen(buf) > 0)
    {
      eol_line_cpy(sliderHfile,buf);
    }
    eol_config_get_line_by_tag(buf,conf,"slider_horizontal_high");
    if (strlen(buf) > 0)
    {
      eol_line_cpy(sliderHhighfile,buf);
    }
    
    eol_config_free(&conf);
  }

  _eol_component_slider[0] = eol_sprite_load(sliderVfile,-1,-1);
  _eol_component_slider[1] = eol_sprite_load(sliderVhighfile,-1,-1);
  _eol_component_slider[2] = eol_sprite_load(sliderHfile,-1,-1);
  _eol_component_slider[3] = eol_sprite_load(sliderHhighfile,-1,-1);
  eol_line_cpy(_eol_component_slider_files[0],sliderVfile);
  eol_line_cpy(_eol_component_slider_files[1],sliderVhighfile);
  eol_line_cpy(_eol_component_slider_files[2],sliderHfile);
  eol_line_cpy(_eol_component_slider_files[3],sliderHhighfile);
  
}

eolComponent * eol_component_new()
{
  eolComponent *component = NULL;
  component = (eolComponent *)malloc(sizeof(eolComponent));
  if (component == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_component: failed to allocate new component!\n");
    return NULL;
  }
  memset(component,0,sizeof(eolComponent));
  return component;
}

void eol_component_free(eolComponent **component)
{
  if ((!component)||(!*component))return;
  if ((*component)->data_free != NULL)
  {
    (*component)->data_free(*component);
  }
  free(*component);
  *component = NULL;
}

void eol_component_move(eolComponent *component,eolRect newbounds)
{
  if (!component)return;
  if (component->data_move != NULL)
  {
    component->data_move(component,newbounds);
  }
  else
  {
    eol_component_get_rect_from_bounds(&component->bounds,newbounds,component->rect);
  }
}

eolBool eol_component_update(eolComponent *component)
{
  if (!component)return eolFalse;
  if (component->data_update != NULL)
  {
    return component->data_update(component);
  }
  return eolFalse;
}

eolBool eol_component_changed(eolComponent *component)
{
  if (!component)return eolFalse;
  if (component->data_changed == NULL)return eolFalse;
  return component->data_changed(component);
}

void eol_component_set_focus(eolComponent *component,eolBool focus)
{
  if (!component)return;
  if (!component->canHasFocus)return;
  component->hasFocus = focus;
}

eolComponentLabel *eol_component_get_label_data(eolComponent *component)
{
  if ((!component)||
    (!component->componentData)||
    (component->type != eolLabelComponent))
  {
    return NULL;
  }
  return (eolComponentLabel*)component->componentData;
}

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

eolComponentImage *eol_component_get_image_data(eolComponent *component)
{
  if ((!component)||
    (!component->componentData)||
    (component->type != eolImageComponent))
  {
    return NULL;
  }
  return (eolComponentImage*)component->componentData;
}

eolComponentActor *eol_component_get_actor_data(eolComponent *component)
{
  if ((!component)||
    (!component->componentData)||
    (component->type != eolActorComponent))
  {
    return NULL;
  }
  return (eolComponentActor*)component->componentData;
}

eolComponentSlider *eol_component_get_slider_data(eolComponent *component)
{
  if ((!component)||
    (!component->componentData)||
    (component->type != eolSliderComponent))
  {
    return NULL;
  }
  return (eolComponentSlider*)component->componentData;
}

/*
 **** Free ****
*/

void eol_component_label_free(eolComponent *component)
{
  eolComponentLabel *label = eol_component_get_label_data(component);
  if (label == NULL)return;
  if (label->buffer != NULL)free(label->buffer);
  if (label->font != NULL)eol_font_free(&label->font);
  free(label);
  label = NULL;
  component->componentData = NULL;
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

void eol_component_actor_free(eolComponent *component)
{
  eolComponentActor *actor = eol_component_get_actor_data(component);
  if (actor == NULL)return;
  eol_actor_free(&actor->actor);
  free(actor);
  component->componentData = NULL;
}

void eol_component_slider_free(eolComponent *component)
{
  eolComponentSlider *slider = eol_component_get_slider_data(component);
  if (slider == NULL)return;
  eol_sprite_free(&slider->slider);
  eol_sprite_free(&slider->sliderHigh);
  eol_sprite_free(&slider->bar);
  eol_sprite_free(&slider->cap1);
  eol_sprite_free(&slider->cap2);
  free(slider);
  component->componentData = NULL;
}

void eol_component_image_free(eolComponent *component)
{
  eolComponentImage * image = eol_component_get_image_data(component);
  if (image == NULL)return;
  eol_sprite_free(&image->image);
  free(image);
  component->componentData = NULL;
}

/*


**** DRAW ****


*/


void eol_component_slider_draw(eolComponent *component, eolRect bounds)
{
  eolRect r;
  eolVec2D drawPosition;
  eolComponentSlider *slider = eol_component_get_slider_data(component);
  if (slider == NULL)return;
  eol_rect_copy(&r,bounds);
  if (slider->vertical)
  {
    r.w = bounds.w * 0.5;
    r.x += r.w * 0.5;
    drawPosition.x = r.x + (r.w * 0.5);
    drawPosition.y = r.y + (r.h * slider->position);
  }
  else
  {
    r.h = bounds.h * 0.5;
    r.y += r.h * 0.5;
    drawPosition.y = r.y + (r.h * 0.5);
    drawPosition.x = r.x + (r.w * slider->position);
  }
  
  eol_draw_solid_rect(r,slider->barColor,1);
  eol_draw_rect(r,eol_vec3d(1,1,1),1);
  if (slider->sliderState == eolSliderIdle)
  {
    if (slider->slider != NULL)
    {
      drawPosition.x -= slider->slider->frameWidth / 2;
      drawPosition.y -= slider->slider->frameHeight / 2;
      eol_sprite_draw(slider->slider,0,drawPosition.x,drawPosition.y);
    }
    else
    {
      /*TODO: draw a slider*/
    }
  }
  else
  {
    if (slider->sliderHigh != NULL)
    {
      drawPosition.x -= slider->sliderHigh->frameWidth / 2;
      drawPosition.y -= slider->sliderHigh->frameHeight / 2;
      eol_sprite_draw(slider->sliderHigh,0,drawPosition.x,drawPosition.y);
    }
    else
    {
      /*TODO: draw a slider*/
    }
  }
}

void eol_component_entry_draw(eolComponent *component, eolRect bounds)
{
  eolRect r;
  eolComponentEntry *entry = eol_component_get_entry_data(component);
  if (entry == NULL)return;
  r.x = bounds.x - 1;
  r.y = bounds.y - 1;
  r.w = bounds.w + 2;
  r.h = bounds.h + 2;
  eol_draw_solid_rect(r,eol_vec3d(1,1,1),1);
  eol_draw_solid_rect(bounds,entry->bgcolor,1);
  if (entry->buffer->len <= 0)return;
  if (entry->font == NULL)
  {
    if (entry->wordWrap)
    {
      eol_font_draw_text_block(
      entry->buffer->str,
      bounds.x,
      bounds.y,
      bounds.w,
      0,
      entry->color,
      entry->alpha,
      entry->fontSize
      );
    }
    else
    {
      eol_font_draw_text_justify(
        entry->buffer->str,
        bounds.x,
        bounds.y,
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
        entry->buffer->str,
        bounds,
        entry->color,
        entry->alpha,
        entry->font
      );
    }
    else
    {
      eol_font_draw_text_justify_custom(
        entry->buffer->str,
        bounds.x,
        bounds.y,
        entry->color,
        entry->alpha,
        entry->font,
        entry->justify
      );
    }
  }
}

void eol_component_label_draw(eolComponent *component, eolRect bounds)
{
  eolComponentLabel *label = eol_component_get_label_data(component);
  if (label == NULL)return;
  if (label->buffer == NULL)return;
  if (label->font == NULL)
  {
    if (label->wordWrap)
    {
      eol_font_draw_text_block(
      label->buffer->str,
      component->bounds.x,
      component->bounds.y,
      component->bounds.w,
      0,
      label->color,
      label->alpha,
      label->fontSize
      );
    }
    else
    {
      eol_font_draw_text_justify(
        label->buffer->str,
        component->bounds.x,
        component->bounds.y,
        label->color,
        label->alpha,
        label->fontSize,
        label->justify
      );
    }
  }
  else
  {
    if (label->wordWrap)
    {
      eol_font_draw_text_block_custom(
        label->buffer->str,
        component->bounds,
        label->color,
        label->alpha,
        label->font
      );
    }
    else
    {
      eol_font_draw_text_justify_custom(
        label->buffer->str,
        component->bounds.x,
        component->bounds.y,
        label->color,
        label->alpha,
        label->font,
        label->justify
      );
    }
  }
}

void eol_component_draw(eolComponent *component,eolRect bounds)
{
  eolRect drawRect;
  if (!component)return;
  if (component->data_draw == NULL)return;
  drawRect.x = bounds.x + (bounds.w * component->rect.x);
  drawRect.y = bounds.y + (bounds.h * component->rect.y);
  if(component->rect.w <= 1)
    drawRect.w = (bounds.w * component->rect.w);
  else drawRect.w = component->rect.w;
  if(component->rect.h <= 1)
    drawRect.h = (bounds.h * component->rect.h);
  else drawRect.h = component->rect.h;
  component->data_draw(component,drawRect);
}

/*

  ***** Update *****
  
 */


eolBool eol_component_slider_update(eolComponent *component)
{
  eolComponentSlider *slider;
  eolRect sliderRect;
  eolVec2D drawPosition;
  eolInt x = 0,y = 0;
  if (!component)return eolFalse;
  slider = eol_component_get_slider_data(component);
  if (slider == NULL)return eolFalse;
  eol_mouse_get_position(&x,&y);
  
  if (slider->vertical)
  {
    drawPosition.x = component->bounds.x + (component->bounds.w * 0.5);
    drawPosition.y = component->bounds.y + (component->bounds.h * slider->position);
  }
  else
  {
    drawPosition.y = component->bounds.y + (component->bounds.h * 0.5);
    drawPosition.x = component->bounds.x + (component->bounds.w * slider->position);
  }
  
  if (slider->slider != NULL)
  {
    sliderRect.x = drawPosition.x - (slider->slider->frameWidth /2);
    sliderRect.y = drawPosition.y - (slider->slider->frameHeight /2);
    sliderRect.w = slider->slider->frameWidth;
    sliderRect.h = slider->slider->frameHeight;
  }
  else
  {
    /*TODO: support drawn sliders*/
  }
  if ((slider->sliderState == eolSliderHeld) && (eol_mouse_input_released(eolMouseLeft)))
  {
    return eolTrue;
  }
  if ((slider->sliderState == eolSliderHeld) && (eol_mouse_input_held(eolMouseLeft)))
  {
    slider->oldPosition = slider->position;
    if (slider->vertical)
    {
      if (y < component->bounds.y)y = component->bounds.y;
      if (y > component->bounds.y + component->bounds.h)y = component->bounds.y + component->bounds.h;
      y -= component->bounds.y;
      if (component->bounds.h != 0)slider->position = (float)y / component->bounds.h;
    }
    else
    {
      if (x < component->bounds.x)x = component->bounds.x;
      if (x > (component->bounds.x + component->bounds.w))x = component->bounds.x + component->bounds.w;
      x -= component->bounds.x;
      if (component->bounds.w != 0)slider->position = (float)x / component->bounds.w;
    }
    return eolFalse;
  }
  if (eol_mouse_in_rect(sliderRect))
  {
    slider->sliderState = eolSliderHigh;
    if (eol_mouse_input_pressed(eolMouseLeft))
    {
      slider->sliderState = eolSliderHeld;
    }
  }
  else
  {
    slider->sliderState = eolSliderIdle;
  }
  return eolFalse;
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

void eol_entry_assign_output(eolComponent *component)
{
  eolComponentEntry * entry = NULL;
  entry = eol_component_get_entry_data(component);
  if (entry == NULL)
  {
    eol_logger_message(EOL_LOG_WARN,"eol_component:unable to assign entry output\n");
    return;
  }
  if (entry->output == NULL)
  {
    eol_logger_message(EOL_LOG_WARN,"eol_component:unable to assign output.  NULL pointer specified\n");
    return;
  }
  strncpy(entry->output,entry->buffer->str,entry->bufferLimit);
}

/*

  ****** Makers ******

*/

void eol_component_make_slider(
    eolComponent * component,
    eolBool        vertical,
    eolLine        sliderFile,
    eolLine        sliderHigh,
    eolLine        bar,
    eolLine        cap1,
    eolLine        cap2,
    eolVec3D       barColor,
    eolVec3D       sliderColor,
    eolFloat       startPosition,
    eolUint        sliderType
  )
{
  eolComponentSlider * slider = NULL;
  if (!component)return;
  eol_component_slider_new(component);
  slider = eol_component_get_slider_data(component);
  if (slider == NULL)
  {
    return;
  }
  
  slider->vertical = vertical;
  switch(sliderType)
  {
    case eolSliderCommon:
      if (vertical)
      {
        slider->slider = _eol_component_slider[0];
        slider->sliderHigh = _eol_component_slider[1];
      }
      else
      {
        slider->slider = _eol_component_slider[2];
        slider->sliderHigh = _eol_component_slider[3];
      }
      break;
    case eolSliderStock:
    case eolSliderCustom:
      slider->slider = eol_sprite_load(sliderFile,-1,-1);
      slider->sliderHigh = eol_sprite_load(sliderHigh,-1,-1);
      slider->bar = eol_sprite_load(bar,-1,-1);
      slider->cap1 = eol_sprite_load(cap1,-1,-1);
      slider->cap2 = eol_sprite_load(cap2,-1,-1);
      break;
  }
  slider->sliderType = sliderType;
  eol_vec3d_copy(slider->barColor,barColor);
  eol_vec3d_copy(slider->sliderColor,sliderColor);
  slider->position = slider->oldPosition = startPosition;
  
  component->data_free = eol_component_slider_free;
  component->data_draw = eol_component_slider_draw;
  component->data_update = eol_component_slider_update;
}

void eol_component_make_entry(
    eolComponent * component,
    char         * output,
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
 
  entry->buffer = g_string_new(output);
  if (entry->buffer == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_component:Failed to duplicate string for new entry\n");
    eol_component_entry_free(component);
    return;
  }
  entry->output = output;
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


void eol_component_make_label(
    eolComponent * component,
    char         * text,
    eolUint        justify,
    eolInt         fontSize,
    eolBool        wordWrap,
    char         * fontName,
    eolVec3D       color,
    eolFloat       alpha
  )
{
  eolRect r;
  eolComponentLabel * label = NULL;
  if (!component)return;
  eol_component_label_new(component);
  
  label = eol_component_get_label_data(component);
  if (label == NULL)
  {
    return;
  }

  label->buffer = g_string_new(text);
  if (label->buffer == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_component:Failed to duplicate string for new label\n");
    eol_component_label_free(component);
    return;
  }
  
  if ((fontName != NULL)&&(strlen(fontName) > 0))
  {
    label->font = eol_font_load(fontName,fontSize);
    r = eol_font_get_bounds_custom(text,label->font);
  }
  else
  {
    r = eol_font_get_bounds(text,fontSize);
  }
  if (r.w > component->bounds.w)component->bounds.w = r.w;
  if (r.h > component->bounds.h)component->bounds.h = r.h;
  label->justify = justify;
  label->fontSize = fontSize;
  label->alpha = alpha;
  label->wordWrap = wordWrap;
  eol_vec3d_copy(label->color,color);
  component->data_free = eol_component_label_free;
  component->data_draw = eol_component_label_draw;
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

void eol_component_slider_new(eolComponent *component)
{
  if (component->componentData != NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_component:tried to make a label out of an existing component\n");
    return;
  }
  component->componentData = malloc(sizeof(eolComponentSlider));
  if (component->componentData == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_actor: failed to allocate data for new slider\n");
    return;
  }
  memset(component->componentData,0,sizeof(eolComponentSlider));
  component->type = eolSliderComponent;
}

void eol_component_label_new(eolComponent *component)
{
  if (component->componentData != NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_component:tried to make a label out of an existing component\n");
    return;
  }
  component->componentData = malloc(sizeof(eolComponentLabel));
  if (component->componentData == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_actor: failed to allocate data for new label\n");
    return;
  }
  memset(component->componentData,0,sizeof(eolComponentLabel));
  component->type = eolLabelComponent;
}

void eol_label_set_text(eolComponent *comp,char *text)
{
  eolComponentLabel * label = NULL;
  if (!text)return;
  label = eol_component_get_label_data(comp);
  if (label == NULL)return;
  label->buffer = g_string_assign(label->buffer,text);
}

eolFloat eol_slider_get_position(eolComponent *comp)
{
  eolComponentSlider * slider = NULL;
  slider = eol_component_get_slider_data(comp);
  if (!slider)return 0;
  return slider->position;
}

void eol_slider_set_position(eolComponent *comp, eolFloat newPos)
{
  eolComponentSlider * slider = NULL;
  slider = eol_component_get_slider_data(comp);
  if (!slider)return;
  if (newPos < 0)newPos = 0;
  if (newPos > 1)newPos = 1;
  slider->position = newPos;
}

/*

  ******* new components *******

 */

eolComponent *eol_slider_common_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    eolBool        vertical,
    eolVec3D       barColor,
    eolFloat       startPosition
  )
{
  if (vertical)
  {
    return eol_slider_new(id,
                          name,
                          rect,
                          bounds,
                          vertical,
                          "",
                          "",
                          "",
                          "",
                          "",
                          barColor,
                          eol_vec3d(1,1,1),
                          startPosition,
                          eolSliderCommon
                          );
  }
  else
  {
    
    return eol_slider_new(id,
                          name,
                          rect,
                          bounds,
                          vertical,
                          "",
                          "",
                          "",
                          "",
                          "",
                          barColor,
                          eol_vec3d(1,1,1),
                          startPosition,
                          eolSliderCommon
                          );
  }
}

eolComponent *eol_slider_new(
    eolUint       id,
    eolWord       name,
    eolRectFloat  rect,
    eolRect       bounds,
    eolBool       vertical,
    eolLine       slider,
    eolLine       sliderHigh,
    eolLine       bar,
    eolLine       cap1,
    eolLine       cap2,
    eolVec3D      barColor,
    eolVec3D      sliderColor,
    eolFloat      startPosition,
    eolUint       sliderType
  )
{
  eolComponent *component = NULL;
  component = eol_component_new();
  if (!component)return NULL;
  
  eol_component_make_slider(component,
                            vertical,
                            slider,
                            sliderHigh,
                            bar,
                            cap1,
                            cap2,
                            barColor,
                            sliderColor,
                            startPosition,
                            sliderType);
  if (component->componentData == NULL)
  {
    eol_component_free(&component);
    return NULL;
  }
  component->id = id;
  eol_word_cpy(component->name,name);
  component->canHasFocus = eolTrue;
  component->type = eolSliderComponent;
  eol_rectf_copy(&component->rect,rect);
  eol_component_get_rect_from_bounds(&component->bounds,bounds, rect);
  if ((rect.w <= 1) && (rect.w >= 0))
    component->bounds.w = bounds.w * rect.w;
  else component->bounds.w = rect.w;
  if ((rect.h <= 1)  && (rect.h >= 0))
    component->bounds.h = bounds.h * rect.h;
  else component->bounds.h = rect.h;
  return component;

}

eolComponent *eol_label_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    eolBool        canHasFocus,
    char         * text,
    eolUint        justify,
    eolBool        wordWrap,
    eolInt         fontSize,
    char         * fontName,
    eolVec3D       color,
    eolFloat       alpha
  )
{
  eolComponent *component = NULL;
  if (!text)return NULL;
  component = eol_component_new();
  if (!component)return NULL;
  eol_component_get_rect_from_bounds(&component->bounds,bounds, rect);
  eol_component_make_label(
    component,
    text,
    justify,
    fontSize,
    wordWrap,
    fontName,
    color,
    alpha
  );
  if (component->componentData == NULL)
  {
    eol_component_free(&component);
    return NULL;
  }
  component->id = id;
  strncpy(component->name,name,EOLWORDLEN);
  eol_rectf_copy(&component->rect,rect);
  component->canHasFocus = canHasFocus;
  component->type = eolLabelComponent;
  return component;
}

eolComponent *eol_line_entry_new(
    eolUint       id,
    eolWord       name,
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


eolComponent *eol_entry_new(
    eolUint       id,
    eolWord       name,
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
  component->canHasFocus = eolTrue;
  component->type = eolEntryComponent;
  eol_component_get_rect_from_bounds(&component->bounds,bounds, rect);
  return component;
}

/*utility functions*/
void eol_component_get_rect_from_bounds(eolRect *rect,eolRect canvas, eolRectFloat bounds)
{
  eolRectFloat rbounds;
  if (!rect)return;

  if (bounds.x < -1)
  {
    /*raw number from right edge*/
    rbounds.x = (canvas.w + bounds.x) / (float)canvas.w;
  }
  else if (bounds.x < 0)
  {
    /*percent of canvas from right*/
    rbounds.x = 1 + bounds.x;
  }
  else if (bounds.x <= 1)
  {
    /*percent of canvas from left*/
    rbounds.x = bounds.x;
  }
  else
  {
    rbounds.x = bounds.x / (float)canvas.w;
  }

  if (bounds.y < -1)
  {
    /*raw number from right edge*/
    rbounds.y = (canvas.h + bounds.y) / (float)canvas.h;
  }
  else if (bounds.y < 0)
  {
    /*percent of canvas from right*/
    rbounds.y = 1 + bounds.y;
  }
  else if (bounds.y <= 1)
  {
    /*percent of canvas from left*/
    rbounds.y = bounds.y;
  }
  else
  {
    rbounds.y = bounds.y / (float)canvas.h;
  }


  if (bounds.w <= 0)
  {
    /*fill to end*/
    rbounds.w = 1 - rbounds.x;
  }
  else if (bounds.w > 1)
  {
    /*exact size*/
    rbounds.w = bounds.w / (float)canvas.w;
  }
  else
  {
    /*% of canvas*/
    rbounds.w = bounds.w;
  }

  if (bounds.h <= 0)
  {
    /*fill to end*/
    rbounds.h = 1 - rbounds.y;
  }
  else if (bounds.h > 1)
  {
    /*exact size*/
    rbounds.h = bounds.h / (float)canvas.h;
  }
  else
  {
    /*% of canvas*/
    rbounds.h = bounds.h;
  }
  
  rect->x = canvas.x + (rbounds.x * canvas.w);
  rect->y = canvas.y + (rbounds.y * canvas.h);
  rect->w = canvas.w * rbounds.w;
  rect->h = canvas.h * rbounds.h;
}

/*eol@eof*/

