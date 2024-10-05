CC = g++

SRC = src/main.cpp src/glad.c
EXEC = bin/game

CFLAGS = -Ilib/GLAD/include
LDFLAGS = -lglfw -lGL

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(SRC) -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -r $(EXEC)

.PHONY: all clean
