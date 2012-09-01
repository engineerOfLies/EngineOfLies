#include "eol_input.h"
#include "eol_logger.h"
#include "eol_config.h"
#include <SDL.h>

eolInput * _eol_input_list = NULL;
eolUint    _eol_input_count = 0;
eolBool    _eol_input_initialized = eolFalse;
eolUint    _eol_input_held_threshold = 0;
eolUint    _eol_input_double_tap_threshold = 0;
eolUint    _this_frame_keyboard_mod = 0;
eolUint    _last_frame_keyboard_mod = 0;
Uint8    * _this_frame_keyboard = NULL;
Uint8    * _last_frame_keyboard = NULL;
eolInt     _eol_input_keyboard_numkeys = 0;
/* local funtions */
void eol_input_keyboard_init();
void eol_input_keyboard_close();
void updateKeyboard();
void eol_input_close();

void eol_input_init()
{
  eol_logger_message(EOL_LOG_INFO,"eol_input: initializing\n");
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
  eol_input_load_config();
  eol_input_keyboard_init();
  atexit(eol_input_close);
  _eol_input_initialized = eolTrue;
  eol_logger_message(EOL_LOG_INFO,"eol_input: initialized\n");
}

void eol_input_close()
{
  eol_logger_message(EOL_LOG_INFO,"eol_input: closing\n");
  eol_input_keyboard_close();
  if (_eol_input_list != NULL)
  {
    free(_eol_input_list);
    _eol_input_list = NULL;
  }
  _eol_input_initialized = eolFalse;
  eol_logger_message(EOL_LOG_INFO,"eol_input: closed\n");
}

eolBool eol_input_is_initialized()
{
  return _eol_input_initialized;
}

void eol_input_update()
{
  int i;
  SDLMod mod;
  SDL_Event event;
  while(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_ALLEVENTS));
  updateKeyboard();
  SDL_PumpEvents();
  mod = SDL_GetModState();
  for(i = 0;i < _eol_input_count;i++)
  {
    _eol_input_list[i].oldvalue = _eol_input_list[i].value;
    if(strcmp(_eol_input_list[i].type,"key") == 0)
    {
      _eol_input_list[i].value = _this_frame_keyboard[_eol_input_list[i].id];
    }
    else if(strcmp(_eol_input_list[i].type,"mod") == 0)
    {
      if(mod & _eol_input_list[i].id)
      {
        _eol_input_list[i].value = 1;
      }
      else _eol_input_list[i].value = 0;
    }
    /*TODO:mouse section*/
    /*TODO:joystick section*/
    if(_eol_input_list[i].value == 1)
    {
      _eol_input_list[i].lastPress = _eol_input_list[i].timePress;
      _eol_input_list[i].timePress = SDL_GetTicks();
    }
  }
}

eolInt eol_input_parse(char *type, char *input)
{
  int i;
  if((type == NULL)||(input == NULL))return -1;
  if(strcmp(type,"key")==0)
  {
    if(input[0] == 'F')
    {
      sscanf(input,"F%i",&i);
      i--;
      return SDLK_F1 + i;
    }
    if((input[0] >= 'a')&&(input[0] <= 'z'))
    {
      return input[0]-'a' + SDLK_a;
    }
    if((input[0] >= '0')&&(input[0] <= '9'))
    {
      return input[0]-'0' + SDLK_0;
    }
    if(input[0] == '/')return SDLK_SLASH;
    if(input[0] == '\\')return SDLK_BACKSLASH;
    if(input[0] == '[')return SDLK_LEFTBRACKET;
    if(input[0] == ']')return SDLK_RIGHTBRACKET;
    if(input[0] == '-')return SDLK_MINUS;
    if(input[0] == '=')return SDLK_EQUALS;
    if(input[0] == '`')return SDLK_CARET;
    if(input[0] == ',')return SDLK_COMMA;
    if(input[0] == '.')return SDLK_PERIOD;
    if(strcmp(input,"TAB")==0)return SDLK_TAB;
    if(strcmp(input,"ESC")==0)return SDLK_ESCAPE;
    if(strcmp(input,"ENTER")==0)return SDLK_RETURN;
    if(strcmp(input,"SPACE")==0)return SDLK_SPACE;
    if(strcmp(input,"UPARROW")==0)return SDLK_UP;
    if(strcmp(input,"DOWNARROW")==0)return SDLK_DOWN;
    if(strcmp(input,"LEFTARROW")==0)return SDLK_LEFT;
    if(strcmp(input,"RIGHTARROW")==0)return SDLK_RIGHT;
    if(strcmp(input,"BACKSPACE")==0)return SDLK_BACKSPACE;
  }
  if(strcmp(type,"mod")==0)
  {
    if(strcmp(input,"LShift")==0)
    {
      return KMOD_LSHIFT;
    }
    if(strcmp(input,"RShift")==0)
    {
      return KMOD_RSHIFT;
    }
    if(strcmp(input,"LCtrl")==0)
    {
      return KMOD_LCTRL;
    }
    if(strcmp(input,"RCtrl")==0)
    {
      return KMOD_RCTRL;
    }
    if(strcmp(input,"LAlt")==0)
    {
      return KMOD_LALT;
    }
    if(strcmp(input,"RAlt")==0)
    {
      return KMOD_RALT;
    }
  }
  if(strcmp(type,"button")==0)
  {
    sscanf(input,"%i",&i);
    return i;
  }
  if(strcmp(type,"+axis")==0)
  {
    sscanf(input,"%i",&i);
    return i;
  }
  if(strcmp(type,"-axis")==0)
  {
    sscanf(input,"%i",&i);
    return i;
  }
  return -1;
}

