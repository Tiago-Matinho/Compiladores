CC = clang
CFLAGS = -g
OUTFILE = ya!c
LIBS = -lm

OFILES = t_exp.o


all: parser.c lexer.c parser.h yalangcom.o latex.o
	gcc $(CFLAGS) parser.c lexer.c yalangcom.o latex.o -o $(OUTFILE) $(LIBS)

parser.c: yalang.y
	bison -y -d yalang.y -o parser.c

lexer.c: yalang.lex
	flex -o lexer.c yalang.lex

yalangcom.o: yalangcom.h yalangcom.c
	gcc $(CFLAGS) -c yalangcom.c

latex.o: latex.h latex.c
	gcc $(CFLAGS) -c latex.c
clean: 
	rm -rf parser.c lexer.c parser.h
	rm -f *~
	rm -f *.o
	rm -f *.output
	rm -f 'ya!c'

clean-all clean_all: clean
	rm -f $(OUTFILE)
