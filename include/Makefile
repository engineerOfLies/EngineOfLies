##############################################################################
#
# The Linux-GCC Makefile
#
##############################################################################

#
# Object files.
#

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

#
# Compiler stuff -- adjust to your system.
#

# Linux
PROJECT = eol
CC      = gcc
#CC      = clang
GLIB_CFLAGS = `pkg-config --cflags glib-2.0`
GLIB_LDFLAGS = `pkg-config --libs glib-2.0`
SDL_CFLAGS = `sdl-config --cflags` 
SDL_LDFLAGS = `sdl-config --libs` -lSDL_image -lSDL_ttf -lSDL_mixer -lSDL_net -lGLU -lz -lphysfs -lglib-2.0 -lchipmunk
LFLAGS = -g -shared -Wl,-soname,lib$(PROJECT).so.1 -o ../lib/lib$(PROJECT).so.1.0.1
CFLAGS = -g  -fPIC -Wall -pedantic -Wno-unknown-pragmas 
# -fgnu89-inline 
# -std=gnu99 
# -ffast-math for relase version

DOXYGEN = doxygen

#
# Targets
#

$(PROJECT): $(OBJECTS)
<<<<<<< HEAD
	$(CC) $(OBJECTS) $(LFLAGS) $(SDL_LDFLAGS) $(GLIB_LDFLAGS)
=======
	$(CC) $(OBJECTS) $(LFLAGS) $(SDL_LDFLAGS) $(GLIB_LDFLAGS) -lchipmunk
>>>>>>> a507eb15fddba2e68b80df53aa69b13189bc98bc

docs:
	$(DOXYGEN) doxygen.cfg

sources:
	echo $(patsubst %.c,%.o,$(wildcard *.c)) > makefile.sources
#	@-rm makefile.sources
#	@echo Building source list
#	@for i in *.c; do echo `basename $$i .c`.o\\ >> makefile.sources; done
#	@echo Done

makefile.dep: depend

depend:
	@-rm makefile.dep
	@echo Creating dependencies.
	@for i in *.c; do $(CC) $(INC) -MM $$i; done > makefile.dep
	@echo Done.

clean:
	rm *.o ../lib/lib$(PROJECT).a

count:
	wc *.c *.h makefile

static:
	ar rcs ../lib/lib$(PROJECT).a $(OBJECTS)

#
# Dependencies.
#

include makefile.dep

#
# The default rule.
#

.c.o:
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $(GLIB_CFLAGS) -c $<


