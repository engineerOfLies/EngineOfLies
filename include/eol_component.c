#include "eol_component.h"
#include "eol_logger.h"
#include "eol_font.h"
#include "eol_sprite.h"
#include "eol_actor.h"
#include "eol_mouse.h"
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
  char    * buffer;
  eolInt    bufferLimit; /**<if -1 no limit, otherwise its the maximum character
  that will be added to buffer*/
  eolUint   fontSize;    /**<size of the font to use when displaying the text*/
  eolFont * font;    /**<if defined, it will use the custom font to draw text*/
  eolBool   number; /**<if true, limits input to 0-9 - and .*/
}eolComponentInput;

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
  eolSprite *slider;
  eolSprite *bar;
  eolBool    vertical;
  eolFloat   position;
  eolFloat   oldPosition;
}eolComponentSlider;

typedef struct
{
  eolUint   listCount;
  eolInt    focusItem;
  eolUint   listType;
  eolVec2D  itemBounds; /**<width (x) and height limit of items in the list*/
  GList   * itemList;   /**<list of eolComponent's*/
}eolComponentList;

/*local global variables*/
eolSprite * _eol_component_stock_button[3] = {NULL,NULL,NULL};
eolVec3D    _eol_component_button_color[3];

/*local function prototypes*/
void eol_component_button_new(eolComponent *component);
void eol_component_label_new(eolComponent *component);
void eol_component_label_free(eolComponent *component);

void eol_component_textinput_new(eolComponent *component);
void eol_component_textinput_get_text(eolComponent *component,char *text);
eolComponentInput *eol_component_textinput_get_component(eolComponent *component);

void eol_component_actor_new(eolComponent *component);
void eol_component_actor_load(eolComponent *component,char * filename);

eolComponentButton *eol_component_get_button_data(eolComponent *component);

void eol_component_button_free(eolComponent *component);
void eol_component_label_free(eolComponent *component);
void eol_component_textinput_free(eolComponent *component);
void eol_component_actor_free(eolComponent *component);
void eol_component_slider_free(eolComponent *component);
void eol_component_list_free(eolComponent *component);
void eol_component_image_free(eolComponent *component);

void eol_component_make_label(
    eolComponent * component,
    char         * text,
    eolUint        justify,
    eolInt         fontSize,
    char         * fontName,
    eolVec3D       color,
    eolFloat       alpha
  );

/*definitions*/
void eol_component_config()
{
  /*TODO: load from config file*/
  _eol_component_stock_button[0] = eol_sprite_load("images/UI/btn.png",-1,-1);
  _eol_component_stock_button[1] = eol_sprite_load("images/UI/btn_high.png",-1,-1);
  _eol_component_stock_button[2] = eol_sprite_load("images/UI/btn_hit.png",-1,-1);
  eol_vector_set_3D(_eol_component_button_color[0],0.8,0.8,0.8);
  eol_vector_set_3D(_eol_component_button_color[1],1,1,0);
  eol_vector_set_3D(_eol_component_button_color[2],0.6,0.6,0.6);
}

void eol_button_get_stock_size(eolUint *w, eolUint *h)
{
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
eolBool eol_component_has_changed(eolComponent *component);
eolInt eol_component_get_state(eolComponent *component);

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

eolComponentInput *eol_component_get_input_data(eolComponent *component)
{
  if ((!component)||
    (!component->componentData)||
    (component->type != eolInputComponent))
  {
    return NULL;
  }
  return (eolComponentInput*)component->componentData;
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

void eol_component_textinput_free(eolComponent *component)
{
  eolComponentInput *input = eol_component_get_input_data(component);
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
  eol_sprite_free(&slider->bar);
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
      bounds.h,
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
  if (!component)return;
  if (component->data_draw == NULL)return;
  eolRect drawRect;
  drawRect.x = bounds.x + (bounds.w * component->rect.x);
  drawRect.y = bounds.y + (bounds.h * component->rect.y);
  drawRect.w = (bounds.w * component->rect.w);
  drawRect.h = (bounds.h * component->rect.h);
  component->data_draw(component,drawRect);
}

void eol_component_make_label(
    eolComponent * component,
    char         * text,
    eolUint        justify,
    eolInt         fontSize,
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
  eol_vec3d_copy(label->color,color);
  component->data_free = eol_component_label_free;
  component->data_draw = eol_component_label_draw;
}

eolBool eol_component_button_update(eolComponent *component)
{
  eolVec2D v;
  eolInt x,y;
  component->oldState = component->state;
  eol_mouse_get_position(&x,&y);
  v.x = x;
  v.y = y;
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
  }
  else
  {
    component->state = eolButtonIdle;
  }
  return eolFalse;
}

void eol_component_button_draw(eolComponent *component,eolRect bounds)
{
  eolRect r;
  eolComponentButton *button = NULL;
  eolSprite *img = NULL;
  eolInt x,y;
  button = eol_component_get_button_data(component);
  x = bounds.x;
  y = bounds.y;
  if (button == NULL)return;
  if ((component->state >= 0) &&
    (component->state < eolButtonStateMax))
  {
    img = button->button[component->state];

    if (img != NULL)
    {
      r = eol_font_get_bounds(
        button->buttonText,
        button->fontSize
      );
      eol_sprite_draw(img,
                      0,
                      bounds.x,
                      bounds.y);
      x = bounds.x + (img->frameWidth/2);
      y = bounds.y + (img->frameHeight/2) - (r.h*0.7);
    }
    eol_font_draw_text_justify(
      button->buttonText,
      x,
      y,
      _eol_component_button_color[component->state],
      button->alpha,
      button->fontSize,
      button->justify
    );
  }
}

void eol_component_make_button(
    eolComponent * component,
    char         * buttonText,
    eolUint        buttonType,
    eolInt         buttonHotkey,
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
      component->bounds.w = button->button[eolButtonIdle]->frameWidth;
      component->bounds.h = button->button[eolButtonIdle]->frameHeight;
      break;
  }
  component->data_update = eol_component_button_update;
  component->data_free = eol_component_button_free;
  component->data_draw = eol_component_button_draw;
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
  if ((!comp) || (!text))return;
  label = eol_component_get_label_data(comp);
  if (label == NULL)
  {
    return;
  }
  label->buffer = g_string_assign(label->buffer,text);
}

eolComponent *eol_label_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    eolBool        canHasFocus,
    char         * text,
    eolUint        justify,
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
    eolBool        center
  )
{
  return eol_button_new(
    id,
    name,
    rect,
    bounds,
    eolTrue,
    buttonText,
    eolButtonStock,
    buttonHotkey,
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
    eolBool        canHasFocus,
    char         * buttonText,
    eolInt         buttonType,
    eolInt         buttonHotkey,
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
  component->canHasFocus = canHasFocus;
  component->type = eolButtonComponent;
  component->bounds.x = bounds.x + (bounds.w * rect.x);
  component->bounds.y = bounds.y + (bounds.h * rect.y);
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

/*eol@eof*/

