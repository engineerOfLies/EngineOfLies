#include "eol_message.h"
#include "eol_window.h"
#include "eol_config.h"
#include "eol_font.h"
#include "eol_logger.h"
#include "eol_graphics.h"

/* local types */
typedef struct
{
  eolLine     text;
  eolUint     fontSize;
  eolFont   * customFont;
  eolVec3D    color;
  eolFloat    alpha;
  eolUint     deathTime;
}eolMessage;

typedef struct
{
  eolUint       fontSize;
  eolVec3D      color;
  eolFloat      alpha;
  eolUint       timeout;
  eolUint       messageLimit;
  eolUint       padding;  /*between lines in pixels*/
  eolBool       ascending;
  eolRectFloat  rect;     /*area of the window to use for drawing*/
  eolRect       bounds;   /*area of the window to use for drawing in pixel space*/
  eolFont * font;
  GList *messageList;
  void (*draw)(eolWindow *self);  /**<callback for drawing*/
  eolBool (*update)(eolWindow *self,GList *updates); /**<callback for updates*/
}eolMessageData;

/*local function prototypes*/

void eol_message_close();
eolMessageData *eol_message_get_data();
eolMessage *eol_message_create();

void eol_message_window_draw(eolWindow *self);
eolBool eol_message_window_update(eolWindow *self,GList *updates);

/*local variable instances*/
eolWindow * _eol_message_window = NULL;
eolBool     _eol_message_initialized = eolFalse;

/*function definitions*/

void eol_message_setup()
{
  eolRectFloat rect = {0,0,1,1};
  eolMessageData *messageData = NULL;
  eolConfig *conf = NULL;
  eolLine windowDefPath = "ui/eol_message.def";
  eolLine font = "";

  eol_logger_message(
    EOL_LOG_INFO,
    "eol_message:initializing\n");

  messageData = (eolMessageData *)malloc(sizeof(eolMessageData));
  if (messageData == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_message:failed to allocate message data\n");
    return;
  }
  memset(messageData,0,sizeof(eolMessageData));
  
  conf = eol_config_load("system/message.cfg");
  if (conf != NULL)
  {
    eol_config_get_line_by_tag(windowDefPath,conf,"windowDefPath");
    eol_config_get_uint_by_tag(&messageData->fontSize,conf,"fontSize");
    eol_config_get_vec3d_by_tag(&messageData->color,conf,"color");
    eol_config_get_float_by_tag(&messageData->alpha,conf,"alpha");
    eol_config_get_uint_by_tag(&messageData->timeout,conf,"timeout");
    eol_config_get_uint_by_tag(&messageData->messageLimit,conf,"messageLimit");
    eol_config_get_bool_by_tag(&messageData->ascending,conf,"ascending");
    eol_config_get_rectfloat_by_tag(&messageData->rect,conf,"rect");
    eol_config_get_uint_by_tag(&messageData->padding,conf,"padding");
    if (eol_config_get_line_by_tag(font,conf,"font"))
    {
      messageData->font = eol_font_load(font,messageData->fontSize);
    }
  }
  _eol_message_window = eol_window_load_from_file(windowDefPath);
  if (_eol_message_window == NULL)
  {
    free(messageData);
    return;
  }
  _eol_message_window->customData = (void *)messageData;
  _eol_message_window->draw = eol_message_window_draw;
  _eol_message_window->update = eol_message_window_update;
  
  eol_component_get_rect_from_bounds(&messageData->bounds,_eol_message_window->rect, rect);
  
  _eol_message_initialized = eolTrue;
  atexit(eol_message_close);
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_message:initialized\n");
}

void eol_message_close()
{
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_message:closing\n");
  eol_window_free(&_eol_message_window);
  _eol_message_initialized = eolFalse;
  
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_message:closed\n");
}

void eol_message_data_delete(void *genericdata)
{
  eolMessageData *data;
  if (!genericdata)return;
  data = (eolMessageData*)genericdata;
  eol_message_clear();
  eol_font_free(&data->font);
  free(data);
}

eolMessageData *eol_message_get_data()
{
  if (!_eol_message_window)return NULL;
  return (eolMessageData*)_eol_message_window->customData;
}

eolMessage *eol_message_create()
{
  eolMessage * mess;
  mess = (eolMessage *)malloc(sizeof(eolMessage));
  if (!mess)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_message:failed to allocate a new message\n");
    return NULL;
  }
  memset(mess,0,sizeof(eolMessage));
  return mess;
}

void eol_message_delete(eolMessage *mess)
{
  if (!mess)return;
  eol_font_free(&mess->customFont);
  free(mess);
}

