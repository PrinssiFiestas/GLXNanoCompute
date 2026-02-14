CFLAGS = -Wall -Wextra -Iinclude -gdwarf -ggdb3 -fsanitize=address -fsanitize=undefined -fsanitize=leak
LFLAGS = -lX11 -lGLX -lGL

all:
	gcc $(CFLAGS) src/*.c $(LFLAGS)
