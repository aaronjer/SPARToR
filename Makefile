
MODNAME = mcdiddy
EXE_NAME = spartor_linux
OBJS = engine/main.o engine/font.o engine/console.o engine/command.o engine/net.o engine/host.o engine/client.o engine/input.o engine/video.o engine/sjdl.o $(MODNAME)/mod.o
CC = gcc
FLAGS = -g -Wall -DGLEW_STATIC
LIBS = -lm -lSDL -lSDL_net -lSDL_image -lGL -lGLU -lGLEW
INC = -I ./engine -I ./$(MODNAME)


$(EXE_NAME): $(OBJS)
	$(CC) -o $@ $(OBJS) $(FLAGS) $(INC) $(LIBS) $(XLIBS)
	-rm $(OBJS)

$(OBJS):
	$(CC) -o $@ -c `sdl-config --cflags` $(FLAGS) $(INC) $*.c

clean:
	-rm $(OBJS)
