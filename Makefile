EXECUTABLE=lab_5.elf
SRC = src/main.c src/vectors.c src/equate.c
OBJ = $(SRC:.c=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	gcc -Wall $(OBJ) -o $(EXECUTABLE) -lm

%.o: %.c
	gcc -g -Wall -c $< -o $@

clean:
	rm -f $(OBJ) $(EXECUTABLE)

.PHONY: all clean
