#include <glib-2.0/glib.h>
#include <yaml.h>
#include <stdio.h>
#include "eol_config.h"

gboolean dump(GNode *node, gpointer data);

int main(int argc, char* argv[])
{
	eol_config_init();
	printf("Opening yaml config %s\n", argv[1]);
	eolConfig *config = eol_config_load(argv[1]);
	//if(config != NULL) g_node_destroy(config);
	g_node_traverse(config->_node, G_PRE_ORDER, G_TRAVERSE_ALL, -1, dump, NULL);
	eolInt pattern = 640;
	printf("Searching config for %d\n", pattern);
	GNode *found = g_node_find(config->_node, G_PRE_ORDER, G_TRAVERSE_ALL, &pattern);
	if(found != NULL) {
		g_node_traverse(found, G_PRE_ORDER, G_TRAVERSE_ALL, -1, dump, NULL);
	} else {
		printf("Pattern %d not found\n", pattern);
	}
	//printf("%s and yres are (%d, %d)\n", pattern, eol_config_get_int_by_tag(eolconf, pattern), eol_config_get_int_by_tag(eolconf, "yRes"));
	eol_config_destroy(config);
	return 0;
}


gboolean dump(GNode *node, gpointer data) {
	    int i = g_node_depth(node);
	        while (--i) printf("-");
		    printf("%s\n", (char*) node->data);
		    return(FALSE);
}
