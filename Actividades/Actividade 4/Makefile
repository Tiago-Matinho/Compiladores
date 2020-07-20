CC = gcc
CFLAGS = -Wall
OUTFILE = mycalc

$(OUTFILE): parser.c lexer.c 
	$(CC) $(CFLAGS) parser.c lexer.c -o $(OUTFILE)

parser.c: calc.y
	bison -y -d calc.y -o parser.c

lexer.c: calc.l
	flex -o lexer.c calc.l 

clean:
	rm -rf parser.c lexer.c parser.h
	rm -f *.o
	rm -f mycalc