void eol_input_add_input_from_link(eolKeychain *link,eolUint i)
{
  eolLine temp;
  if (i >= _eol_input_count)return;
  eol_keychain_get_hash_value_as_line(_eol_input_list[i].name,link,"name");
  eol_keychain_get_hash_value_as_line(temp,link,"type");
  eol_word_cpy(_eol_input_list[i].type,temp);
  eol_keychain_get_hash_value_as_line(temp,link,"id");
  _eol_input_list[i].id = eol_input_parse(_eol_input_list[i].type, temp);
}

void eol_input_load_config()
{
  int i;
  eolKeychain *chain;
  eolConfig *conf;
  eolUint count;
  conf = eol_config_load("system/controls.cfg");
  if (conf == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_input:failed to load control configuration\n");
    return;
  }
  if (!eol_config_get_keychain_by_tag(&chain,conf,"primary"))
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_input:no primary inputs in config\n");
    eol_config_free(&conf);
    return;
  }
  count = eol_keychain_get_list_count(chain);
  /*TODO: support secondary input methods*/
  _eol_input_count = count;
  _eol_input_list = (eolInput *)malloc(sizeof(eolInput) * count);
  if (_eol_input_list == NULL)
  {
    eol_logger_message(
      EOL_LOG_FATAL,
      "eol_input:failed to allocate input list\n");
    eol_config_free(&conf);
    return;
  }
  memset(_eol_input_list,0,sizeof(eolInput) * count);
  for (i = 0; i < count; i++)
  {
    eol_input_add_input_from_link(eol_keychain_get_list_nth(chain, i),i);
  }
  eol_config_free(&conf);
}

void eol_input_keyboard_close()
{
  if (_last_frame_keyboard != NULL)free(_last_frame_keyboard);
}

void eol_input_keyboard_init()
{
  _this_frame_keyboard = SDL_GetKeyState(&_eol_input_keyboard_numkeys);
  _last_frame_keyboard = (Uint8 *)malloc(sizeof(Uint8)*_eol_input_keyboard_numkeys);
  memcpy(
      _last_frame_keyboard,
      _this_frame_keyboard,
      sizeof(Uint8)*_eol_input_keyboard_numkeys
      );
      if(_last_frame_keyboard == NULL)
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_input: unable to initialize keyboard structure!\n");
  }
}

void updateKeyboard()
{
  int i;
  if((!_eol_input_initialized) ||
     (_last_frame_keyboard == NULL) ||
     (_this_frame_keyboard == NULL))return;
  for (i = 0; i < _eol_input_keyboard_numkeys;i++)
  {
    _last_frame_keyboard[i] = _this_frame_keyboard[i];
  }
  _last_frame_keyboard_mod = _this_frame_keyboard_mod;
  _this_frame_keyboard_mod = SDL_GetModState();
}

void eol_input_clear_keyboard()
{
  if((!_eol_input_initialized) ||
     (_last_frame_keyboard == NULL))return;
  memset(_last_frame_keyboard,0,sizeof(Uint8)*_eol_input_keyboard_numkeys);
  _last_frame_keyboard_mod = 0;
  _this_frame_keyboard_mod = 0;
}

