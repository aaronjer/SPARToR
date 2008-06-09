EXE_NAME = core
OBJS = main.o font.o console.o
CC = gcc
FLAGS = -g
LIBS =


.C.o:
	$(CC) $< -c `sdl-config --cflags` $(FLAGS) $(INC)

$(EXE_NAME): $(OBJS)
	$(CC) -o $@ $(OBJS) $(FLAGS) $(INC) $(LIBS) $(XLIBS) -lm -lSDL -lSDL_net

clean:
	rm $(OBJS)
