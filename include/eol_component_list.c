#include "eol_component_list.h"
#include "eol_font.h"
#include "eol_logger.h"
#include "eol_draw.h"
#include "eol_mouse.h"

/*local function prototypes*/
void eol_component_list_free(eolComponent *component);
void eol_component_list_new(eolComponent *component);
eolComponentList *eol_component_get_list_data(eolComponent *component);
eolVec2D eol_component_list_get_total_area(eolComponentList *list);
eolVec2D eol_component_list_get_item_position(eolComponent *list,eolUint position);
eolVec2D eol_component_list_scaleable_area(eolComponentList *list);

void eol_component_make_list(
    eolComponent * component,
    eolUint   listType,
    eolBool   showHSlider,
    eolBool   showVSlider,
    eolVec2D  displayItems,
    eolUint   fontSize
  );
  
void eol_component_list_select_item(
    eolComponent *component,
    eolComponentListItem *item
  );
  
/*function definitions*/

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
    itemDimensions,
    fontSize
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
  list->itemBounds.x = component->bounds.x;
  list->itemBounds.y = component->bounds.y;
  list->itemBounds.w = component->bounds.w;
  list->itemBounds.h = component->bounds.h;

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

eolBool eol_list_item_bound_check(eolComponentList *list,eolRect item)
{
  eolRect b;
  b.x = list->itemBounds.x + list->displayItems.x;
  b.y = list->itemBounds.y + list->displayItems.y;
  b.w = list->itemBounds.w - (list->displayItems.x * 2);
  b.h = list->itemBounds.h - (list->displayItems.y * 2);
  return (eol_rect_lap_rect(b, item));
}

void eol_component_list_draw(eolComponent *component, eolRect bounds)
{
  eolRect r;
  eolVec2D itemPos = {0,0};
  eolVec2D scaleArea;
  int position = 0;
  GList *it = NULL;
  eolComponentListItem *item = NULL;
  eolComponentList *list = eol_component_get_list_data(component);
  if (list == NULL)return;
  r.x = component->bounds.x - 1;
  r.y = component->bounds.y - 1;
  r.w = component->bounds.w + 2;
  r.h = component->bounds.h + 2;
  eol_draw_solid_rect(component->bounds,eol_vec3d(0.1,0.1,0.1),1);
  /*draw list items*/
  scaleArea = eol_component_list_scaleable_area(list);
  /*TODO make the iterator start at the top position of the scroll*/
  for (it = list->itemList,position = 0;it != NULL;it = it->next,position++)
  {
    if (!it->data)continue;
    item = (eolComponentListItem*)it->data;
    itemPos = eol_component_list_get_item_position(component,position);
    item->item->bounds.x = itemPos.x;
    item->item->bounds.y = itemPos.y;
    if (!eol_list_item_bound_check(list,item->item->bounds))continue;
    if (item->selected)
    {
      eol_draw_solid_rect(item->item->bounds,list->highlightColor,0.9);
    }
    if (item->highlight)
    {
      eol_draw_rect(item->item->bounds,list->highlightColor,1);
    }
    eol_component_draw(item->item,item->item->bounds);
  }
  eol_draw_rect(r,eol_vec3d(1,1,1),1);
  if ((list->showVSlider) && (scaleArea.y > 1))
  {
    eol_component_draw(list->vSlider,list->vSliderBounds);
  }
  if ((list->showHSlider) && (scaleArea.x > 1))
  {
    eol_component_draw(list->hSlider,list->hSliderBounds);
  }
}

