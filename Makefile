# Multi-platform makefile for SPARToR

# Get system name
UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')

# Same for all platforms, probably
CC = gcc
MODNAME = mcdiddy
OBJS = engine/main.o \
       engine/font.o \
       engine/console.o \
       engine/command.o \
       engine/net.o \
       engine/host.o \
       engine/client.o \
       engine/input.o \
       engine/video.o \
       engine/sjdl.o \
       engine/patt.o \
       mt19937ar/mt19937ar.o

include $(MODNAME)/Makefile-include

# Only useful on certain platforms
OBJSRES =
WINDRES =
POSTCC =


ifeq ($(UNAME),Linux)
	EXE_NAME = spartor_linux
	FLAGS = `sdl-config --cflags` -g -Wall -DGLEW_STATIC
	LIBS = -lm -lSDL -lSDL_net -lSDL_image -lGL -lGLU -lGLEW
	INC = -Iengine -I$(MODNAME) -Imt19937ar
endif
ifeq ($(UNAME),Darwin)
	EXE_NAME = spartor_mac
	FLAGS = -g -Wall -DGLEW_STATIC
	LIBS = spartor_mac.app/SDLmain.m \
	       -lm \
	       -L/opt/local/var/macports/software/glew/1.5.8_0/opt/local/lib -lGLEW \
	       -framework OPENGL \
	       -framework SDL \
	       -framework SDL_net \
	       -framework SDL_image \
	       -framework Cocoa \
	       -Fspartor_mac.app/Contents/Frameworks
	INC = -Iengine -I$(MODNAME) -Imt19937ar \
	      -I/opt/local/var/macports/software/glew/1.5.8_0/opt/local/include \
	      -Ispartor_mac.app \
	      -Ispartor_mac.app/Contents/Frameworks/SDL.framework/Headers \
	      -Ispartor_mac.app/Contents/Frameworks/SDL_net.framework/Headers \
	      -Ispartor_mac.app/Contents/Frameworks/SDL_image.framework/Headers
	POSTCC = mv $(EXE_NAME) spartor_mac.app/Contents/MacOS
endif
ifneq (,$(findstring MINGW,$(UNAME)))
	EXE_NAME = spartor_win32.exe
	OBJSRES = $(MODNAME)/icon.o
	WINDRES = windres
	FLAGS = -g -Wall -mwindows -DGLEW_STATIC
	LIBS = -L /local/lib -L ./glew-1.5.8/lib -lmingw32 -lSDLmain -lSDL -lSDL_net -lSDL_image -lglew32s -lopengl32 -lglu32 -lm
	INC = -Iengine -I$(MODNAME) -Imt19937ar -I/usr/local/include/SDL -Iglew-1.5.8/include
endif


$(EXE_NAME): $(OBJS) $(OBJSRES)
	$(CC) -o $@ $(OBJS) $(OBJSRES) $(FLAGS) $(INC) $(LIBS) $(XLIBS)
	$(POSTCC)
	-rm $(OBJS) $(OBJSRES)

$(OBJS):
	$(CC) -o $@ -c $(FLAGS) $(INC) $*.c

.rc.o:
	$(WINDRES) $^ -o $@
%.o : %.rc
	$(WINDRES) $^ -o $@

clean:
	-rm $(OBJS) $(OBJSRES)

