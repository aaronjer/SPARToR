#Required packages:
#  libsdl1.2-dev
#  libsdl-net1.2-dev
EXE_NAME = core
OBJS = main.o font.o console.o sjstring.o command.o net.o host.o client.o sjui.o input.o video.o mod.o tests.o
CC = gcc
FLAGS = -g -Wall
LIBS = -lm -lSDL -lSDL_net


$(EXE_NAME): $(OBJS)
	$(CC) -o $@ $(OBJS) $(FLAGS) $(INC) $(LIBS) $(XLIBS)

$(OBJS):
	$(CC) -o $@ -c `sdl-config --cflags` $(FLAGS) $(INC) $*.c

clean:
	-rm $(OBJS)
