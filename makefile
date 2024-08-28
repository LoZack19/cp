CC=gcc
CFLAGS=-g -Wall -Wextra -I.

TARGET=cp
OBJ=src/copy.o main.o src/pathresolve.o

.PHONY=all
all: $(TARGET)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY=clean
clean:
	rm *.o $(TARGET)

.PHONY=count
count:
	wc -l main.c src/*.c include/*.h