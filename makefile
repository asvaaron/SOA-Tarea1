CC=gcc
# Use c11 standard
# Ignore warnings
CFLAGS= -std=c11 -w -I.
LDFLAGS=-pthread -w -lm
# Add dependencies
DEPS=include/color.h include/validator.h include/car.h

SRC_CD=src/validator.c  src/car.c  src/color.c

TARGET=main main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mainmake: main.o
	$(CC) -o $(TARGET) $(SRC_CD) $(LDFLAGS)

clean:
	rm *.o main