############################################################################
#
# The Linux-GCC Makefile
#
##############################################################################

#
# Object files.
#

OBJ = $(patsubst %.c,%.o,$(wildcard *.c))

#
# Compiler stuff -- adjust to your system.
#

# Linux
PROJECT = test_app
CC      = gcc
#CC	= clang
EOL_PATH = ../../EngineOfLies/
GLIB_CFLAGS = `pkg-config --cflags glib-2.0`
GLIB_LDFLAGS = `pkg-config --libs glib-2.0`
SDL_CFLAGS = `sdl-config --cflags`
SDL_LDFLAGS = `sdl-config --libs` -lSDL_image -lSDL_ttf -lSDL_mixer -lSDL_net -lGLU -lz -lphysfs -lglib-2.0 -lyaml
YAML_LDFLAGS = /usr/lib/x86_64-linux-gnu/libyaml.so
EOL_CFLAGS = -I$(EOL_PATH)/include
EOL_LDFLAGS = $(EOL_PATH)/lib/libeol.a
LFLAGS = -g  -o ../$(PROJECT) -lphysfs
CFLAGS = -g -Wall -pedantic -std=gnu99 -fgnu89-inline -Wno-unknown-pragmas 
# -ffast-math for relase version

DOXYGEN = doxygen

#
# Targets
#

$(PROJECT): $(OBJ)
	$(CC) $(OBJ) $(LFLAGS) $(EOL_LDFLAGS) $(SDL_LDFLAGS) $(GLIB_LDFLAGS) $(YAML_LDFLAGS) libchipmunk.a
docs:
	$(DOXYGEN) doxygen.cfg
  
depend:
	@-rm makefile.dep
	@echo Creating dependencies.
	@for i in *.c; do $(CC) $(EOL_CFLAGS) $(INC) -MM $$i; done > makefile.dep
	@echo Done.

clean:
	rm *.o ../$(PROJECT)

count:
	wc *.c *.h makefile

#
# Dependencies.
#

include makefile.dep

#
# The default rule.
#

.c.o:
	$(CC) $(CFLAGS) $(EOL_CFLAGS) $(SDL_CFLAGS) $(GLIB_CFLAGS) -c $<

