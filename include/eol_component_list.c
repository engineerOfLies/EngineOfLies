#include "eol_component_list.h"
#include "eol_font.h"
#include "eol_logger.h"
#include "eol_draw.h"

/*local function prototypes*/
eolComponentList *eol_component_get_list_data(eolComponent *component);

void eol_component_make_list(
    eolComponent * component,
    eolUint   listType,
    eolBool   showHSlider,
    eolBool   showVSlider,
    eolVec2D  displayItems
  );

/*function definitions*/

eolVec2D eol_component_list_get_item_position(eolComponent *list,eolUint position)
{
  eolComponentList *ldata;
  eolVec2D pos = {0,0};
  eolUint  row = 0,col = 0;
  ldata = eol_component_get_list_data(list);
  if (!ldata)return pos;
  /*TODO: offset based on scroll position*/
  col = position % ldata->numPerRow;
  row = position / ldata->numPerRow;
  pos.x = row * (ldata->displayItems.x + ldata->itemPadding.x);
  pos.y = col * (ldata->displayItems.y + ldata->itemPadding.y);
  return pos;
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

eolComponent *eol_list_new(
    eolUint       id,
    eolWord       name,
    eolRectFloat  rect,
    eolRect       bounds,
    eolUint       listType,
    eolVec2D      itemDimensions,
    eolBool       showVSlider,
    eolBool       showHSlider,
    eolUint       fontSize,
    eolVec3D      textColor,
    eolFloat      alpha
  )
{
  eolRectFloat slideRect = {0,0,1,1};
  eolComponent *component = NULL;
  eolComponentList *list = NULL;
  component = eol_component_new();
  if (!component)return NULL;

  eol_component_make_list(
    component,
    listType,
    showHSlider,
    showVSlider,
    itemDimensions
  );

  list = eol_component_get_list_data(component);
  if (list == NULL)
  {
    eol_component_free(&component);
    return NULL;
  }
  component->id = id;
  eol_word_cpy(component->name,name);
  component->canHasFocus = eolTrue;
  eol_rectf_copy(&component->rect,rect);
  eol_component_get_rect_from_bounds(&component->bounds,bounds, rect);
  list->itemBounds.x = component->bounds.w;
  list->itemBounds.y = component->bounds.h;
  if (showVSlider)
  {
    list->vSliderBounds.h = component->bounds.h;
    list->vSliderBounds.y = component->bounds.y;
    list->vSliderBounds.w = 25;/*TODO: make a function for getting width of slider*/
    list->vSliderBounds.x = component->bounds.x + component->bounds.w;
    list->vSlider = eol_slider_common_new(
      0,
      "vertical_scroll",
      slideRect,
      list->vSliderBounds,
      eolTrue,
      eol_vec3d(0,0,0),
      0
    );
  }
  if (showHSlider)
  {
    list->hSliderBounds.w = component->bounds.w;
    list->hSliderBounds.x = component->bounds.x;
    list->hSliderBounds.h = 25;/*TODO: make a function for getting width of slider*/
    list->hSliderBounds.y = component->bounds.y + component->bounds.h;
    list->hSlider = eol_slider_common_new(
      0,
      "horizontal_scroll",
      slideRect,
      list->hSliderBounds,
      eolFalse,
      eol_vec3d(0,0,0),
      0
    );
  }
  list->fontSize = fontSize;
  eol_vec3d_copy(list->textColor,textColor);
  list->alpha = alpha;
  return component;
}


void eol_component_list_free(eolComponent *component)
{
  GList *l;
  eolComponentList * list = eol_component_get_list_data(component);
  if (list == NULL)return;
  eol_component_free(&list->vSlider);
  eol_component_free(&list->hSlider);
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

void eol_component_list_draw(eolComponent *component, eolRect bounds)
{
  eolRect r;
  GList *it = NULL;
  eolComponentList *list = eol_component_get_list_data(component);
  if (list == NULL)return;
  r.x = component->bounds.x - 1;
  r.y = component->bounds.y - 1;
  r.w = component->bounds.w + 2;
  r.h = component->bounds.h + 2;
  eol_draw_solid_rect(r,eol_vec3d(1,1,1),1);
  eol_draw_solid_rect(component->bounds,eol_vec3d(0.1,0.1,0.1),1);
  eol_component_draw(list->vSlider,list->vSliderBounds);
  eol_component_draw(list->hSlider,list->hSliderBounds);
  /*TODO: draw sliders
    draw list items*/
  
  for (it = list->itemList;it != NULL;it = it->next)
  {

  }
}

eolBool eol_component_list_update(eolComponent *component)
{
  eolComponentList *list;
  eolBool updated = eolFalse;
  list = eol_component_get_list_data(component);
  if (list == NULL)return eolFalse;
  /*Update Sliders if visible*/
  updated = (eol_component_update(list->vSlider) || updated);
  updated = (eol_component_update(list->hSlider) || updated);
  /*iterate through glist and update elements*/
  return updated;
}

void eol_component_make_list(
    eolComponent * component,
    eolUint   listType,
    eolBool   showHSlider,
    eolBool   showVSlider,
    eolVec2D  displayItems
  )
{
  eolComponentList * list = NULL;
  if (!component)return;
  eol_component_list_new(component);
  list = eol_component_get_list_data(component);
  if (list == NULL)
  {
    return;
  }
  switch(listType)
  {
    default:
    case eolListLines:
      break;
    case eolListBlock:
      break;
    case eolListDock:
      break;
  }
  list->listType = listType;
  eol_vec2d_copy(list->displayItems,displayItems);
  list->showVSlider = showVSlider;
  list->showHSlider = showHSlider;

  component->data_free = eol_component_list_free;
  component->data_draw = eol_component_list_draw;
  component->data_update = eol_component_list_update;
}


void eol_list_add_item(eolComponent *list,eolComponent *item)
{
  eolComponentList * ldata;
  ldata = eol_component_get_list_data(list);
  if ((!ldata) || (!item))return;/*no op*/
  ldata->itemCount++;
  ldata->itemList = g_list_append(ldata->itemList,item);
  /*Update item's rect and bounds to reflect position in list*/
}

void eol_list_add_text_item(
    eolComponent  * list,
    eolUint         itemId,
    eolLine         text
  )
{
  eolComponent *label = NULL;
  eolComponentList * ldata;
  ldata = eol_component_get_list_data(list);
  if (!ldata)return;
  label = eol_label_new(
    itemId,
    "label_item",
    list->rect,
    list->bounds,
    eolTrue,
    text,
    eolJustifyLeft,
    eolFalse,
    ldata->fontSize,
    NULL,
    ldata->textColor,
    ldata->alpha
  );
  eol_list_add_item(list,label);
}

void eol_component_list_new(eolComponent *component)
{
  if (component->componentData != NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_component:tried to make a list out of an existing component\n");
    return;
  }
  component->componentData = malloc(sizeof(eolComponentList));
  if (component->componentData == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_actor: failed to allocate data for new list\n");
    return;
  }
  memset(component->componentData,0,sizeof(eolComponentList));
  component->type = eolListComponent;
}


/*eol@eof*/
