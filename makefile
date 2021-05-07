CC=gcc
CFLAGS=-g -Wall -Wextra

TARGET=cp
OBJ=copy.o main.o pathresolve.o

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
	wc -l *.c *.h