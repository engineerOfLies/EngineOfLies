#include "eol_config.h"
#include <glib.h>
#include <yaml.h>

/*local globals*/
eolBool _eol_config_initialized;


/*local functions*/
void eol_config_deinit(void);
void eol_config_parse_tier(yaml_parser_t *parser, GNode *cfg);

/*function definitions*/

void eol_config_init()
{
    fprintf(stdout,"eol_config: initializing\n");
    _eol_config_initialized = eolTrue;
    atexit(eol_config_deinit);
    fprintf(stdout,"eol_config: initialized\n");
}

void eol_config_deinit(void)
{
    // NOP for now
}

eolConfig *eol_config_load(char* filename)
{
    yaml_parser_t parser;
    yaml_event_t event;
    eolConfig *config = malloc(sizeof(eolConfig));
    strcpy(config->filename, filename);
    config->_node = g_node_new(filename);
    if(config->_node == NULL) {
        fputs("ERROR: Unable to allocate GNode for config", stderr);
        return NULL;
    }
    if(!yaml_parser_initialize(&parser)) {
        fputs("ERROR: Failed to initialize yaml parser", stderr);
        return NULL;
    }
    FILE *input = fopen(filename, "r");
    if(input == NULL) {
        fprintf(stderr, "ERROR, Can't open config file %s", filename );
        return NULL;
    }
    yaml_parser_set_input_file(&parser, input);
    eol_config_parse_tier(&parser, config->_node);
    if(parser.error != YAML_NO_ERROR) {
        fprintf(stderr, "ERROR: yaml_error_type_e %d: %s %s at (line: %d, col: %d)\n",
                parser.error, parser.context, parser.problem, parser.problem_mark.line,
                parser.problem_mark.column);
        return NULL;
    }
    if(event.type != YAML_STREAM_END_EVENT)
        yaml_event_delete(&event);
    yaml_parser_delete(&parser);

    fclose(input);
    return config;
}

void eol_config_destroy(eolConfig *config) 
{
    g_node_destroy(config->_node);
    free(config);
}

void eol_config_parse_tier(yaml_parser_t *parser, GNode *cfg)
{
    GNode *last_leaf = cfg;
    yaml_event_t event;
    // First element must be a variable, or we'll change states to SEQ
    int state = VAR;
    do {
        yaml_event_delete(&event);
        yaml_parser_parse(parser, &event);
        switch(event.type) {
        case YAML_SCALAR_EVENT:
            if (state == VAR) {
                // new variable, create a new element
                last_leaf = g_node_append(cfg, g_node_new(g_strdup((gchar*) event.data.scalar.value)));
            } else {
                // state is VAL or SEQ
                g_node_append_data(last_leaf, g_strdup((gchar*) event.data.scalar.value));
            }
            state ^= VAL; // Toggles VAR/VAL, avoids touching SEQ
            break;
        case YAML_SEQUENCE_START_EVENT:
            state = SEQ;
            break;
        case YAML_SEQUENCE_END_EVENT:
            state = VAR;
            break;
        case YAML_MAPPING_START_EVENT:
            eol_config_parse_tier(parser, last_leaf);
            state ^= VAL;
            break;
        case YAML_MAPPING_END_EVENT:
        case YAML_STREAM_END_EVENT:
            // terminate the while loop, see below
            break;
        default:
            fprintf(stderr, "ERROR: unhandled YAML event %d\n", event.type);
        }
        if(parser->error != YAML_NO_ERROR) {
            fprintf(stderr, "ERROR: yaml_error_type_e %d: %s %s at (line: %d, col: %d)\n",
                    parser->error, parser->context, parser->problem, parser->problem_mark.line,
                    parser->problem_mark.column);
            return;
        }
    } while( event.type != YAML_MAPPING_END_EVENT && event.type != YAML_STREAM_END_EVENT );
    yaml_event_delete(&event);

}

void eol_config_dump(char* filename, GNode* data)
{
    // NOP
}


eolInt eol_config_get_int_by_tag( eolConfig *conf, eolLine tag)
{
	GNode *gndata = g_node_find(conf->_node, G_PRE_ORDER, G_TRAVERSE_ALL, tag);
	if(gndata != NULL) {
		return (eolInt) gndata->data;
	} else {
		printf("Config tag %s not found in %s\n", tag, conf->filename);
		return NULL;
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

