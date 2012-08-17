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
  eolUint     input;                    /**<if defined, the input will operate as a hotkey*/
  eolUint     hotkeymod;
  eolInt      justify;
  eolUint     fontSize;
  eolFloat    alpha;
  eolLine     buttonText;               /**<text to display over button...should it be a label component?*/
  eolUint     buttonType;               /**<if its an image, or raw text or both*/
  eolSprite * button[eolButtonStateMax];/**<if defined, it will use these over
  stock button images*/
}eolComponentButton;

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

typedef struct
{
  eolUint   listCount;
  eolInt    focusItem;
  eolUint   listType;
  eolVec2D  itemBounds; /**<width (x) and height limit of items in the list*/
  eolVec2D  displayItems;
  GList   * itemList;   /**<list of eolComponent's*/
  GList   * topItem;
  GList   * selection;
}eolComponentList;

/*local global variables*/
eolSprite * _eol_component_stock_button[3] = {NULL,NULL,NULL};
eolSprite * _eol_component_slider[4] = {NULL,NULL,NULL,NULL};
eolLine     _eol_component_slider_files[4];
eolVec3D    _eol_component_button_color[3];
eolInt      _eol_component_button_offset_x = 0;
eolInt      _eol_component_button_offset_y = 0;
eolBool     _eol_component_button_text_outline = eolTrue;

/*local function prototypes*/
void eol_component_button_new(eolComponent *component);
void eol_component_label_new(eolComponent *component);
void eol_component_label_free(eolComponent *component);
void eol_component_entry_new(eolComponent *component);
void eol_component_list_new(eolComponent *component);
void eol_component_slider_new(eolComponent *component);

eolBool eol_component_has_changed(eolComponent *component);
eolInt eol_component_get_state(eolComponent *component);

void eol_component_textinput_new(eolComponent *component);
void eol_component_textinput_get_text(eolComponent *component,char *text);
eolComponentEntry *eol_component_textinput_get_component(eolComponent *component);

void eol_component_actor_new(eolComponent *component);
void eol_component_actor_load(eolComponent *component,char * filename);

eolComponentButton *eol_component_get_button_data(eolComponent *component);

void eol_component_button_free(eolComponent *component);
void eol_component_label_free(eolComponent *component);
void eol_component_entry_free(eolComponent *component);
void eol_component_actor_free(eolComponent *component);
void eol_component_slider_free(eolComponent *component);
void eol_component_list_free(eolComponent *component);
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
  eolLine buttonfile,buttonhitfile,buttonhighfile;
  eolConfig *conf = NULL;
  _eol_component_button_offset_x = 0;
  _eol_component_button_offset_y = 0;
  eol_vec3d_set(_eol_component_button_color[0],0.8,0.8,0.8);
  eol_vec3d_set(_eol_component_button_color[1],1,1,0);
  eol_vec3d_set(_eol_component_button_color[2],0.6,0.6,0.6);
  eol_line_cpy(buttonfile,"images/UI/btn.png");
  eol_line_cpy(buttonhitfile,"images/UI/btn_hit.png");
  eol_line_cpy(buttonhighfile,"images/UI/btn_high.png");
  
  eol_line_cpy(sliderVfile,"images/UI/slider_v.png");
  eol_line_cpy(sliderVhighfile,"images/UI/slider_v_high.png");
  eol_line_cpy(sliderHfile,"images/UI/slider.png");
  eol_line_cpy(sliderHhighfile,"images/UI/slider_high.png");
  
  conf = eol_config_load("system/component.cfg");
  if (conf != NULL)
  {
    eol_config_get_int_by_tag(&_eol_component_button_offset_x,
                              conf,
                              "button_x_offset");
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
    eol_config_get_vec3d_by_tag(&_eol_component_button_color[0],conf,"button_text_color");
    eol_config_get_vec3d_by_tag(&_eol_component_button_color[1],conf,"button_high_text_color");
    eol_config_get_vec3d_by_tag(&_eol_component_button_color[2],conf,"button_hit_text_color");
    eol_config_get_bool_by_tag(&_eol_component_button_text_outline,conf,"button_text_outline");
    
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

  _eol_component_stock_button[0] = eol_sprite_load(buttonfile,-1,-1);
  _eol_component_stock_button[1] = eol_sprite_load(buttonhighfile,-1,-1);
  _eol_component_stock_button[2] = eol_sprite_load(buttonhitfile,-1,-1);
  
  _eol_component_slider[0] = eol_sprite_load(sliderVfile,-1,-1);
  _eol_component_slider[1] = eol_sprite_load(sliderVhighfile,-1,-1);
  _eol_component_slider[2] = eol_sprite_load(sliderHfile,-1,-1);
  _eol_component_slider[3] = eol_sprite_load(sliderHhighfile,-1,-1);
  eol_line_cpy(_eol_component_slider_files[0],sliderVfile);
  eol_line_cpy(_eol_component_slider_files[1],sliderVhighfile);
  eol_line_cpy(_eol_component_slider_files[2],sliderHfile);
  eol_line_cpy(_eol_component_slider_files[3],sliderHhighfile);
  
}

