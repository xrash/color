PROGRAM = color
BIN = bin
SRC = src
INSTALL_PATH = /usr/local/bin
BINARY = $(BIN)/$(PROGRAM)
OBJECTS = $(BIN)/main.o $(BIN)/colors.o $(BIN)/options.o $(BIN)/list.o $(BIN)/scanner.o
CFLAGS = -Wall -Werror -Wextra -pedantic -ansi
LDFLAGS = -lpcre

# linking
$(BINARY) : $(OBJECTS)
	gcc $(ARGS) $(CFLAGS) -o $(BINARY) $(OBJECTS) $(LDFLAGS)

$(BIN)/main.o : $(SRC)/main.c $(SRC)/colors.h $(SRC)/options.h
	gcc $(ARGS) $(CFLAGS) -c $(SRC)/main.c -o $(BIN)/main.o

$(BIN)/colors.o : $(SRC)/colors.c
	gcc $(ARGS) $(CFLAGS) -c $(SRC)/colors.c -o $(BIN)/colors.o

$(BIN)/options.o : $(SRC)/options.c
	gcc $(ARGS) $(CFLAGS) -c $(SRC)/options.c -o $(BIN)/options.o

$(BIN)/list.o : $(SRC)/list.c
	gcc $(ARGS) $(CFLAGS) -c $(SRC)/list.c -o $(BIN)/list.o

$(BIN)/scanner.o : $(SRC)/scanner.c
	gcc $(ARGS) $(CFLAGS) -c $(SRC)/scanner.c -o $(BIN)/scanner.o

install: $(BINARY)
	cp $(BINARY) $(INSTALL_PATH)/$(PROGRAM) -v

.PHONY : clean
clean :
	-rm $(BINARY) $(OBJECTS)

.PHONY : run
run :
	make
	#echo -n "Sao Paulo, 5 de Janeiro de 2014." | bin/color -m char -c green/red '[^0-9]'
	#cat ~/T.go | bin/color import
	#echo -e "Sao Paulo, Sao Paulo\n, 5 de Janeiro de 2014." | bin/color 'o Paulo,' -c cyan 'Sao Paulo' -c yellow/blue Janeiro -c green/red de -c cyan
	echo -n "woaokiwachawokiaokcha" | bin/color -c red/blue -c yellow -c green okiw ok o ia cha

.PHONY : gdb
gdb :
	make ARGS="-g"
	gdb bin/color
