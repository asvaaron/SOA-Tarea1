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


depends:
	$(CC) -MM $(OBJECTS:.o=.c) > depends

clean:
	rm ./$(TARGET) *.o