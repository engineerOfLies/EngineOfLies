#include "eol_actor.h"
#include "eol_logger.h"

/*local global variables*/

eolBool              _eol_actor_initialized = eolFalse;
eolUint              _eol_actor_max = 0;
eolResourceManager * _eol_actor_manager = NULL;

/*local function prototypes*/
void eol_actor_delete(void **act);

void eol_actor_init()
{
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_actor:initializing");
_eol_actor_manager = eol_resource_manager_init(
    "eol_actor_manager",
    _eol_actor_max,
    sizeof(eolActor),
    eol_actor_delete
    );

  eol_logger_message(
      EOL_LOG_INFO,
      "eol_actor:initialized");
  atexit(eol_actor_close);
  _eol_actor_initialized = eolTrue;
}

void eol_actor_config()
{
  _eol_actor_max = 2048;
}

void eol_actor_delete(void **act)
{

}

void eol_actor_close()
{
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_actor:closing");
  eol_resource_manager_close(_eol_actor_manager);
  _eol_actor_max = 0;
  _eol_actor_initialized = eolFalse;
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_actor:closed");

}

eolBool eol_actor_initialized();

/*eol@eof*/

