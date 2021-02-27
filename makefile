CC=gcc

CFLAGS= -std=c11 -w -I. -pthread
# Add dependencies
DEPS = hello.h validator.h car.h

LDFLAGS=-pthread
OBJECTS=main.o src/*.o
TARGET=main

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

include depends

depends:
	$(CC) -MM $(OBJECTS:.o=.c) > depends

clean:
	rm ./$(TARGET) *.o