void eol_button_get_stock_size(eolUint *w, eolUint *h)
{
  if (_eol_component_stock_button[0] == NULL)return;
  if (w)*w = _eol_component_stock_button[0]->frameWidth;
  if (h)*h = _eol_component_stock_button[0]->frameHeight;
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

eolComponentList *eol_component_get_list_data(eolComponent *component)
{
  if ((!component)||
    (!component->componentData)||
    (component->type != eolListComponent))
  {
    return NULL;
  }
  return (eolComponentList*)component->componentData;
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

void eol_component_list_free(eolComponent *component)
{
  GList *l;
  eolComponentList * list = eol_component_get_list_data(component);
  if (list == NULL)return;
  l = list->itemList;
  while (l != NULL)
  {
    eol_component_free((eolComponent**)&l->data);
  }
  free(list);
  g_list_free(list->itemList);
  list->itemList = NULL;
  free(list);
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
      bounds.x,
      bounds.y,
      bounds.w,
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
        bounds.x,
        bounds.y,
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
        bounds,
        label->color,
        label->alpha,
        label->font
      );
    }
    else
    {
      eol_font_draw_text_justify_custom(
        label->buffer->str,
        bounds.x,
        bounds.y,
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
      if (component->bounds.h != 0)slider->position = y / component->bounds.h;
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
      if (eol_input_is_input_down("LShift") ||
          eol_input_is_input_down("RShift"))
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
  component->bounds.w = r.w;
  component->bounds.h = r.h;
  label->justify = justify;
  label->fontSize = fontSize;
  label->alpha = alpha;
  label->wordWrap = wordWrap;
  eol_vec3d_copy(label->color,color);
  component->data_free = eol_component_label_free;
  component->data_draw = eol_component_label_draw;
}

eolBool eol_component_button_update(eolComponent *component)
{
  eolVec2D v;
  eolInt x,y;
  eolBool mod = eolTrue;
  eolComponentButton *button = NULL;
  if (!component)return eolFalse;
  button = eol_component_get_button_data(component);
  
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

void eol_component_button_draw(eolComponent *component,eolRect bounds)
{
  eolRect r;
  eolComponentButton *button = NULL;
  eolSprite *img = NULL;
  eolInt x,y;
  eolUint ofx = 0, ofy = 0;
  button = eol_component_get_button_data(component);
  x = bounds.x;
  y = bounds.y;
  if (button == NULL)return;
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
                      bounds.x,
                      bounds.y);
      x = bounds.x + (img->frameWidth/2);
      y = bounds.y + (img->frameHeight/2) - (r.h*0.5);
      ofx = _eol_component_button_offset_x;
      ofy = _eol_component_button_offset_y;
    }
    r.x = x + ofx;
    r.y = y + ofy;
    eol_font_draw_text_justify(
      button->buttonText,
      r.x,
      r.y,
      _eol_component_button_color[component->state],
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
    eolLine        buttonDownFile
  )
{
  eolRect r;
  eolComponentButton * button = NULL;
  if (!component)return;
  eol_component_button_new(component);

  button = eol_component_get_button_data(component);
  if (button == NULL)
  {
    return;
  }
  button->alpha = 1;
  button->fontSize = 3;
  button->input = buttonHotkey;
  button->hotkeymod = buttonHotkeymod;
  
  strncpy(button->buttonText,buttonText,EOLLINELEN);
  button->buttonType = buttonType;
  switch(buttonType)
  {
    case eolButtonCustom:
      button->button[eolButtonIdle] = eol_sprite_load(buttonUpFile,-1,-1);
      button->button[eolButtonHighlight] = eol_sprite_load(buttonHighFile,-1,-1);
      button->button[eolButtonPressed] = eol_sprite_load(buttonDownFile,-1,-1);
      button->justify = eolJustifyCenter;
      component->bounds.w = button->button[eolButtonIdle]->frameWidth;
      component->bounds.h = button->button[eolButtonIdle]->frameHeight;
      break;
    case eolButtonText:
      button->button[eolButtonIdle] = NULL;
      button->button[eolButtonHighlight] = NULL;
      button->button[eolButtonPressed] = NULL;
      r = eol_font_get_bounds(buttonText,button->fontSize);
      component->bounds.w = r.w;
      component->bounds.h = r.h;
      button->justify = eolJustifyLeft;
      break;
    case eolButtonStock:
      button->button[eolButtonIdle] = _eol_component_stock_button[eolButtonIdle];
      button->button[eolButtonHighlight] = _eol_component_stock_button[eolButtonHighlight];
      button->button[eolButtonPressed] = _eol_component_stock_button[eolButtonPressed];
      button->justify = eolJustifyCenter;
      if (button->button[eolButtonIdle] != NULL)
      {
        component->bounds.w = button->button[eolButtonIdle]->frameWidth;
        component->bounds.h = button->button[eolButtonIdle]->frameHeight;
      }
      break;
  }
  component->data_update = eol_component_button_update;
  component->data_free = eol_component_button_free;
  component->data_draw = eol_component_button_draw;
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
      "eol_actor: failed to allocate data for new label\n");
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
    eolFloat       startPosition,
    eolUint        sliderType
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
  
  component->bounds.x = bounds.x + (bounds.w * rect.x);
  component->bounds.y = bounds.y + (bounds.h * rect.y);
  if (rect.w <= 1)
    component->bounds.w = bounds.w * rect.w;
  else component->bounds.w = rect.w;
  if (rect.h <= 1)
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
  component->bounds.x = bounds.x + (bounds.w * rect.x);
  component->bounds.y = bounds.y + (bounds.h * rect.y);
  return component;
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
    eolButtonStock,
    buttonHotkey,
    buttonHotkeymod,
    center,
    NULL,
    NULL,
    NULL
  );
}

