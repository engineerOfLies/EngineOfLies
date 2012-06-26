#include <glib.h>
#include "eol_config.h"

int main(int argc, char* argv[])
{
	eol_config_init();
	GNode *config = eol_config_load("./graphics.yaml");
	return 0;
}
