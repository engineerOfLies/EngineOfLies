#include "eol_component_actor.h"
#include "eol_logger.h"
#include "eol_actor.h"
#include "eol_draw.h"
/*local function prototypes*/

void eol_component_actor_new(eolComponent *component);
void eol_component_actor_free(eolComponent *component);
void eol_component_actor_draw(eolComponent *component, eolRect bounds);
void eol_component_actor_move(eolComponent *component,eolRect newbounds);
eolBool eol_component_actor_update(eolComponent *component);


/*function definitions*/
eolComponentActor *eol_component_get_actor_data(eolComponent *component)
{
  if ((!component)||
    (!component->componentData)||
    (component->type != eolActorComponent))
  {
    return NULL;
  }
  return (eolComponentActor*)component->componentData;
}



void eol_component_make_actor(
    eolComponent  * component
  )
{
  eolComponentActor * actor = NULL;
  if (!component)return;
  eol_component_actor_new(component);
  actor = eol_component_get_actor_data(component);
  if (actor == NULL)
  {
    return;
  }
  eol_orientation_clear(&actor->ori);
  component->data_free = eol_component_actor_free;
  component->data_draw = eol_component_actor_draw;
  component->data_update = eol_component_actor_update;
  component->data_move = eol_component_actor_move;
}

eolComponent *eol_actor_component_new(
    eolUint       id,
    eolWord       name,
    eolRectFloat  rect,
    eolRect       bounds,
    eolLine       actorFile,
    eolLine       action,
    eolBool       rotating,
    eolBool       fading
)
{
  eolComponent *component = NULL;
  eolComponentActor *actor = NULL;
  component = eol_component_new();
  if (!component)return NULL;
  eol_rectf_copy(&component->rect,rect);

  eol_component_make_actor(
    component
  );
  
  actor = eol_component_get_actor_data(component);
  if (!actor)
  {
    eol_component_free(&component);
    return NULL;
  }
  actor->actor = eol_actor_load(actorFile);

  eol_component_actor_move(component,bounds);
  
  component->id = id;
  eol_line_cpy(component->name,name);

  return component;
}

void eol_component_actor_move(eolComponent *component,eolRect newbounds)
{
  eolVec3D actorSize;
  eolFloat actorLargest;
  eolFloat boundsSmallest;
  eolFloat scaleFactor;
  eolVec3D out3d;
  eolComponentActor *actor = eol_component_get_actor_data(component);
  if (actor == NULL)return;
  eol_component_get_rect_from_bounds(&component->bounds,newbounds,component->rect);
  if (actor->actor)
  {
    /*get 3D space coordinates for the component bounds*/
    eol_draw_get_3d_position(&out3d,eol_vec2d(component->bounds.x,component->bounds.y));
    
    actor->bounds3D.x = out3d.x;
    actor->bounds3D.y = out3d.y;
    
    eol_draw_get_3d_position(
      &out3d,
      eol_vec2d(
        component->bounds.x + component->bounds.w,
        component->bounds.y + component->bounds.h
      )
    );
    
    actor->bounds3D.w = fabs(out3d.x - actor->bounds3D.x);
    actor->bounds3D.h = fabs(out3d.y - actor->bounds3D.y);
    
    actorSize = eol_model_get_bounds(actor->actor->model);
    actorLargest = MAX(actorSize.x,actorSize.y);
    boundsSmallest = MIN(actor->bounds3D.w,actor->bounds3D.h);
    if (actorLargest != 0)
    {
      /*scale model to fit range.*/
      scaleFactor = boundsSmallest / actorLargest;

      eol_vec3d_set(
        actor->ori.scale,
        scaleFactor,scaleFactor,scaleFactor);
    }
  }
  /*set model potion to the center of the bounds in 3d space*/
  eol_draw_get_3d_position(
    &actor->ori.position,
    eol_vec2d(
      component->bounds.x + (component->bounds.w /2),
      component->bounds.y + (component->bounds.h /2)
    )
  );
}


void eol_component_actor_free(eolComponent *component)
{
  eolComponentActor *actor = eol_component_get_actor_data(component);
  if (actor == NULL)return;
  eol_actor_free(&actor->actor);
  free(actor);
  component->componentData = NULL;
}

eolBool eol_component_actor_update(eolComponent *component)
{
  eolComponentActor *actor = eol_component_get_actor_data(component);
  if (actor == NULL)return eolFalse;
  /*apply animation*/
  
  return eolFalse;
}

void eol_component_actor_draw(eolComponent *component, eolRect bounds)
{
  eolComponentActor *actor = eol_component_get_actor_data(component);
  if (!actor)return;
  eol_actor_draw(
    actor->actor,
    actor->ori.position,
    actor->ori.rotation,
    actor->ori.scale,
    actor->ori.color,
    actor->ori.alpha
  );
  
}

void eol_component_actor_new(eolComponent *component)
{
  if (component->componentData != NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_component_actor_new:tried to make an actor out of an existing component\n");
    return;
  }
  component->componentData = malloc(sizeof(eolComponentActor));
  if (component->componentData == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_component_actor_new: failed to allocate data for new actor component\n");
    return;
  }
  memset(component->componentData,0,sizeof(eolComponentActor));
  component->type = eolActorComponent;
}

eolComponent *eol_component_actor_create_from_config(eolKeychain *def, eolRect parentRect)
{
  eolUint       id;
  eolLine       name;
  eolRectFloat  rect = {0,0,1,1};
  
  eolLine         actorFile;
  eolLine         action;
  eolBool         rotating;
  eolBool         fading;
  
  if (!def)
  {
    eol_logger_message(EOL_LOG_WARN,"eol_component_actor_create_from_config: passed bad def parameter");
    return NULL;
  }

  eol_keychain_get_hash_value_as_line(name, def, "name");
  eol_keychain_get_hash_value_as_uint(&id, def, "id");
  eol_keychain_get_hash_value_as_rectfloat(&rect, def, "rect");

  eol_keychain_get_hash_value_as_line(actorFile, def, "actorFile");
  eol_keychain_get_hash_value_as_line(action, def, "action");
  eol_keychain_get_hash_value_as_bool(&rotating, def, "rotating");
  eol_keychain_get_hash_value_as_bool(&fading, def, "fading");
  
  return eol_actor_component_new(
    id,
    name,
    rect,
    parentRect,
    actorFile,
    action,
    rotating,
    fading
  );
}
/*eol@eof*/
