#include "eol_actor.h"
#include "eol_logger.h"

/*local global variables*/


/*local function prototypes*/


void eol_actor_load(
    eolActor *act,
    char     *filename
  )
{
  if (!act)return;
  act->model = eol_model_load(filename);
}


void eol_actor_clear(eolActor *act)
{
  if (!act)return;
  if (act->model != NULL)
  {
    eol_model_free(&act->model);
  }
  memset(act,0,sizeof(eolActor));
}

void eol_actor_draw(
    eolActor *act,
    eolVec3D position,
    eolVec3D rotation,
    eolVec3D scale,
    eolVec3D color,
    eolFloat alpha
  )
{
  if (!act)return;
  eol_model_draw(
    act->model,
    position,
    rotation,
    scale,
    color,
    alpha,
    act->frame
  );
}

void eol_actor_set_frame_rate(
    eolActor *act,
    eolFloat rate
  )
{
  if (!act)return;
  if (rate == 0)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_actor:passed a 0 frame rate.\n"
    );
  }
  act->frameRate = rate;
}

void eol_actor_set_action(
    eolActor * act,
    eolWord    aname
  )
{
  eolAction *action;
  if (!act)return;
  action = eol_model_get_action(
      act->model,
      aname
  );
  if (!action)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_actor:cannot set action for actor %s\n",
      act->name);
    return;
  }
  
}

/*eol@eof*/

