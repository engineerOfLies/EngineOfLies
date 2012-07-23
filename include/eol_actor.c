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

void eol_actor_draw_wire(
    eolActor *act,
    eolVec3D position,
    eolVec3D rotation,
    eolVec3D scale,
    eolVec3D color,
    eolFloat alpha
  )
{
  if (!act)return;
  eol_model_draw_wire(
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

eolActor * eol_actor_new()
{
  eolActor *actor = NULL;
  actor = (eolActor *)malloc(sizeof(eolActor));
  if (actor == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_actor:unable to allocate actor data!\n");
    return NULL;
  }
  memset(actor,0,sizeof(eolActor));
  return actor;
}

void eol_actor_free(eolActor **actor)
{
  if ((!actor) || (!*actor))return;
  eol_actor_clear(*actor);
  free(*actor);
  *actor = NULL;
}

void eol_actor_set_action(
    eolActor * act,
    eolWord    actionName
  )
{
  eolAction *action;
  if (!act)return;
  if (!act->model)return;
  if (strlen(actionName) <= 0)return;
  action = eol_model_get_action(act->model,actionName);
  if (action == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_actor: model %s does not have action %s\n",
      act->model->name,actionName);
    return;
  }
  act->action = action;
  strncpy(act->actionName,actionName,EOLWORDLEN);
  act->frame = action->begin;
  act->frameRate = action->frameRate;
  act->frameDirection = 1;
}

eolBool eol_actor_next_frame(eolActor * act)
{
  if (!act)return eolFalse;
  if (!act->model)return eolFalse;
  if (!act->action)return eolFalse;
  act->frame = act->frame + (act->frameRate * act->frameDirection);
  if (act->frame > act->action->end)
  {
    switch(act->action->type)
    {
      case eolActionLoop:
        act->frame = act->action->begin;
        return eolFalse;
      case eolActionPass:
        act->frame = act->action->end;
        return eolTrue;
      case eolActionOsci:
        act->frame = act->action->end;
        act->frameDirection = -1;
        return eolFalse;
    }
  }
  if (act->frame < act->action->begin)
  {
    switch(act->action->type)
    {
      case eolActionLoop:
        act->frame = act->action->begin;
        return eolFalse;
      case eolActionPass:
        act->frame = act->action->begin;
        return eolFalse;
      case eolActionOsci:
        act->frame = act->action->begin;
        act->frameDirection = 1;
        return eolTrue;
    }
  }
  return eolFalse;
}

eolFloat eol_actor_get_frame(eolActor *act)
{
  if (!act)return 0;
  return act->frame;
}
/*eol@eof*/

