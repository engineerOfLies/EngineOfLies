#include "eol_config.h"
#include "eol_logger.h"
#include <glib.h>
#include <yaml.h>

/*local globals*/
eolBool _eol_config_initialized;


/*local functions*/
void eol_config_deinit(void);
void eol_config_parse_tier(yaml_parser_t *parser, eolKeychain *cfg);

/*function definitions*/

void eol_config_init()
{
  eol_logger_message( EOL_LOG_INFO, "eol_config: initializing\n");
  _eol_config_initialized = eolTrue;
  atexit(eol_config_deinit);
  eol_logger_load_config();
  eol_logger_message( EOL_LOG_INFO, "eol_config: initialized\n");
}

void eol_config_deinit(void)
{
  eol_logger_message( EOL_LOG_INFO, "eol_config: closing\n");
  _eol_config_initialized = eolFalse;
  eol_logger_message( EOL_LOG_INFO, "eol_config: closed\n");
}

eolConfig *eol_config_load(char* filename)
{
  yaml_parser_t parser;
  FILE *input;
  eolConfig *config;
  
  if(!yaml_parser_initialize(&parser))
  {
    eol_logger_message( EOL_LOG_ERROR, "eol_config: Failed to initialize yaml parser\n");
    return NULL;
  }
  
  config = malloc(sizeof(eolConfig));
  if (config == NULL)
  {
    eol_logger_message( EOL_LOG_ERROR, "eol_config: Unable to allocate config!\n");
    return NULL;
  }
  
  eol_line_cpy(config->filename, filename);
  config->_node = eol_keychain_new_hash();
  if(config->_node == NULL)
  {
    eol_logger_message( EOL_LOG_ERROR, "eol_config: Unable to allocate keychain for config\n");
    return NULL;
  }
  input = fopen(filename, "r");
  if(input == NULL)
  {
    eol_logger_message( EOL_LOG_ERROR, "eol_config: Can't open config file %s\n", filename );
    return NULL;
  }
  yaml_parser_set_input_file(&parser, input);

  eol_config_parse_tier(&parser, config->_node);

  yaml_parser_delete(&parser);
  fclose(input);
  return config;
}

void eol_config_destroy(eolConfig *config)
{
  if (!config)return;
  eol_keychain_destroy(config->_node);
  free(config);
}

void eol_config_free(eolConfig **config)
{
  if (!config)return;
  if (!*config)return;
  eol_config_destroy(*config);
  *config = NULL;
}

void eol_config_parse_sequence(yaml_parser_t *parser, eolKeychain *chain)
{
  int done = 0;
  eolKeychain *next = NULL;
  yaml_event_t event;
  /* First element must be a variable, or we'll change states to SEQ */
  int state = KEY;
  do
  {
    yaml_parser_parse(parser, &event);
    switch(event.type)
    {
      case YAML_MAPPING_START_EVENT:
        next = eol_keychain_new_hash();
        eol_keychain_list_append(chain,next);
        state ^= VAL;
        eol_config_parse_tier(parser, next);
        break;
      case YAML_SEQUENCE_END_EVENT:
      case YAML_MAPPING_END_EVENT:
      case YAML_STREAM_END_EVENT:
        done = 1;
        /* terminate the while loop, see below */
        break;
      default:
        eol_logger_message( EOL_LOG_INFO, "unhandled YAML event %d\n", event.type);
    }
    if(parser->error != YAML_NO_ERROR)
    {
      eol_logger_message( EOL_LOG_ERROR, "yaml_error_type_e %d: %s %s at (line: %lu, col: %lu)\n",
                          parser->error, parser->context, parser->problem, parser->problem_mark.line,
                          parser->problem_mark.column);
                          return;
    }
    yaml_event_delete(&event);
  }while (!done);
}

