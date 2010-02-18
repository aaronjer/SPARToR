#Required packages:
#  libsdl1.2-dev
#  libsdl-net1.2-dev
#  libsdl-image1.2-dev
EXE_NAME = spartor_linux
OBJS = main.o font.o console.o command.o net.o host.o client.o input.o video.o mod.o tests.o
CC = gcc
FLAGS = -g -Wall
LIBS = -lm -lSDL -lSDL_net -lSDL_image


$(EXE_NAME): $(OBJS)
	$(CC) -o $@ $(OBJS) $(FLAGS) $(INC) $(LIBS) $(XLIBS)
	-rm $(OBJS)

$(OBJS):
	$(CC) -o $@ -c `sdl-config --cflags` $(FLAGS) $(INC) $*.c

clean:
	-rm $(OBJS)