eolComponent *eol_button_text_new(
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
    eolButtonText,
    buttonHotkey,
    buttonHotkeymod,
    center,
    NULL,
    NULL,
    NULL
  );
}

eolComponent *eol_button_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    char         * buttonText,
    eolUint        buttonType,
    eolInt         buttonHotkey,
    eolUint        buttonHotkeymod,
    eolBool        center,
    char         * buttonFileUp,
    char         * buttonFileHigh,
    char         * buttonFileDown
  )
{
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
    buttonFileDown
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
  component->type = eolButtonComponent;
  component->bounds.x = bounds.x + (bounds.w * rect.x);
  component->bounds.y = bounds.y + (bounds.h * rect.y);
  if (rect.w > 1)component->bounds.w = (eolUint)rect.w;
  if (rect.h > 1)component->bounds.h = (eolUint)rect.h;
  if (center)
  {
    component->bounds.x -= component->bounds.w/2;
    component->bounds.y -= component->bounds.h/2;
    if (bounds.w != 0)
    {
      component->rect.x -= (component->bounds.w/(float)bounds.w)/2;
    }
    if (bounds.h != 0)
    {
      component->rect.y -= (component->bounds.h/(float)bounds.h)/2;
    }
  }
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
  component->bounds.w = bounds.w;
  component->bounds.h = bounds.h;
  component->bounds.x = bounds.x + (bounds.w * rect.x);
  component->bounds.y = bounds.y + (bounds.h * rect.y);
  
  return component;
}

/*eol@eof*/

