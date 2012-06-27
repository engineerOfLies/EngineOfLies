#include "eol_dialog.h"
#include "eol_logger.h"
#include "eol_graphics.h"
#include "eol_font.h"

/*local global variables*/
eolBool _eol_dialog_quitting = eolFalse;

void eol_dialog_yes_no_update(eolWindow *win,GList *updates)
{
  GList *c;
  eolWindowCallback call = NULL;
  eolComponent *comp = NULL;
  eolComponent *labelComp = NULL;
  if ((win == NULL)||(updates == NULL))return;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    labelComp = eol_window_get_component_by_id(win,0);
    switch (comp->id)
    {
      case 1:
        if (win->callbacks != NULL)
        {
          call = win->callbacks[0];
          if (call != NULL)call(win->customData);
        }
        eol_window_free(&win);
        break;
      case 2:
        if (win->callbacks != NULL)
        {
          call = win->callbacks[1];
          if (call != NULL)call(win->customData);
        }
        eol_window_free(&win);
        break;
    }
  }
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
      "eol_dialog:failed to make yes/no dialog.\n");
    return;
  }
  strncpy(win->name,"yesno",EOLLINELEN);
  win->id = 1;
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
                       question,eolJustifyCenter,3,NULL,eol_vec3d(1,1,1),1);
  eol_window_add_component(win,comp);
  comp = eol_button_stock_new(1,
                              "yes_button",
                              eol_rectf(0.25,0.65,1,1),
                              win->rect,
                              "YES",
                              SDLK_y,
                              eolTrue);
  eol_window_add_component(win,comp);
  comp = eol_button_stock_new(2,
                              "no_button",
                              eol_rectf(0.75,0.65,1,1),
                              win->rect,
                              "NO",
                              SDLK_y,
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

/*eol@eof*/
