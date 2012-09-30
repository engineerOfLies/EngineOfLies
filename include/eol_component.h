#ifndef __EOL_COMPONENTS__
#define __EOL_COMPONENTS__
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

#include "eol_types.h"

/**
 * @purpose This will create the components used for window construction.
    Similar in concept to a gtkWidget, but severely limited to the components
    needed in a game
 */

enum eolComponentTypes {
  eolNullComponent   = 0,  /**<this type is set by default*/
  eolLabelComponent  = 1,  /**<For text string labels*/
  eolButtonComponent = 2,  /**<for buttons*/
  eolEntryComponent  = 3,  /**<for text input entries*/
  eolSliderComponent = 4,  /**<for slider controls and scroll bars*/
  eolImageComponent  = 5,  /**<for static images and icons*/
  eolActorComponent  = 6,  /**<for animated images and 3D models*/
  eolListComponent   = 7   /**<for lists of stuff*/
};

enum eolSliderTypes {
  eolSliderCommon  = 0,  /**<stock slider image, drawn bar*/
  eolSliderDraw    = 1,  /**<drawn slider, drawn bar*/
  eolSliderStock   = 2,  /**<stock slider, stock bar*/
  eolSliderCustom  = 3   /**<custom images for slider and bar*/
};

enum eolSliderStates {
  eolSliderIdle   = 0,  /**<Not touched*/
  eolSliderHigh   = 1,  /**<Mouse is over it, or the slider has focus*/
  eolSliderHeld   = 2   /**<Mouse is interacting with it*/
};

enum eolButtonTypes {
  eolButtonStock  = 0,  /**<use the common stock button graphics*/
  eolButtonText   = 1,  /**<doesnt use button graphics, but highlights text*/
  eolButtonHidden = 2,  /**<does not draw the button, but it can still get hotkey input*/
  eolButtonCustom = 3   /**<same as stock, but uses custom graphics*/
};

enum eolButtonStates {
  eolButtonIdle       = 0,  /**<no activity*/
  eolButtonHighlight  = 1,  /**<mouse is over the button or the button has focus*/
  eolButtonPressed    = 2   /**<mouse or hotkey is pressed on the button*/
};
#define eolButtonStateMax 4

enum eolListTypes {
  eolListText   = 0, /**<items are text and drawn from top to bottom*/
  eolListLines  = 1, /**<items are drawn from top to bottom fit within bounding
                         rect*/
  eolListBlock  = 2, /**<items are drawn left to right, then top to bottom fit
                         within bounding rect*/
  eolListDock   = 3  /**<items are drawn left to right.  fit within bounding rect*/
};

/**
 * @note the relative float rect for all components follows this convention:
 * x or y should be from 0-1, 0 being at the far left/top, 1 meaning far right/bottom.
 * if x or y is negative it is taken in relative postiton from the right/bottom.
 * w and h are widths relative to the size of the bounding canvas.  So 0.5 means the
 * width should take up half of the space.
 * 0 or negative w or h means SNAP to edge.  Just like a 0 would mean for x or y.
 * @example rect {0.25,-0.25,-1,0.2} means that the top left corner will be at
 * (0.25,0.75) and the bottom right corner will be at (1,0.95)
*/

/**
  @brief this structure serves as a header for all components
 */
typedef struct eolComponent_S
{
  eolUint       id;
  eolWord       name;
  eolRectFloat  rect;           /**<relative position to draw*/
  eolRect       bounds;         /**<calculated position to draw*/
  eolBool       canHasFocus;    /**<I apologize for the lolcat reference*/
  eolBool       hasFocus;
  eolInt        state;
  eolInt        oldState;
  eolUint       type;
  void        * componentData;
  void          (*data_free)(struct eolComponent_S *component);
  void          (*data_draw)(struct eolComponent_S *component,eolRect bounds);
  eolBool       (*data_update)(struct eolComponent_S *component);
  eolInt        (*data_get_state)(struct eolComponent_S *component);
  eolBool       (*data_changed)(struct eolComponent_S *component);
}eolComponent;

/**
 * @brief loads default component assets.
 */
void eol_component_config();

/*Generic Component functions*/
/**
 * @brief checks if a component has changed since last frame
 *
 * @param component the component to check
 * @return eolTrue if the component has changed, eolFalse otherwise
 */
eolBool eol_component_changed(eolComponent *component);

/**
 * @brief creates a new blank component and returns a pointer to it
 * @return a pointer to an empty, formless component or NULL on error
 */
eolComponent * eol_component_new();

/**
 * @brief frees a previously allocated component
 * @param component a pointer to a component pointer.  Set to NULL on success
 */
void eol_component_free(eolComponent **component);

/**
 * @brief runs the appropriate update function on the component.
 * This is called automatically interally to EOL.  It should not have to be called directly.
 * @param component the component to be updated
 * @return eolTrue if the component has update information or eolFalse otherwise
 */
eolBool eol_component_update(eolComponent *component);

