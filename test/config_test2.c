#include <glib-2.0/glib.h>
#include <yaml.h>
#include <stdio.h>
#include "eol_config.h"

gboolean dump(GNode *node, gpointer data);

int main(int argc, char* argv[])
{
	/* gpointer *found; */
	eol_config_init();
	eolInt some_int = 0;
	eolLine mipmap;
	printf("Opening yaml config %s\n", argv[1]);
	eolConfig *config = eol_config_load(argv[1]);
	//if(config != NULL) g_node_destroy(config);
	eolLine pattern = "xRes";
	printf("Searching config for %s\n", pattern);
	/* found = g_hash_table_lookup(config->_node, &pattern); */
	some_int = eol_config_get_int_by_tag(config, "xRes");
	if(some_int != 0) {
		printf("Pattern %s is %d\n", pattern, some_int);
	} else {
		printf("Pattern %s not found\n", pattern);
	}
	printf("xRes and yRes are (%d, %d)\n", eol_config_get_int_by_tag(config, "xRes"), eol_config_get_int_by_tag(config, "yRes"));
	eol_config_get_line_by_tag(mipmap, config, "mipmapping");
	printf("Mip Mapping set to %s\n", mipmap);
	eol_config_destroy(config);
	return 0;
}


gboolean dump(GNode *node, gpointer data) {
	    int i = g_node_depth(node);
	        while (--i) printf("-");
		    printf("%s\n", (char*) node->data);
		    return(FALSE);
}
