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
  
void eol_component_list_select_item(
    eolComponent *component,
    eolComponentListItem *item
  );
  
/*function definitions*/

eolVec2D eol_component_list_get_item_position(eolComponent *list,eolUint position)
{
  eolComponentList *ldata;
  eolVec2D pos = {0,0};
  eolUint  row = position,col = 0;
  ldata = eol_component_get_list_data(list);
  if (!ldata)return pos;
  /*TODO: offset based on scroll position*/
  if (ldata->numPerRow > 1)
  {
    col = position % ldata->numPerRow;
    row = position / ldata->numPerRow;
  }
  pos.x = (col * (ldata->displayItems.x + ldata->itemPadding.x)) + list->bounds.x;
  pos.y = (row * (ldata->displayItems.y + ldata->itemPadding.y)) + list->bounds.y;
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

  eol_rectf_copy(&component->rect,rect);
  eol_component_get_rect_from_bounds(&component->bounds,bounds, rect);

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
  eolVec2D itemPos = {0,0};
  int position = 0;
  GList *it = NULL;
  eolComponentListItem *item = NULL;
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
  /*draw list items*/
  /*TODO make the iterator start at the top position of the scroll
    TODO check for visibility*/
  printf("items:\n");
  for (it = list->itemList,position = 0;it != NULL;it = it->next,position++)
  {
    if (!it->data)continue;
    item = (eolComponentListItem*)it->data;
    itemPos = eol_component_list_get_item_position(component,position);
    item->item->bounds.x = itemPos.x;
    item->item->bounds.y = itemPos.y;
    eol_component_draw(item->item,item->item->bounds);
    printf("\tdrawn item %i (%f,%f)\n",position,itemPos.x,itemPos.y);
  }
}

eolBool eol_component_list_update(eolComponent *component)
{
  GList *it = NULL;
  eolComponentList *list;
  eolComponentListItem *item = NULL;
  eolBool updated = eolFalse;
  list = eol_component_get_list_data(component);
  if (list == NULL)return eolFalse;
  /*Update Sliders if visible*/
  updated = (eol_component_update(list->vSlider) || updated);
  updated = (eol_component_update(list->hSlider) || updated);
  /*iterate through glist and update elements*/
  /*TODO: check for visibility*/
  for (it = list->itemList;it != NULL;it = it->next)
  {
    if (!it->data)continue;
    item = (eolComponentListItem*)it->data;
    updated = eol_component_update(item->item) || updated;
  }
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
  list->numPerRow = 1;
  list->itemPadding.x = 8;
  list->itemPadding.y = 8;
  switch(listType)
  {
    default:
    case eolListText:
      displayItems.y = eol_font_get_text_height_average(list->fontSize);
      displayItems.x = component->bounds.w;
    case eolListLines:
      break;
    case eolListBlock:
      break;
    case eolListDock:
      list->numPerRow = -1;
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

eolComponentListItem * eol_list_item_new()
{
  eolComponentListItem *item = NULL;
  item = (eolComponentListItem *)malloc(sizeof(eolComponentListItem));
  if (item == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_component_list: failed to allocate list item\n");
    return NULL;
  }
  memset(item,0,sizeof(eolComponentListItem));
  return item;
}

void eol_list_item_free(eolComponentListItem **item)
{
  if ((!item)||(!item))return;
  eol_component_free(&(*item)->item);
  free(*item);
  *item = NULL;
}

void eol_list_add_item(eolComponent *list,eolComponent *item)
{
  eolVec2D itemPos;
  eolComponentList * ldata;
  eolComponentListItem *listItem;
  ldata = eol_component_get_list_data(list);
  if ((!ldata) || (!item))return;/*no op*/
  listItem = eol_list_item_new();
  if (!listItem)return;/*couldn't add, BAD*/
    listItem->item = item;
  eol_rectf_set(&item->rect,0,0,1,1);
  
  itemPos = eol_component_list_get_item_position(list,ldata->itemCount);
  item->bounds.w = ldata->displayItems.x;
  item->bounds.h = ldata->displayItems.y;
  item->bounds.x = itemPos.x;
  item->bounds.y = itemPos.y;
  /*x position will be set at draw*/
  ldata->itemList = g_list_append(ldata->itemList,listItem);
  ldata->itemCount++;
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

void eol_component_list_select_item_n(
    eolComponent *component,
    eolUint       n
  )
{
  eolComponentListItem *item;
  eolComponentList * ldata;
  ldata = eol_component_get_list_data(component);
  if (!ldata)return;
  item = (eolComponentListItem *)g_list_nth(ldata->itemList,n);
  eol_component_list_select_item(
    component,
    item
  );
}

void eol_component_list_select_item(
    eolComponent *component,
    eolComponentListItem *item
  )
{
  eolComponentList * ldata;
  ldata = eol_component_get_list_data(component);
  if (!ldata)return;
  ldata->selection = g_list_find(ldata->itemList,item);
  if (!ldata->selection)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "failed to find item in list for selection!\n");
    return;
  }
  if (!ldata->multiSelection)
  {
    eol_component_list_deselect_all(component);
  }
  item->selected = eolTrue;
}

void eol_component_list_deselect_all(
    eolComponent *component
  )
{
  GList *it = NULL;
  eolComponentListItem *item;
  eolComponentList * ldata;
  ldata = eol_component_get_list_data(component);
  if (!ldata)return;
  for (it = ldata->itemList;it != NULL;it = it->next)
  {
    if (!it->data)continue;
    item = (eolComponentListItem *)it->data;
    item->selected = eolFalse;
  }
}

/*eol@eof*/
