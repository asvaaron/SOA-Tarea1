CC=gcc
# Use c11 standard
# Ignore warnings
CFLAGS= -std=c11 -w -I.
DEPS = hello.h


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mainmake: hello.o main.o
	$(CC) -o main main.o hello.o

clean:
	rm *.o main