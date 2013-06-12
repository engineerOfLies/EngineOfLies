#include "eol_component_percent_bar.h"
#include "eol_logger.h"
#include "eol_draw.h"

/*local function prototypes*/
void eol_component_percent_bar_new(eolComponent *component);
void eol_component_percent_bar_draw(eolComponent *component, eolRect bounds);
eolBool eol_component_percent_bar_update(eolComponent *component);
void eol_component_percent_bar_free(eolComponent *component);


/*function definitions*/

eolComponentPercentBar *eol_component_get_percent_bar_data(eolComponent *component)
{
  if ((!component)||
    (!component->componentData)||
    (component->type != eolPercentBarComponent))
  {
    return NULL;
  }
  return (eolComponentPercentBar*)component->componentData;
}


void eol_component_make_percent_bar(
    eolComponent  * component,
    eolBool         vertical,
    eolBool         ascending,
    eolFloat        percent,
    eolVec3D        statusColor,
    eolVec3D        backgroundColor,
    eolFloat        statusAlpha,
    eolFloat        backgroundAlpha
  )
{
  eolComponentPercentBar * bar = NULL;
  if (!component)return;
  eol_component_percent_bar_new(component);
  bar = eol_component_get_percent_bar_data(component);
  if (bar == NULL)
  {
    return;
  }
  bar->statusAlpha = statusAlpha;
  bar->backgroundAlpha = backgroundAlpha;
  bar->vertical = vertical;
  bar->percent = percent;
  bar->ascending = ascending;
  eol_vec3d_copy(bar->statusColor,statusColor);
  eol_vec3d_copy(bar->backgroundColor,backgroundColor);
  component->data_free = eol_component_percent_bar_free;
  component->data_draw = eol_component_percent_bar_draw;
  component->data_update = eol_component_percent_bar_update;
}

eolComponent *eol_percent_bar_new(
    eolUint       id,
    eolWord       name,
    eolRectFloat  rect,
    eolRect       bounds,
    eolBool       vertical,
    eolBool       ascending,
    eolFloat      percent,
    eolVec3D      statusColor,
    eolVec3D      backgroundColor,
    eolFloat      statusAlpha,
    eolFloat      backgroundAlpha
    )
{
  eolComponent *component = NULL;
  eolComponentPercentBar *bar = NULL;
  component = eol_component_new();
  if (!component)return NULL;
  eol_rectf_copy(&component->rect,rect);
  eol_component_get_rect_from_bounds(&component->bounds,bounds, rect);

  eol_component_make_percent_bar(
    component,
    vertical,
    ascending,
    percent,
    statusColor,
    backgroundColor,
    statusAlpha,
    backgroundAlpha
  );

  bar = eol_component_get_percent_bar_data(component);
  if (!bar)
  {
    eol_component_free(&component);
    return NULL;
  }
  component->id = id;
  eol_line_cpy(component->name,name);

  return component;
}

void eol_component_percent_bar_free(eolComponent *component)
{
  eolComponentPercentBar * bar = eol_component_get_percent_bar_data(component);
  if (bar == NULL)return;
  free(bar);
  component->componentData = NULL;
}

eolBool eol_component_percent_bar_update(eolComponent *component)
{
  eolComponentPercentBar *bar = eol_component_get_percent_bar_data(component);
  if (bar == NULL)return eolFalse;
  if ((bar->max != NULL) && (bar->value != NULL) && (*bar->max != 0))
  {
    bar->percent = *bar->value / *bar->max;
  }
  if (bar->percent > 1)bar->percent = 1;
  if (bar->percent < 0)bar->percent = 0;
  return eolFalse;
}

void eol_component_percent_bar_draw(eolComponent *component, eolRect bounds)
{
  eolRect r;
  eolComponentPercentBar *bar = eol_component_get_percent_bar_data(component);
  if (bar == NULL)return;
  eol_rect_copy(&r,component->bounds);
  eol_draw_solid_rect(component->bounds,bar->backgroundColor,bar->backgroundAlpha);
  if (bar->vertical)
  {
    r.h = component->bounds.h * bar->percent;
    if (bar->ascending)
    {
      r.y = component->bounds.y + (component->bounds.h - r.h);
    }
  }
  else
  {
    r.w = component->bounds.w * bar->percent;
    if (bar->ascending)
    {
      r.x = component->bounds.x + (component->bounds.w - r.w);
    }
  }
  eol_draw_solid_rect(r,bar->statusColor,bar->statusAlpha);
}

void eol_component_percent_bar_new(eolComponent *component)
{
  if (component->componentData != NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_component_percent_bar:tried to make a percent bar out of an existing component\n");
    return;
  }
  component->componentData = malloc(sizeof(eolComponentPercentBar));
  if (component->componentData == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_component_percent_bar: failed to allocate data for new percent bar\n");
    return;
  }
  memset(component->componentData,0,sizeof(eolComponentPercentBar));
  component->type = eolPercentBarComponent;
}

void eol_component_percent_bar_assign_values(eolComponent *component,eolFloat *max,eolFloat *value)
{
  eolComponentPercentBar *bar = eol_component_get_percent_bar_data(component);
  if (bar == NULL)return;
  bar->max = max;
  bar->value = value;
}

void eol_component_percent_bar_set(eolComponent *component,eolFloat percent)
{
  eolComponentPercentBar *bar = eol_component_get_percent_bar_data(component);
  if (bar == NULL)return;
  bar->percent = percent;
}

eolComponent *eol_percent_bar_create_from_config(eolKeychain *def,eolRect parentRect)
{
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

  if (!def)
  {
    eol_logger_message(EOL_LOG_WARN,"eol_component_get_percent_bar: passed bad def parameter");
    return NULL;
  }

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

  return eol_percent_bar_new(
    id,
    name,
    rect,
    parentRect,
    vertical,
    ascending,
    percent,
    statusColor,
    backgroundColor,
    statusAlpha,
    backgroundAlpha
  );
}
/*eol@eof*/