void eol_config_parse_tier(yaml_parser_t *parser, eolKeychain *chain)
{
  int done = 0;
  eolLine last_key;
  eolKeychain *next = NULL;
  yaml_event_t event;
  /* First element must be a variable, or we'll change states to SEQ */
  int state = KEY;
  eol_line_cpy(last_key,"");
  do
  {
    yaml_parser_parse(parser, &event);
    switch(event.type)
    {
      case YAML_SCALAR_EVENT:
        if (state == KEY)
        {
          /* new key, hold on to it until we get a value as well */
          eol_line_cpy(last_key,(char *)event.data.scalar.value);
        }
        else
        {
          /* state is VAL or SEQ */
          /* TODO data type logic should go here */
          eol_logger_message( EOL_LOG_INFO, "eol_config: adding key -> value (%s -> %s)\n", last_key, event.data.scalar.value);
          next = eol_keychain_new_string((char *)event.data.scalar.value);
          eol_keychain_hash_insert(chain,last_key,next);
        }
        state ^= VAL; /* Toggles KEY/VAL, avoids touching SEQ */
        break;
      case YAML_SEQUENCE_START_EVENT:
        eol_logger_message(EOL_LOG_INFO,"eol_config: adding sequence %s...\n",last_key);
        next = eol_keychain_new_list();
        eol_keychain_hash_insert(chain,last_key,
                                 next);
        eol_config_parse_sequence(parser, next);
        break;
      case YAML_MAPPING_START_EVENT:
        if (strlen(last_key) == 0)break;/*first level is implied hash.*/
        eol_logger_message(EOL_LOG_INFO,"eol_config: adding hash %s...\n",last_key);
        next = eol_keychain_new_hash();
        eol_keychain_hash_insert(chain,last_key,next);
        state ^= VAL;
        eol_config_parse_tier(parser, next);
        break;
      case YAML_MAPPING_END_EVENT:
      case YAML_STREAM_END_EVENT:
      case YAML_SEQUENCE_END_EVENT:
        done = 1;
        /* terminate the while loop, see below */
        break;
      default:
        eol_logger_message( EOL_LOG_INFO, "unhandled YAML event %d\n", event.type);
    }
    if(parser->error != YAML_NO_ERROR)
    {
      eol_logger_message( EOL_LOG_ERROR, "yaml_error_type_e %d: %s %s at (line: %lu, col: %lu)\n",
                          parser->error, parser->context, parser->problem, parser->problem_mark.line,
                          parser->problem_mark.column);
                          return;
    }
    yaml_event_delete(&event);
  }
  while (!done);
}

eolBool eol_config_get_keychain(eolKeychain *output,
                                eolConfig *conf)
{
  g_return_val_if_fail(conf->_node, eolFalse);
  g_return_val_if_fail(output, eolFalse);
  output = conf->_node;
  return eolTrue;
}

eolBool eol_config_get_keychain_by_tag(eolKeychain **output,
                                       eolConfig *conf,
                                       eolLine tag)
{
  g_return_val_if_fail(conf->_node, eolFalse);
  g_return_val_if_fail(output, eolFalse);
  *output = eol_keychain_get_hash_value(conf->_node, tag);
  if (*output == NULL)return eolFalse;
  return eolTrue;
}


eolBool eol_config_get_vec3d_by_tag(
  eolVec3D  *output,
  eolConfig *conf,
  eolLine    tag
)
{
  g_return_val_if_fail(conf, eolFalse);
  g_return_val_if_fail(output, eolFalse);
  return eol_keychain_get_hash_value_as_vec3d(output, conf->_node, tag);
}

eolBool eol_config_get_float_by_tag(eolFloat *output, eolConfig *conf, eolLine tag)
{
  g_return_val_if_fail(conf, eolFalse);
  g_return_val_if_fail(output, eolFalse);
  return eol_keychain_get_hash_value_as_float(output, conf->_node, tag);
}

eolBool eol_config_get_int_by_tag(eolInt *output, eolConfig *conf, eolLine tag)
{
  g_return_val_if_fail(conf, eolFalse);
  g_return_val_if_fail(output, eolFalse);
  return eol_keychain_get_hash_value_as_int(output, conf->_node, tag);
}

eolBool eol_config_get_uint_by_tag(eolUint *output, eolConfig *conf, eolLine tag)
{
  g_return_val_if_fail(conf, eolFalse);
  g_return_val_if_fail(output, eolFalse);
  return eol_keychain_get_hash_value_as_uint(output, conf->_node, tag);
}

eolBool eol_config_get_bool_by_tag(eolBool *output, eolConfig *conf, eolLine tag)
{
  g_return_val_if_fail(conf, eolFalse);
  g_return_val_if_fail(output, eolFalse);
  return eol_keychain_get_hash_value_as_bool(output, conf->_node, tag);
}

eolBool eol_config_get_line_by_tag( eolLine output,  eolConfig *conf, eolLine tag)
{
  g_return_val_if_fail(conf, eolFalse);
  return eol_keychain_get_hash_value_as_line(output, conf->_node, tag);
}

eolBool eol_config_get_rectfloat_by_tag( eolRectFloat * output,  eolConfig *conf, eolLine tag)
{
  g_return_val_if_fail(conf, eolFalse);
  return eol_keychain_get_hash_value_as_rectfloat(output, conf->_node, tag);
}

/*eol@eof*/

