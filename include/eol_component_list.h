#ifndef __EOL_COMPONENT_LIST__
#define __EOL_COMPONENT_LIST__
/*
    Copyright 2012 Engineer of Lies
    This file is part of the Engine of Lies game engine library

    The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EOL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the EOL game engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "eol_component.h"
#include "eol_types.h"
#include <glib/glist.h>

typedef struct
{
  eolRectFloat  placement;
  eolBool       selected;
  eolComponent *item;
}eolComponentListItem;

typedef struct
{
  eolUint   listType;   /**<see list type enumaration in header*/
  eolUint   itemCount;  /**<number of items in the list*/
  eolRectFloat itemBounds; /**<sub area for drawing items*/
  eolVec2D  itemPadding;
  eolVec2D  displayItems;/**<dimensions for drawing items*/
  eolUint   numPerRow;  /**<how many items appear per row*/
  GList   * itemList;   /**<list of eolComponent's*/
  GList   * topItem;    /**<the top left most item*/
  GList   * selection;  /**<the list item that is selected*/
  eolBool   showHSlider;/**<if true, shows the Horizontal slider*/
  eolBool   showVSlider;/**<if true, shows the Vertical slider*/
  eolRect   vSliderBounds;
  eolRect   hSliderBounds;
  eolComponent *hSlider;/**<the horizontal slider*/
  eolComponent *vSlider;/**<the vertical slider*/
  eolUint    fontSize;
  eolVec3D   textColor;
  eolFloat   alpha;
}eolComponentList;

void eol_component_list_free(eolComponent *component);
void eol_component_list_new(eolComponent *component);


#endif
