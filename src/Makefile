TARGET  = labyrinth
CC      = gcc
CFLAGS  = -g -std=c99 -Wall -Wextra -I/usr/X11R6/include -I/usr/pkg/include
LDFLAGS = -L/usr/X11R6/lib -L/usr/pkg/lib
LDLIBS  = -lglut -lGLU -lGL -lm

$(TARGET): main.o ball.o board.o image.o level.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) $(CFLAGS)

.PHONY: clean

clean:
	rm $(TARGET) *.o

main.o: main.c ball.c board.c image.c level.c
	$(CC) $(LDFLAGS) -o $@ -c $< $(LDLIBS) $(CFLAGS)
