#include "eol_config.h"
#include "eol_logger.h"
#include <glib.h>
#include <yaml.h>

/*local globals*/
eolBool _eol_config_initialized;


/*local functions*/
void eol_config_deinit(void);
void eol_config_parse_tier(yaml_parser_t *parser, GHashTable *cfg);

/*function definitions*/

void eol_config_init()
{
  eol_logger_message( EOL_LOG_INFO, "eol_config: initializing\n");
  _eol_config_initialized = eolTrue;
  atexit(eol_config_deinit);
  eol_logger_message( EOL_LOG_INFO, "eol_config: initialized\n");
}

void eol_config_deinit(void)
{
  /* NOP for now */
}

eolConfig *eol_config_load(char* filename)
{
  yaml_parser_t parser;
  FILE *input;
  eolConfig *config = malloc(sizeof(eolConfig));
  eol_line_cpy(config->filename, filename);
  config->_node = g_hash_table_new(&g_str_hash, &g_str_equal);
  if(config->_node == NULL)
    {
      /*use the logger, I made a ticket for this.*/
      eol_logger_message( EOL_LOG_ERROR, "Unable to allocate GHash for config");
      return NULL;
    }
  if(!yaml_parser_initialize(&parser))
    {
      eol_logger_message( EOL_LOG_ERROR, "Failed to initialize yaml parser");
      return NULL;
    }
  input = fopen(filename, "r");
  if(input == NULL)
    {
      eol_logger_message( EOL_LOG_ERROR, "Can't open config file %s", filename );
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
  g_hash_table_destroy(config->_node);
  free(config);
}

void eol_config_parse_tier(yaml_parser_t *parser, GHashTable *cfg)
{
  gchar *last_key;
  yaml_event_t event;
  /* First element must be a variable, or we'll change states to SEQ */
  int state = KEY;
  do
    {
			/* NOTE: using this initialized :-/ */
			yaml_event_delete(&event);
      yaml_parser_parse(parser, &event);
      switch(event.type)
        {
        case YAML_SCALAR_EVENT:
          if (state == KEY)
            {
              /* new key, hold on to it until we get a value as well */
              last_key = g_strdup((gchar*) event.data.scalar.value);
            }
          else
            {
              /* state is VAL or SEQ */
              /* TODO data type logic should go here */
              eol_logger_message( EOL_LOG_INFO, "adding key -> value (%s -> %s)\n", last_key, event.data.scalar.value);
              g_hash_table_insert(cfg, last_key, g_strdup((gchar*) event.data.scalar.value));
            }
          state ^= VAL; /* Toggles KEY/VAL, avoids touching SEQ */
          break;
        case YAML_SEQUENCE_START_EVENT:
          state = SEQ;
          break;
        case YAML_SEQUENCE_END_EVENT:
          state = KEY;
          break;
        case YAML_MAPPING_START_EVENT:
          /* create a child hash table and recurse
          mapping = g_hash_table_new(&g_str_hash, &g_str_equal);
          g_hash_table_insert(cfg, last_key, mapping);
                eol_config_parse_tier(parser, mapping);
                state ^= VAL;
          */
          break;
        case YAML_MAPPING_END_EVENT:
        case YAML_STREAM_END_EVENT:
          /* terminate the while loop, see below */
          break;
        default:
          eol_logger_message( EOL_LOG_WARN, "unhandled YAML event %d\n", event.type);
        }
      if(parser->error != YAML_NO_ERROR)
        {
          eol_logger_message( EOL_LOG_ERROR, "yaml_error_type_e %d: %s %s at (line: %lu, col: %lu)\n",
                  parser->error, parser->context, parser->problem, parser->problem_mark.line,
                  parser->problem_mark.column);
          return;
        }
    }
  while( event.type != YAML_MAPPING_END_EVENT && event.type != YAML_STREAM_END_EVENT );
  yaml_event_delete(&event);

}

void eol_config_dump(char* filename, GHashTable* data)
{

}

void eol_config_print(GHashTable* data)
{
}

eolBool eol_config_get_int_by_tag(eolInt *output, eolConfig *conf, eolLine tag)
{
  gchar *data;
  g_return_val_if_fail(conf->_node, eolFalse);
	g_return_val_if_fail(output, eolFalse);

  data = g_hash_table_lookup(conf->_node, tag);
  if(data)
    {
      *output = (eolInt) atoi(data);
			return eolTrue;
    }
  else
    {
      eol_logger_message( EOL_LOG_WARN,"Config tag %s not found in %s\n", tag, conf->filename);
      return eolFalse;
    }
}

eolBool eol_config_get_line_by_tag( eolLine output,  eolConfig *conf, eolLine tag)
{
  gchar *data;
	g_return_val_if_fail(conf->_node, eolFalse);
	g_return_val_if_fail(output, eolFalse);

  data = g_hash_table_lookup(conf->_node, tag);
  if(data)
    {
      g_strlcpy(output, data, sizeof(eolLine));
			return eolTrue;
    }
  else
    {
      eol_logger_message( EOL_LOG_WARN, "Config tag %s not found in %s\n", tag, conf->filename);
      return eolFalse;
    }
}
/*
void eol_config_get_line_by_tag(
    eolLine     output,
    eolConfig * conf,
    eolLine     tag
);

eolUint eol_config_get_list_count_by_tag(
    eolConfig * conf,
    eolLine     tag
);

GList * eol_config_get_list_by_tag(
    eolConfig * conf,
    eolLine     tag
);
*/

/*eol@eof*/

