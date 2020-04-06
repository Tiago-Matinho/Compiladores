%{

#include <stdio.h>
#include "yalangapt.h"

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

program:  		/*    empty */													{ $$ = NULL; }
		|	    decls  															{ $$ = $1; }
		;

decls:   	    decl															{ $$ = t_decls_new($1, NULL); }
		|	    decl decls	{ $$ = t_decls_new($1, $2); }
		;

decl:    	    ids COLON type SEMI												{ $$ = t_decl_new_var($1, $3, NULL); }
		|	    ids COLON type ASSIGN exp SEMI 									{ $$ = t_decl_new_var($1, $3, $5); }
		|	    ID LPAR RPAR COLON type LCBRACE stms RCBRACE SEMI 				{ $$ = t_decl_new_funct($1, NULL, $5, $7); }
		|	    ID LPAR argdefs RPAR COLON type LCBRACE stms RCBRACE SEMI 		{ $$ = t_decl_new_funct($1, $3, $6, $8); }
		|	    DEFINE ID type SEMI 											{ $$ = t_decl_new_define($2, $3); }
		;

argdefs: 	    argdef															{ $$ = t_argsdef_new($1, NULL); }
		|	    argdef COMMA argdefs											{ $$ = t_argsdef_new($1, $3); }
		;

argdef:  	    ID COLON type													{ $$ = t_argdef_new($1, $3); }
		;

args:		    exp																{ $$ = t_args_new($1, NULL); }
		|	    exp COMMA args													{ $$ = t_args_new($1, $3); }
		;

ids:     	    ID																{ $$ = t_ids_new($1, NULL); }
		|	    ID COMMA ids													{ $$ = t_ids_new($1, $3); }
		;

stms:    	    stm																{ $$ = t_stms_new($1, NULL); }
		|	    stm stms														{ $$ = t_stms_new($1, $2); }
		;

stm:     	    decl															{ $$ = t_stm_new_decl($1); }
		|	    exp SEMI														{ $$ = t_stm_new_exp($1); }
		|	    RETURN exp SEMI													{ $$ = t_stm_new_return($1); }
		|	    IF exp THEN LCBRACE stms RCBRACE SEMI							{ $$ = t_stm_new_ifelse($2, $5, NULL); }
		|	    IF exp THEN LCBRACE stms RCBRACE ELSE LCBRACE stms RCBRACE SEMI { $$ = t_stm_new_ifelse($2, $5, $9); }
		|	    WHILE exp DO LCBRACE stms RCBRACE SEMI							{ $$ = t_stm_new_while($2, $5); }
		|       NEXT															{ $$ = t_stm_new_next(); }
		;

type:    	    T_INT															{ $$ = t_type_new('i'); }
		|	    T_FLOAT															{ $$ = t_type_new('f'); }
		|	    T_STRING														{ $$ = t_type_new('s'); }
		|	    T_BOOL															{ $$ = t_type_new('b'); }
		|	    T_VOID															{ $$ = t_type_new('v'); }
		|	    ID																{ $$ = t_type_new_id($1); }
		|	    type LSBRACE INTLIT RSBRACE										{ $$ = t_type_new($1, $3); }
		;

lit:     	    INTLIT															{ $$ = t_exp_new_intlit($1); }
		|	    FLOATLIT														{ $$ = t_exp_new_floatlit($1); }
		|	    STRLIT															{ $$ = t_exp_new_stringlit($1); }
		|	    BOOLLIT															{ $$ = t_exp_new_boollit($1); }
		;

exp:     	    lit																{ $$ = $1; }
		|	    ID																{ $$ = t_exp_new_id($1); }
		|	    exp LSBRACE INTLIT RSBRACE										{ $$ = t_exp_new_array($1, $3); }
		|	    exp ADD exp														{ $$ = t_exp_new_op("+", $1, $3); }
		|	    exp SUB exp														{ $$ = t_exp_new_op("-", $1, $3); }
		|	    exp MUL exp														{ $$ = t_exp_new_op("*", $1, $3); }
		|	    exp DIV exp														{ $$ = t_exp_new_op("/", $1, $3); }
		|	    exp POW exp														{ $$ = t_exp_new_op("^", $1, $3); }
		|	    exp MOD exp														{ $$ = t_exp_new_op("%", $1, $3); }
		|	    exp GT exp														{ $$ = t_exp_new_op(">", $1, $3); }
		|	    exp LT exp														{ $$ = t_exp_new_op("<", $1, $3); }
		|	    exp GEQ exp														{ $$ = t_exp_new_op(">=", $1, $3); }
		|	    exp LEQ exp														{ $$ = t_exp_new_op("<=", $1, $3); }
		|	    exp EQ exp														{ $$ = t_exp_new_op("==", $1, $3); }
		|	    exp NEQ exp														{ $$ = t_exp_new_op("!=", $1, $3); }
		|	    exp AND exp														{ $$ = t_exp_new_op("&&", $1, $3); }
		|	    exp OR exp														{ $$ = t_exp_new_op("||", $1, $3); }
		|	    NOT exp															{ $$ = t_exp_new_op("!", $1, NULL); }
		|	    SUB exp  %prec NEG												{ $$ = t_exp_new_op("-", $1, NULL); }
		|	    LPAR exp RPAR													{ $$ = t_exp_new_op("( )", $1, NULL); }
		|	    ID LPAR RPAR													{ $$ = t_exp_new_funct($1, NULL); }
		|	    ID LPAR args RPAR												{ $$ = t_exp_new_funct($1, $3); }
		|	    exp ASSIGN exp													{ $$ = t_exp_new_assign($1, $3); }
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
