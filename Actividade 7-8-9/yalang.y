%{

#include <stdio.h>
#include "yalang_apt.h"
#include "latex.h"  /* print_prologue(), etc. */

extern int yylineno;
extern FILE *yyin;
extern char *yytext;

int yylex (void);
void yyerror (char const *);

%}
			
%union {
    int    ival;
    double dval;
    char   *str;
}
			
/* Bison declarations.  */
%token	<ival>          INTLIT BOOLLIT
%token	<dval>          FLOATLIT
%token	<str>           STRLIT ID
			
%token                  COLON
			            
%token			DEFINE RETURN STRUCT WHILE IF THEN ELSE DO NEXT BREAK
			
%token			T_INT T_FLOAT T_STRING T_BOOL T_VOID /* atomic types */
			
%left			SEMI    /* semicolon */
			
%right			ASSIGN

%left			COMMA

%left			OR
%left			AND
%left			NOT

%nonassoc		EQ NEQ GT GEQ LT LEQ

%left			SUB ADD 
%left			MUL DIV MOD
%right			POW
%left			NEG     /* negation--unary minus */

%nonassoc		LCBRACE RCBRACE
%nonassoc		LSBRACE RSBRACE
%nonassoc		LPAR RPAR


%%

program:  /*    empty */ 
        |	    decls       { print_prologue();
                                yalang_decls_print($1);
                                print_epilogue(); }
        ;

decls:   	    decl        {yalang_decls_new_empty($1); }
        |	    decl decls  {yalang_decls_new($1, $2);   }
                ;

decl:    	    ids COLON type SEMI {yalang_decl_new_var( yalang_var_new($1, $3) ); }
        |	    ids COLON type ASSIGN exp SEMI
        |	    ID LPAR RPAR COLON type LCBRACE stms RCBRACE SEMI
        |	    ID LPAR argdefs RPAR COLON type LCBRACE stms RCBRACE SEMI
        |	    DEFINE ID type SEMI
                ;

argdefs: 	    argdef
        |	    argdef COMMA argdefs
                ;

argdef:  	    ID COLON type
                ;

args:		    exp
        |	    exp COMMA args
                ;

ids:     	    ID                  {yalang_ids_new_empty($1) ;}
        |	    ID COMMA ids        {yalang_ids_new($1, $3) ;}
                ;

stms:    	    stm
        |	    stm stms
                ;

stm:     	    decl
        |	    exp SEMI
        |	    RETURN exp SEMI
        |	    IF exp THEN LCBRACE stms RCBRACE SEMI
        |	    IF exp THEN LCBRACE stms RCBRACE ELSE LCBRACE stms RCBRACE SEMI 
        |	    WHILE exp DO LCBRACE stms RCBRACE SEMI
        |       NEXT
                ;

type:    	    T_INT       {yalang_type_new_int() ;}
        |	    T_FLOAT     {yalang_type_new_float() ;}
        |	    T_STRING    {yalang_type_new_string() ;}
        |	    T_BOOL      {yalang_type_new_bool() ;}
        |	    T_VOID      {yalang_type_new_void() ;}
        |	    ID          {yalang_type_new_id() ;}
        |	    type LSBRACE INTLIT RSBRACE         {yalang_type_new_array($1, $3) ;}
                ;

lit:     	    INTLIT
        |	    FLOATLIT
        |	    STRLIT
        |	    BOOLLIT
                ;

exp:     	    lit
        |	    ID
        |	    exp LSBRACE INTLIT RSBRACE
        |	    exp ADD exp
        |	    exp SUB exp
        |	    exp MUL exp
        |	    exp DIV exp
        |	    exp POW exp
        |	    exp MOD exp
        |	    exp GT exp
        |	    exp LT exp
        |	    exp GEQ exp
        |	    exp LEQ exp
        |	    exp EQ exp
        |	    exp NEQ exp
        |	    exp AND exp
        |	    exp OR exp
        |	    NOT exp
        |	    SUB exp  %prec NEG
        |	    LPAR exp RPAR
        |	    ID LPAR RPAR
        |	    ID LPAR args RPAR
        |	    exp ASSIGN exp
                ;
%%


void yyerror (char const *s)
/* Called by yyparse on error.  */
{
    fprintf (stderr, "Syntax error at line %d at or near \"%s\"\n", yylineno, yytext);
}

int main( int argc, char **argv )
{
    ++argv, --argc;  

    if ( argc > 0 )
            yyin = fopen( argv[0], "r" );
    else
            yyin = stdin;

    return yyparse();
}

/*
int main (void)
{
    return yyparse();
}
*/