/**
 * @brief gives the component focus and updates what that means for the component
 * @param component which component to add focus to
 * @param focus the focus state to be set.  eolTrue to have focus or eolFalse if not.
 */
void eol_component_set_focus(eolComponent *component,eolBool focus);

/**
 * @brief checks the component update status for this engine frame
 * @param component which component to check
 * @return eolTrue if it was updated, eolFalse otherwise
 */
eolBool eol_component_has_changed(eolComponent *component);

/**
 * @brief retrieves the component's state information
 * state means different things to different components
 * @param component which component to check
 * @return the state of the component
 */
eolInt eol_component_get_state(eolComponent *component);

/**
 * @brief draws the component to the screen.
 * @param component the component to draw.  Some components may not be drawn by design
 * @param bounds the window bounds that the component belongs to.
 */
void eol_component_draw(eolComponent *component,eolRect bounds);

/* Slider Component */
/**
 * @brief created a new slider component.
 * @param id the id for this component. This should be unique per window.
 * @param name the name of this component.  This should be unique per window.
 * @param rect the component position relative to the window bounds
 * @param bounds the window rect.
 * @param vertical if eolTrue the slider will be set up as a vertical slider, horizontal otherwise
 * @param slider the slider graphic to use
 * @param sliderHigh the highlighted slider graphic to use
 * @param bar the bar graphic to use
 * @param cap1 the bar end cap to use (top or left)
 * @param cap2 the bar end cap to use (bottom or right)
 * @param barColor the color to draw the bar
 * @param sliderColor the color to draw the slider
 * @param startPosition the starting position of the slider
 * @param sliderType the type of slider (see enum above).
 * @return a pointer to newly allocated and set up slider or NULL on error.
 */
eolComponent *eol_slider_new(
    eolUint      id,
    eolWord      name,
    eolRectFloat rect,
    eolRect      bounds,
    eolBool      vertical,
    eolLine      slider,
    eolLine      sliderHigh,
    eolLine      bar,
    eolLine      cap1,
    eolLine      cap2,
    eolVec3D     barColor,
    eolVec3D     sliderColor,
    eolFloat     startPosition,
    eolUint      sliderType
);

/**
 * @brief created a new slider component using common settings
 * @param id the id for this component. This should be unique per window.
 * @param name the name of this component.  This should be unique per window.
 * @param rect the component position relative to the window bounds
 * @param bounds the window rect.
 * @param vertical if eolTrue the slider will be set up as a vertical slider, horizontal otherwise
 * @param barColor the color to draw the bar
 * @param startPosition the starting position of the slider
 * @return a pointer to newly allocated and set up slider or NULL on error.
 */
eolComponent *eol_slider_common_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    eolBool        vertical,
    eolVec3D       barColor,
    eolFloat       startPosition
);

/**
 * @brief gets the current position of a slider component
 * does component type checking
 * @param the component to check
 * @return the slider position (0-1).  Set to 0 on error.
 */
eolFloat eol_slider_get_position(eolComponent *comp);

/**
 * @brief sets the position for a slider component
 * no op if not a slider
 * @param comp the component to set
 * @param newPos the new position to set the slider to
 */
void eol_slider_set_position(eolComponent *comp, eolFloat newPos);

/* Label Functions */
/**
 * @brief creates a new label component
 * @param id the id for this component. This should be unique per window.
 * @param name the name of this component.  This should be unique per window.
 * @param rect the component position relative to the window bounds
 * @param bounds the window rect.
 * @param canHasFocus not all labels can have focus, nor should it make sense
 * @param text the text to display in the label
 * @param justify how to align the text (see eol_font justify enumeration)
 * @param wordWrap if this label should word wrap or not
 * @param fontSize stock font size
 * @param fontName if not NULL, it will try to load the specified font for drawing.
 * @param color the text draw color (1,1,1 is white)
 * @param alpha the text draw transparency (1 is opaque)
 * @return a newly setup label component or NULL on error
 */
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
  );

/**
 * @brief changes a label's text
 * @param comp the label to change
 * @param text the text to change it to
 */
void eol_label_set_text(eolComponent *comp,char *text);

/* Button Functions*/
/**
 * @brief creates a new button component
 * @param id the id for this component. This should be unique per window.
 * @param name the name of this component.  This should be unique per window.
 * @param rect the component position relative to the window bounds
 * @param bounds the window rect.
 * @param buttonText the text to write on the button.  Can be NULL
 * @param buttonType see button type enum above.
 * @param buttonHotkey the SDLKey hotkey that this button listens to.  0 or less is none
 * @param buttonHotkeymod the SDKMod that this button listens to.  0 or less is none
 * @param center if the button should be centered on the rect or aligned to it.
 * @param buttonFileUp if specified it will attempt to load this file as the button idle image
 * @param buttonFileHigh if specified it will attempt to load this file as the button highligh image
 * @param buttonFileDown if specified it will attempt to load this file as the button pressed image
 */
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
  );