eolBool eol_component_list_update(eolComponent *component)
{
  GList *it = NULL;
  eolVec2D scaleArea;
  eolFloat slide = 0;
  eolComponentList *list;
  eolComponentListItem *item = NULL;
  eolBool updated = eolFalse;
  list = eol_component_get_list_data(component);
  if (list == NULL)return eolFalse;
  /*Update Sliders if visible*/
  scaleArea = eol_component_list_scaleable_area(list);
  if ((list->showVSlider) && (scaleArea.y > 1))
  {
    if (eol_component_update(list->vSlider))
    {
      updated = eolTrue;
      slide = eol_slider_get_position(list->vSlider);
      list->topOffset.y = -(scaleArea.y * slide);
    }
    slide = eol_slider_get_position(list->vSlider);
    list->topOffset.y = -(scaleArea.y * slide);
  }
  slide = 0;
  if ((list->showHSlider) && (scaleArea.x > 1))
  {
    if (eol_component_update(list->hSlider) || updated)
    {
      updated = eolTrue;
    }
    slide = eol_slider_get_position(list->hSlider);
    list->topOffset.x = -(scaleArea.x * slide);
  }
  /*iterate through glist and update elements*/
  for (it = list->itemList;it != NULL;it = it->next)
  {
    if (!it->data)continue;
    item = (eolComponentListItem*)it->data;
    if ((eol_mouse_in_rect(item->item->bounds)) &&
        (eol_list_item_bound_check(list,item->item->bounds)))
    {
      item->highlight = eolTrue;
      if (eol_mouse_input_released(eolMouseLeft))
      {
        eol_component_list_select_item(component,item);
        list->selection = it;
        updated = eolTrue;
      }
    }
    else item->highlight = eolFalse;
  }
  return updated;
}

void eol_component_make_list(
    eolComponent * component,
    eolUint   listType,
    eolBool   showHSlider,
    eolBool   showVSlider,
    eolVec2D  displayItems,
    eolUint   fontSize
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
  eol_vec3d_set(list->highlightColor,0,1,1);
  list->fontSize = fontSize;
  switch(listType)
  {
    default:
    case eolListText:
      displayItems.y = eol_font_get_text_height_average(list->fontSize);
      displayItems.x = component->bounds.w - (list->itemPadding.x * 2);
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
  ldata->selection = NULL;
  for (it = ldata->itemList;it != NULL;it = it->next)
  {
    if (!it->data)continue;
    item = (eolComponentListItem *)it->data;
    item->selected = eolFalse;
  }
}

eolBool eol_component_list_get_selected_item(eolComponent **itemOut,eolComponent *list)
{
  eolComponentListItem *item;
  eolComponentList * ldata;
  if (!itemOut)return eolFalse;
  ldata = eol_component_get_list_data(list);
  if (!ldata)return eolFalse;
  if (ldata->selection == NULL)return eolFalse;
  item = (eolComponentListItem *)ldata->selection->data;
  if (!item)return eolFalse;
  *itemOut = item->item;
  return eolTrue;
}

eolBool eol_component_list_get_selected_id(eolUint *id,eolComponent *list)
{
  eolComponent *item;
  if (!id)return eolFalse;
  if (!eol_component_list_get_selected_item(&item,list))return eolFalse;
  if (item == NULL)return eolFalse;
  *id = item->id;
  return eolTrue;
}

eolVec2D eol_component_list_scaleable_area(eolComponentList *list)
{
  eolVec2D listArea;
  eolVec2D scaleArea = {0,0};
  if (!list)return scaleArea;
  listArea = eol_component_list_get_total_area(list);
  scaleArea.x = listArea.x - list->itemBounds.w;
  scaleArea.y = listArea.y - list->itemBounds.h;
  return scaleArea;
}

eolVec2D eol_component_list_get_total_area(eolComponentList *list)
{
  eolVec2D area = {0,0};
  eolUint rows = 1,cols = 1;
  if (!list)return area;
  if (list->numPerRow)
  {
    rows = list->itemCount / list->numPerRow;
    cols = list->itemCount % list->numPerRow;
  }
  else
  {
    rows = list->itemCount;
  }
  area.x = (list->displayItems.x + list->itemPadding.x) * cols;
  area.y = (list->displayItems.y + list->itemPadding.y) * rows;
  return area;
}

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
  pos.x = (col * (ldata->displayItems.x + ldata->itemPadding.x)) + ldata->itemBounds.x + ldata->topOffset.x + ldata->itemPadding.x;
  pos.y = (row * (ldata->displayItems.y + ldata->itemPadding.y)) + ldata->itemBounds.y + ldata->topOffset.y + ldata->itemPadding.y;
  return pos;
}


/*eol@eof*/
