#include "eol_component.h"
#include "eol_logger.h"

/*local global variables*/

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

/*definitions*/

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
  switch((*component)->type)
  {
    case eolNullComponent:
      break;
    case eolLabelComponent:
      eol_component_label_free(*component);
      break;
    case eolButtonComponent:
      eol_component_button_free(*component);
      break;
    case eolInputComponent:
      eol_component_textinput_free(*component);
      break;
    case eolSliderComponent:
      eol_component_slider_free(*component);
      break;
    case eolImageComponent:
      eol_component_image_free(*component);
      break;
    case eolActorComponent:
      eol_component_actor_free(*component);
      break;
    case eolListComponent:
      eol_component_list_free(*component);
      break;
  }
  free(*component);
  *component = NULL;
}
void eol_component_update(eolComponent *component);
void eol_component_set_focus(eolComponent *component,eolBool focus);
eolBool eol_component_has_changed(eolComponent *component);
eolInt eol_component_get_state(eolComponent *component);
void eol_component_draw(eolComponent *component);

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

void eol_component_actor_free(eolComponent *component);
void eol_component_slider_free(eolComponent *component);
void eol_component_list_free(eolComponent *component);
void eol_component_image_free(eolComponent *component);

/*eol@eof*/

