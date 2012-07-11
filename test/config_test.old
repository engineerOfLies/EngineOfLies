#include <yaml.h>
#include <glib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    yaml_parser_t parser;
    yaml_event_t event;
    if(!yaml_parser_initialize(&parser)) {
        fputs("Failed to initialize yaml parser", stderr);
        return 1;
    }
    FILE *input = fopen(argv[1], "r");
    if(input == NULL) {
        fputs("Can't open config file", stderr);
        return 1;
    }
    yaml_parser_set_input_file(&parser, input);
    do {
        yaml_parser_parse(&parser, &event);
        switch(event.type) {
        case YAML_NO_EVENT:
		puts("No event!\n");
            break;
            /* Stream start/end */
        case YAML_STREAM_START_EVENT:
            puts("STREAM START");
            break;
        case YAML_STREAM_END_EVENT:
            puts("STREAM END");
            break;
            /* Block delimeters */
        case YAML_DOCUMENT_START_EVENT:
            puts("<b>Start Document</b>");
            break;
        case YAML_DOCUMENT_END_EVENT:
            puts("<b>End Document</b>");
            break;
        case YAML_SEQUENCE_START_EVENT:
            puts("<b>Start Sequence</b>");
            break;
        case YAML_SEQUENCE_END_EVENT:
            puts("<b>End Sequence</b>");
            break;
        case YAML_MAPPING_START_EVENT:
            puts("<b>Start Mapping</b>");
            break;
        case YAML_MAPPING_END_EVENT:
            puts("<b>End Mapping</b>");
            break;
            /* Data */
        case YAML_ALIAS_EVENT:
            printf("Got alias (anchor %s)\n", event.data.alias.anchor);
            break;
        case YAML_SCALAR_EVENT:
            printf("Got scalar (value %s)\n", event.data.scalar.value);
            break;
        default:
            printf("Unhandled event %d\n", event.type);
        }
	if(parser.error != YAML_NO_ERROR) {
            printf("ERROR: yaml_error_type_e %d: %s %s at (line: %d, col: %d)\n", parser.error, parser.context, parser.problem, parser.problem_mark.line, parser.problem_mark.column);
	    return parser.error;
	}
        if(event.type != YAML_STREAM_END_EVENT)
            yaml_event_delete(&event);
    } while(event.type != YAML_STREAM_END_EVENT);
    yaml_event_delete(&event);
    yaml_parser_delete(&parser);
    fclose(input);
    return 0;
}
