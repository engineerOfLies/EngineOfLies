#include "eol_dialog.h"
#include "eol_logger.h"
#include "eol_graphics.h"
#include "eol_font.h"

typedef struct
{
  void *customData;
  char *output;
}eolEntryData;

/*local global variables*/
eolBool _eol_dialog_quitting = eolFalse;

eolBool eol_dialog_text_block_update(eolWindow *win,GList *updates)
{
  GList *c;
  eolWindowCallback call = NULL;
  eolComponent *comp = NULL;
  if ((win == NULL)||(updates == NULL))return eolFalse;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    switch (comp->id)
    {
      case 2:
        if (win->callbacks != NULL)
        {
          call = win->callbacks[0];
          if (call != NULL)call(win->customData);
        }
        eol_window_free(&win);
        return eolTrue;
    }
  }
  return eolFalse;
}

void eol_dialog_message(eolLine title,eolText message)
{
  eol_dialog_text_block(title,
                        message,
                        "OK",
                        NULL,
                        NULL);
}

void eol_dialog_text_block(eolLine title,
                           eolText text,
                           eolWord okText,
                           void    *data,
                           eolWindowCallback onOK)
{
  eolRect trect;  /*title dimensions*/
  eolRect brect;
  eolUint ww,wh;  /*window dimensions*/
  eolUint bw,bh;  /*button dimensions*/
  eolFloat bx,by;
  eolUint sw,sh;  /*screen dimensions*/
  eolWindow *win = eol_window_new();
  eolComponent *comp = NULL;
  if (!win)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_dialog:failed to make text block dialog.");
    return;
  }
  strncpy(win->name,"text_block",EOLLINELEN);

  eol_graphics_get_size(&sw, &sh);
  eol_button_get_stock_size(&bw, &bh);
  trect = eol_font_get_bounds(title,4);
  brect = eol_font_get_block_bounds(
    text,
    3,
    480,
    0
  );

  ww = MAX(trect.w + 10, brect.w + 8);
  wh = trect.h + brect.h + bh + 22;

  eol_rect_copy(&win->rect,eol_rect((sw/2) - (ww/2),(sh/2) - (wh/2),ww,wh));
  win->canHasFocus = eolTrue;
  win->drawGeneric = eolTrue;
  win->update = eol_dialog_text_block_update;

  by = eol_window_get_relative_position(2,win->rect.h);
  comp = eol_label_new(
    0,
    "text_title",
    eol_rectf(0.5,by,1,1),
    win->rect,
    eolTrue,
    title,
    eolJustifyCenter,
    eolFalse,
    4,
    NULL,
    eol_vec3d(1,1,1),
    1);
  eol_window_add_component(win,comp);

  
  bx = eol_window_get_relative_position(brect.w,win->rect.w);
  by = eol_window_get_relative_position(4 +trect.h,win->rect.h);
  comp = eol_label_new(
    1,
    "text_block",
    eol_rectf(0.5-(bx/2),by,1,1),
    win->rect,
    eolTrue,
    text,
    eolJustifyLeft,
    eolTrue,
    3,
    NULL,
    eol_vec3d(1,1,1),
    1);
  eol_window_add_component(win,comp);

  bx = eol_window_get_relative_position(win->rect.w - bw,win->rect.w);
  by = eol_window_get_relative_position(win->rect.h - bh,win->rect.h);
  comp = eol_button_stock_new(2,
                              "ok_button",
                              eol_rectf(bx,by,1,1),
                              win->rect,
                              okText,
                              -1,
                              SDLK_RETURN,
                              0,
                              eolFalse);
  eol_window_add_component(win,comp);

  win->customData = data;

  if (onOK != NULL)
  {
    eol_window_allocat_callbacks(win,1);
    if ((win->callbacks != NULL) && (win->callbackCount == 1))
    {
      win->callbacks[0] = onOK;
    }
  }
}

