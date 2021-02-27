CC=gcc
# Use c11 standard
# Ignore warnings
CFLAGS= -lpthread -std=c11 -w -I.
# Add dependencies
DEPS = hello.h validator.h car.h


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mainmake: main.o
	$(CC) -o main main.o src/validator.c src/hello.c src/car.c

clean:
	rm *.o main