/**
 * @brief creates a new button component with common options
 * @param id the id for this component. This should be unique per window.
 * @param name the name of this component.  This should be unique per window.
 * @param rect the component position relative to the window bounds
 * @param bounds the window rect.
 * @param buttonText the text to write on the button.  Can be NULL
 * @param buttonType see button type enum above.
 * @param buttonHotkey the SDLKey hotkey that this button listens to.  0 or less is none
 * @param buttonHotkeymod the SDKMod that this button listens to.  0 or less is none
 * @param center if the button should be centered on the rect or aligned to it.
 */
eolComponent *eol_button_stock_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    char         * buttonText,
    eolInt         buttonHotkey,
    eolUint        buttonHotkeymod,
    eolBool        center
  );

/**
 * @brief creates a new button component setup for use as a text button
 * @param id the id for this component. This should be unique per window.
 * @param name the name of this component.  This should be unique per window.
 * @param rect the component position relative to the window bounds
 * @param bounds the window rect.
 * @param buttonText the text to write on the button.  Can be NULL
 * @param buttonType see button type enum above.
 * @param buttonHotkey the SDLKey hotkey that this button listens to.  0 or less is none
 * @param buttonHotkeymod the SDKMod that this button listens to.  0 or less is none
 * @param center if the button should be centered on the rect or aligned to it.
 */
eolComponent *eol_button_text_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    char         * buttonText,
    eolInt         buttonHotkey,
    eolUint        buttonHotkeymod,
    eolBool        center
  );

/**
 * @brief gets the dimensions for a stock button
 * @param w output.  if set, it will return the width of the stock button in pixels
 * @param h output.  if set, it will return the height of the stock button in pixels
 */
void eol_button_get_stock_size(eolUint *w, eolUint *h);

/*Entry Functions*/
/**
 * @brief creates a new entry component
 * @param id the id for this component. This should be unique per window.
 * @param name the name of this component.  This should be unique per window.
 * @param rect the component position relative to the window bounds
 * @param bounds the window rect.
 * @param output a pointer to the buffer where the output will be stored
 * @param outputLimit the length of the output buffer
 * @param justify how to draw the text in the entry (see eol_font justify enum)
 * @param wordWrap if the text should wrap around or be a single line
 * @param fontSize stock font to use to draw the test.
 * @param fontName if set it will attempt to use this font instead of stock
 * @param number if true, it treats it as a number input and follows that logic
 * @param color text color
 * @param alpha text transparency
 * @param bgcolor background color.  Should be in contrast to color.
  */
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
);

/**
 * @brief creates a new entry component set up with common options for single line input
 * @param id the id for this component. This should be unique per window.
 * @param name the name of this component.  This should be unique per window.
 * @param rect the component position relative to the window bounds
 * @param bounds the window rect.
 * @param output the eolLine where the output will be written.
  */
eolComponent *eol_line_entry_new(
    eolUint       id,
    eolWord       name,
    eolRectFloat  rect,
    eolRect       bounds,
    eolLine       output
);

/**
 * @brief creates a new entry component set up with common options for single line input
 * @param id the id for this component. This should be unique per window.
 * @param name the name of this component.  This should be unique per window.
 * @param rect the component position relative to the window bounds
 * @param bounds the window rect.
 */
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
  );

/**
 * @brief adds a text item to the list.
 * @param list the list to add an item to
 * @param itemId the id of the item to add
 * @param text the text to display for the item
 */
void eol_list_add_text_item(
    eolComponent  * list,
    eolUint         itemId,
    eolLine         text
  );

/**
 * @brief deselects any items in the list that may be selected
 * @param component the list component to deselect from
 */
void eol_component_list_deselect_all(
    eolComponent *component
  );

/**
 * @brief Selects the item at index n in the list.  If the list does not allow multiple
 * selections, then all other items are deselected.
 * @param component the list component
 * @param n the list item to select
 */
void eol_component_list_select_item_n(
    eolComponent *component,
    eolUint       n
  );

/**
 * @brief Get the id of the selected item if there is one.
 * @param id output parameter, the id of the selected item is returned through this.
 * @param list the list to get the selection from
 * @return eolTrue if an item was selected and its ID retrieved.  eolFalse otherwise.
 */
eolBool eol_component_list_get_selected_id(eolUint *id,eolComponent *list);

/**
 * @brief Get the item component of the selected item if there is one.
 * @param itemOut output parameter, a pointer to the selected item is returned through this.
 * @param list the list to get the selection from
 * @return eolTrue if an item was selected and it was retrieved.  eolFalse otherwise.
 */
eolBool eol_component_list_get_selected_item(eolComponent **itemOut,eolComponent *list);


/**
 * @brief changes what is in the output buffer for the entry.
 * @param component the component to set.
 */
void eol_entry_assign_output(eolComponent *component);

/**
 * @brief utility to calculate draw rect from bounds and canvas rect
 */
void eol_component_get_rect_from_bounds(eolRect *rect,eolRect canvas, eolRectFloat bounds);

#endif