eolBool eol_dialog_yes_no_update(eolWindow *win,GList *updates)
{
  GList *c;
  eolWindowCallback call = NULL;
  eolComponent *comp = NULL;
  if ((win == NULL)||(updates == NULL))return eolFalse;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    switch (comp->id)
    {
      case 1:
        if (win->callbacks != NULL)
        {
          call = win->callbacks[0];
          if (call != NULL)call(win->customData);
        }
        eol_window_free(&win);
        return eolTrue;
      case 2:
        if (win->callbacks != NULL)
        {
          call = win->callbacks[1];
          if (call != NULL)call(win->customData);
        }
        eol_window_free(&win);
        return eolTrue;
    }
  }
  return eolFalse;
}

void eol_dialog_yes_no(eolLine question,
                       void * customData,
                       eolWindowCallback onYes,
                       eolWindowCallback onNo)
{
  eolRect trect;  /*text dimensions*/
  eolUint ww,wh;  /*window dimensions*/
  eolUint bw,bh;  /*button dimensions*/
  eolUint sw,sh;  /*screen dimensions*/
  eolWindow *win = eol_window_new();
  eolComponent *comp = NULL;
  if (!win)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_dialog:failed to make yes/no dialog.");
    return;
  }
  strncpy(win->name,"yesno",EOLLINELEN);
  eol_graphics_get_size(&sw, &sh);
  eol_button_get_stock_size(&bw, &bh);
  trect = eol_font_get_bounds(question,3);
  if (trect.w > ((bw*2) + 6))
  {
    ww = trect.w + 10;
  }
  else
  {
    ww = (bw*2) + 16;
  }
  wh = trect.h + bh + 16;
  eol_rect_copy(&win->rect,eol_rect((sw/2) - (ww/2),(sh/2) - (wh/2),ww,wh));
  win->canHasFocus = eolTrue;
  win->drawGeneric = eolTrue;
  win->update = eol_dialog_yes_no_update;
  comp = eol_label_new(0,"yes_title",eol_rectf(0.5,0.1,1,1),win->rect,eolTrue,
                       question,eolJustifyCenter,eolFalse,3,NULL,eol_vec3d(1,1,1),1);
  eol_window_add_component(win,comp);
  comp = eol_button_stock_new(1,
                              "yes_button",
                              eol_rectf(0.25,0.65,1,1),
                              win->rect,
                              "YES",
                              -1,
                              SDLK_y,
                              0,
                              eolTrue);
  eol_window_add_component(win,comp);
  comp = eol_button_stock_new(2,
                              "no_button",
                              eol_rectf(0.75,0.65,1,1),
                              win->rect,
                              "NO",
                              -1,
                              SDLK_n,
                              0,
                              eolTrue);
  eol_window_add_component(win,comp);
  win->customData = customData;
  eol_window_allocat_callbacks(win,2);
  if ((win->callbacks != NULL) && (win->callbackCount == 2))
  {
    win->callbacks[0] = onYes;
    win->callbacks[1] = onNo;
  }
}

void eol_dialog_quit_yes(void *data)
{
  exit(0);
}

void eol_dialog_quit_no(void *data)
{
  _eol_dialog_quitting = eolFalse;
}

void eol_dialog_quit()
{
  if (_eol_dialog_quitting)return;
  eol_dialog_yes_no("Exit the Program?",
                    NULL,
                    eol_dialog_quit_yes,
                    eol_dialog_quit_no);
  _eol_dialog_quitting = eolTrue;
}

/*text prompt*/
eolBool eol_dialog_text_prompt_update(eolWindow *win,GList *updates)
{
  GList *c;
  eolWindowCallback call = NULL;
  eolEntryData *entryData;
  eolComponent *comp = NULL;
  if ((win == NULL)||(updates == NULL))return eolFalse;
  entryData = (eolEntryData*)win->customData;
  if (!entryData)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_dialog_text_prompt_update:NULL entry data!"
    );
    return eolFalse;
  }
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    switch (comp->id)
    {
      case 1:
        eol_entry_get_line(
          eol_window_get_component_by_name(win,"text_entry"),
          entryData->output);
        if (win->callbacks != NULL)
        {
          call = win->callbacks[0];
          if (call != NULL)call(entryData->customData);
        }
        eol_window_free(&win);
        return eolTrue;
      case 2:
        if (win->callbacks != NULL)
        {
          call = win->callbacks[1];
          if (call != NULL)call(entryData->customData);
        }
        eol_window_free(&win);
        return eolTrue;
    }
  }
  return eolFalse;
}

