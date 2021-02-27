CC=gcc

# Add compiler flags
CFLAGS= -std=c11 -w -I. -pthread
# Add Library flags
LDFLAGS=-pthread
# Sources path
DIR_SRC += ./src
OBJECTS=main.o $(DIR_SRC)/*.o
TARGET=main

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

include depends

depends:
	$(CC) -MM $(OBJECTS:.o=.c) > depends

clean:
	rm ./$(TARGET) *.o depends