eolBool eol_input_is_key_pressed(eolInt key)
{
  if((!_eol_input_initialized) ||
    (_last_frame_keyboard == NULL) ||
    (_this_frame_keyboard == NULL))
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_input: uninitialized.");
    return 0;
  }
  if ((key < 0) || (key >= _eol_input_keyboard_numkeys))
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_input: key out of range\n");
    return 0;
  }
  if ((_last_frame_keyboard[key] == 0) &&
      (_this_frame_keyboard[key] == 1))
    return eolTrue;
  return eolFalse;
}

eolBool eol_input_is_key_released(eolInt key)
{
  if((_eol_input_initialized == eolFalse) ||
    (_last_frame_keyboard == NULL) ||
    (_this_frame_keyboard == NULL))
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_input: uninitialized.\n");
    return 0;
  }
  if ((key < 0) || (key >= _eol_input_keyboard_numkeys))
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_input: key out of range\n");
    return 0;
  }
  if ((_last_frame_keyboard[key] == 1) &&
    (_this_frame_keyboard[key] == 0))
    return eolTrue;
  return eolFalse;
}

eolBool eol_input_is_key_held(eolInt key)
{
  if((_eol_input_initialized == eolFalse) ||
    (_last_frame_keyboard == NULL) ||
    (_this_frame_keyboard == NULL))
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_input: uninitialized.");
    return 0;
  }
  if ((key < 0) || (key >= _eol_input_keyboard_numkeys))
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_input: key out of range\n");
    return 0;
  }
  if ((_last_frame_keyboard[key] == 1) &&
    (_this_frame_keyboard[key] == 1))
  {
    return eolTrue;
  }
  return eolFalse;
}

eolBool eol_input_is_mod_held(eolUint mod)
{
  if((_eol_input_initialized == eolFalse) ||
    (_last_frame_keyboard == NULL) ||
    (_this_frame_keyboard == NULL))
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_input: uninitialized.");
    return 0;
  }
  if ((_this_frame_keyboard_mod & mod) &&
    (_last_frame_keyboard_mod & mod))
  {
    return eolTrue;
  }
  return eolFalse;
}

eolBool eol_input_is_mod_pressed(eolUint mod)
{
  if((_eol_input_initialized == eolFalse) ||
    (_last_frame_keyboard == NULL) ||
    (_this_frame_keyboard == NULL))
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_input: uninitialized.");
    return 0;
  }
  if ((_this_frame_keyboard_mod & mod) &&
    (!(_last_frame_keyboard_mod & mod)))
  {
    return eolTrue;
  }
  return eolFalse;
}

eolBool eol_input_is_mod_released(eolUint mod)
{
  if((_eol_input_initialized == eolFalse) ||
    (_last_frame_keyboard == NULL) ||
    (_this_frame_keyboard == NULL))
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_input: uninitialized.");
    return 0;
  }
  if ((_last_frame_keyboard_mod & mod) &&
    (!(_this_frame_keyboard_mod & mod)))
  {
    return eolTrue;
  }
  return eolFalse;
}

eolBool eol_input_quit_check()
{
  SDL_Event event;
  while(SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_QUITMASK))
  {
    if(event.type == SDL_QUIT)
    {
      return eolTrue;
    }
  }
  return eolFalse;
}

eolInput *eol_input_get_by_name(char *input)
{
  int i;
  for (i = 0;i < _eol_input_count;i++)
  {
    if (strncmp(input,_eol_input_list[i].name,EOLLINELEN) == 0)
    {
      return &_eol_input_list[i];
    }
  }
  eol_logger_message(
    EOL_LOG_WARN,
    "eol_input:failed to find input %s\n",
    input);
  return NULL;
}

eolFloat eol_input_is_input_down(char *inputName)
{
  eolInput *input = NULL;
  input = eol_input_get_by_name(inputName);
  if (input == NULL)return 0;
  if (input->value != 0)return input->value;
  return 0;
}

eolBool eol_input_is_input_pressed(char *inputName)
{
  eolInput *input = NULL;
  input = eol_input_get_by_name(inputName);
  if (input == NULL)return eolFalse;
  if ((input->value == 0) && (input->oldvalue != 0))return eolTrue;
  return eolFalse;
}

eolBool eol_input_is_input_released(char *inputName)
{
  eolInput *input = NULL;
  input = eol_input_get_by_name(inputName);
  if (input == NULL)return eolFalse;
  if ((input->value != 0) && (input->oldvalue == 0))return eolTrue;
  return eolFalse;
}

eolFloat eol_input_is_input_held(char *inputName)
{
  eolInput *input = NULL;
  input = eol_input_get_by_name(inputName);
  if (input == NULL)return 0;
  if ((input->value != 0) && (input->oldvalue != 0))return input->value;
  return 0;
}

/*eol@eof*/

