CC=gcc
CFLAGS=-g -Wall -Wextra

TARGET=cp
OBJ=copy.o main.o pathresolve.o

all: $(TARGET)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o $(TARGET)