void eol_message_free(eolMessage **mess)
{
  if ((!mess) || (!*mess))return;
  eol_message_delete(*mess);
  *mess = NULL;
}

void eol_message_remove_oldest()
{
  GList *oldest;
  eolMessageData *data = eol_message_get_data();
  if (!data)return;
  if (data->ascending == eolTrue)
  {
    oldest = g_list_first(data->messageList);
  }
  else
  {
    oldest = g_list_last(data->messageList);
  }
  if (!oldest)return;
  eol_message_delete((eolMessage *)oldest->data);
  data->messageList = g_list_delete_link(data->messageList,oldest);
}

void eol_message_clear()
{
  GList *it;
  eolMessageData *data = eol_message_get_data();
  if (!data)return;
  for (it = data->messageList;it != NULL;it = it->next)
  {
    eol_message_delete((eolMessage *)it->data);
  }
  g_list_free(data->messageList);
  data->messageList = NULL;
}

void eol_message_new(eolLine message)
{
  eolMessageData *data = eol_message_get_data();

  if (!data)return;
  eol_message_new_custom(message,"", data->fontSize,data->color, data->alpha);
}

void eol_message_new_custom(eolLine message,eolLine font, eolUint fontSize,eolVec3D color, eolFloat alpha)
{
  eolFont *customFont;
  eolMessage *newMess = NULL;
  eolMessageData *data = eol_message_get_data();
  if (!data)return;
  newMess = eol_message_create();
  if (!newMess)return;

  eol_line_cpy(newMess->text,message);

  if (strlen(font) > 1)
  {
    customFont = eol_font_load(font,fontSize);
    newMess->customFont = customFont;
  }
  
  newMess->fontSize = fontSize;
  eol_vec3d_copy(newMess->color,color);
  newMess->alpha = alpha;

  if (data->timeout)
  {
    newMess->deathTime = eol_graphics_get_now() + data->timeout;
  }
  
  if (data->ascending == eolTrue)
  {
    data->messageList = g_list_append(data->messageList, newMess);
  }
  else
  {
    data->messageList = g_list_prepend(data->messageList, newMess);
  }
  if (g_list_length(data->messageList) >= data->messageLimit)
  {
    eol_message_remove_oldest();
  }
}

/*window functions*/

eolUint eol_message_draw(eolMessage *message,eolVec2D position)
{
  eolRect bounds;
  eolMessageData *data = eol_message_get_data();
  if (!data)return 0;
  if (message->customFont)
  {
    bounds = eol_font_get_bounds_custom(
      message->text,
      message->customFont
    );
    eol_font_draw_text_custom(
      message->text,
      position.x,
      position.y,
      message->color,
      message->alpha,
      message->customFont
    );
  }
  else
  {
    bounds = eol_font_get_bounds(
      message->text,
      message->fontSize
    );
    eol_font_draw_text(
      message->text,
      position.x,
      position.y,
      message->color,
      message->alpha,
      message->fontSize
    );
  }

  return bounds.h + data->padding;
}

void eol_message_timeout()
{
  GList *it;
  eolUint now;
  eolMessage* mess;
  eolMessageData *data = eol_message_get_data();
  if (!data)return;
  if (!data->timeout)return;
  now = eol_graphics_get_now();
  for (it = data->messageList;it != NULL;it = it->next)
  {
    if (!it->data)continue;
    mess = (eolMessage *)it->data;
    if ((mess->deathTime != 0) && (mess->deathTime < now))
    {
      eol_message_free(&mess);
      data->messageList = g_list_delete_link(data->messageList,it);
    }
  }
}

void eol_message_window_draw(eolWindow *self)
{
  GList *it;
  eolUint position = 0;
  eolVec2D draw = {0,0};
  eolMessageData *data = eol_message_get_data();
  if (!data)return;
  draw.x = data->bounds.x;
  draw.y = data->bounds.y;
  for (it = data->messageList,position = 0;it != NULL; it = it->next,position++)
  {
    if (!it->data)continue;
    draw.y += eol_message_draw((eolMessage *)it->data,draw);
  }
  /*draw items in the list*/
  if (data->draw)data->draw(self);
}

eolBool eol_message_window_update(eolWindow *self,GList *updates)
{
  eolMessageData *data = eol_message_get_data();
  if (!data)return eolFalse;
  /*purge old items in the list*/
  eol_message_timeout();
  if (data->update)
  {
    return data->update(self,updates);
  }
  return eolFalse;
}

/*eol@eof*/