void eol_dialog_text_prompt_delete(void *data)
{
  eolEntryData *entryData;
  if (!data)return;
  entryData = data;
  free(entryData);
}

eolWindow *eol_dialog_text_prompt(char *output,
                                  eolUint bufferSize,
                                  eolUint lineWidth,  /*0 means no limit*/
                                  eolLine question,
                                  void * customData,
                                  eolWindowCallback onOk,
                                  eolWindowCallback onCancel)
{
  eolRect trect;  /*text dimensions*/
  eolRect brect;  /*buffer dimentions*/
  eolUint ww,wh;  /*window dimensions*/
  eolUint bw,bh;  /*button dimensions*/
  eolUint sw,sh;  /*screen dimensions*/
  eolBool wordWrap = eolFalse;
  eolEntryData *entryData;
  eolWindow *win = eol_window_new();
  eolComponent *comp = NULL;
  if (!output)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_dialog_text_prompt:no out parameter provided.");
    return NULL;
  }
  if (!win)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_dialog_text_prompt:failed to make test prompt dialog.");
    return NULL;
  }

  win->customData = malloc(sizeof(eolEntryData));
  if (!win->customData)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_dialog_text_prompt:failed to allocate window data");
    eol_window_free(&win);
    return NULL;
  }
  memset(win->customData,0,sizeof(eolEntryData));
  entryData = (eolEntryData*)win->customData;
  entryData->customData = customData;
  entryData->output = output;
  
  wordWrap = eolFalse;
  
  strncpy(win->name,"gettext",EOLLINELEN);
  eol_graphics_get_size(&sw, &sh);
  eol_button_get_stock_size(&bw, &bh);
  trect = eol_font_get_bounds(question,3);
  brect = eol_font_get_bounds("W",3);
  brect.w *= bufferSize;
  if (MAX(trect.w,brect.w) > ((bw*2) + 6))
  {
    ww = MAX(trect.w,brect.w) + 10;
  }
  else
  {
    ww = (bw*2) + 16;
  }
  if (ww > sw - 20)ww = sw - 20;
  wh = trect.h + brect.h + bh + 30;
  eol_rect_copy(&win->rect,eol_rect((sw/2) - (ww/2),(sh/2) - (wh/2),ww,wh));
  win->canHasFocus = eolTrue;
  win->drawGeneric = eolTrue;
  win->update = eol_dialog_text_prompt_update;
  win->custom_delete = eol_dialog_text_prompt_delete;
  
  comp = eol_label_new(0,"text_prompt_title",eol_rectf(0.5,0.1,1,1),win->rect,eolTrue,
                       question,eolJustifyCenter,eolFalse,3,NULL,eol_vec3d(1,1,1),1);
  eol_window_add_component(win,comp);
  
  comp = eol_button_stock_new(1,
                              "ok_button",
                              eol_rectf(0.25,0.7,1,1),
                              win->rect,
                              "OK",
                              -1,
                              SDLK_RETURN,
                              0,
                              eolTrue);
  eol_window_add_component(win,comp);
  
  comp = eol_button_stock_new(2,
                              "cancel_button",
                              eol_rectf(0.75,0.7,1,1),
                              win->rect,
                              "CANCEL",
                              -1,
                              SDLK_ESCAPE,
                              0,
                              eolTrue);
  eol_window_add_component(win,comp);
  
  comp = eol_entry_new(3,
                       "text_entry",
                       eol_rectf(0.025,0.25,0.95,0.25),
                       win->rect,
                       output,
                       bufferSize,
                       eolJustifyLeft,
                       wordWrap,
                       3,
                       "",
                       eolFalse,
                       eol_vec3d(1,1,1),
                       1,
                       eol_vec3d(0.2,0.2,0.2));
  comp->hasFocus = eolTrue;
  comp->canHasFocus = eolTrue;
  eol_window_add_component(win,comp);

  eol_window_allocat_callbacks(win,2);
  if ((win->callbacks != NULL) && (win->callbackCount == 2))
  {
    win->callbacks[0] = onOk;
    win->callbacks[1] = onCancel;
  }
  return win;
}

/*eol@eof*/
