%{

#include <stdio.h>
#include "yalangapt.h"
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
			
/* Bison decrations.  */
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

%type   <program>       program
%type   <decls>         decls
%type   <decl>          decl
%type   <argdefs>       argdefs
%type   <argdef>        argdef
%type   <args>          args
%type   <ids>           ids
%type   <stms>          stms
%type   <stm>           stm
%type   <type>          type
%type   <lit>           lit
%type   <exp>           exp

%%

program:
                decls                                                                   { print_prologue();
                                                                                        $$ = yalang_decls_print($1);
                                                                                        print_epilogue();
                                                                                        }
        ;

decls:   	decl                                                                    { $$ = yalang_decls_new_single($1); }
        |	decl decls                                                              { $$ = yalang_decls_new($1, $2);   }
        ;

decl:    	ids COLON type SEMI                                                     { $$ = yalang_decl_var_new($1, $3); }
        |	ids COLON type ASSIGN exp SEMI                                          { $$ = yalang_decl_varinit_new( $1, $3, $5); }
        |	ID LPAR RPAR COLON type LCBRACE stms RCBRACE SEMI                       { $$ = yalang_decl_funct_new($1, $5, $7); }
        |	ID LPAR argdefs RPAR COLON type LCBRACE stms RCBRACE SEMI               { $$ = yalang_decl_funct_args_new($1, $3, $6, $8); }
        |	DEFINE ID type SEMI                                                     { $$ = yalang_decl_define_new($2, $3); }
        ;

argdefs:        argdef                                                                  { $$ = yalang_argsdef_new_empty($1); }
        |       argdef COMMA argdefs                                                    { $$ = yalang_argsdef_new($1, $3); }
        ;

argdef:         ID COLON type                                                           { $$ = yalang_argdef_new($1, $3); }
        ;

args:           exp                                                                     { $$ = yalang_args_new_empty($1); }
        |	exp COMMA args                                                          { $$ = yalang_args_new($1, $3); }
        ;

ids:            ID                                                                      { $$ = yalang_ids_new_empty($1); }
        |	ID COMMA ids                                                            { $$ = yalang_ids_new($1, $3);   }
        ;

stms:           stm                                                                     { $$ = yalang_stms_new_empty($1); }
        |	stm stms                                                                { $$ = yalang_stms_new($1, $2); }
        ;

stm:     	decl                                                                    { $$ = yalang_t_stm_decl_new($1); }
        |	exp SEMI                                                                { $$ = yalang_t_stm_exp_new($1); }
        |	RETURN exp SEMI                                                         { $$ = yalang_t_stm_return_new($2); }
        |	IF exp THEN LCBRACE stms RCBRACE SEMI                                   { $$ = yalang_t_stm_if_new($2, $5); }
        |	IF exp THEN LCBRACE stms RCBRACE ELSE LCBRACE stms RCBRACE SEMI         { $$ = yalang_t_stm_ifelse_new($2, $5, $9); }
        |	WHILE exp DO LCBRACE stms RCBRACE SEMI                                  { $$ = yalang_t_stm_while($2, $5); }
        |       NEXT                                                                    { $$ = yalang_t_stm_next(); }
        ;

type:    	T_INT                                                                   { $$ = yalang_type_new('i');         }
        |	T_FLOAT                                                                 { $$ = yalang_type_new('f');         }
        |	T_STRING                                                                { $$ = yalang_type_new('s');         }
        |	T_BOOL                                                                  { $$ = yalang_type_new('b');         }
        |	T_VOID                                                                  { $$ = yalang_type_new('v');         }
        |	ID                                                                      { $$ = yalang_type_new_id($1);       }
        |	type LSBRACE INTLIT RSBRACE                                             { $$ = yalang_type_new_array($1, $3);}
        ;

lit:     	INTLIT                                                                  { $$ = yalang_lit_int_new($1); }
        |	FLOATLIT                                                                { $$ = yalang_lit_float_new($1); }
        |	STRLIT                                                                  { $$ = yalang_lit_string_new($1); }
        |	BOOLLIT                                                                 { $$ = yalang_lit_bool_new($1); }
        ;

exp:     	lit                                                                     { $$ = yalang_exp_new_lit($1); }
        |	ID                                                                      { $$ = yalang_exp_new_id($1); }
        |	exp LSBRACE INTLIT RSBRACE                                              { $$ = yalang_exp_new_arr($1, $3); }
        |	exp ADD exp                                                             { $$ = yalang_exp_new_binop($1, "+", $3); }
        |	exp SUB exp                                                             { $$ = yalang_exp_new_binop($1, "-", $3); }
        |	exp MUL exp                                                             { $$ = yalang_exp_new_binop($1, "*", $3); }
        |	exp DIV exp                                                             { $$ = yalang_exp_new_binop($1, "/", $3); }
        |	exp POW exp                                                             { $$ = yalang_exp_new_binop($1, "^", $3); }
        |	exp MOD exp                                                             { $$ = yalang_exp_new_binop($1, "%", $3); }
        |	exp GT exp                                                              { $$ = yalang_exp_new_binop($1, ">", $3); }
        |	exp LT exp                                                              { $$ = yalang_exp_new_binop($1, "<", $3); }
        |	exp GEQ exp                                                             { $$ = yalang_exp_new_binop($1, ">=", $3); }
        |	exp LEQ exp                                                             { $$ = yalang_exp_new_binop($1, "<=", $3); }
        |	exp EQ exp                                                              { $$ = yalang_exp_new_binop($1, "==", $3); }
        |	exp NEQ exp                                                             { $$ = yalang_exp_new_binop($1, "!=", $3); }
        |	exp AND exp                                                             { $$ = yalang_exp_new_binop($1, "&&", $3); }
        |	exp OR exp                                                              { $$ = yalang_exp_new_binop($1, "||", $3); }
        |	NOT exp                                                                 { $$ = yalang_exp_new_unop($2, "!"); }
        |	SUB exp  %prec NEG                                                      { $$ = yalang_exp_new_unop($2, "-"); }
        |	LPAR exp RPAR                                                           { $$ = yalang_exp_new_unop($2, "()"); }
        |	ID LPAR RPAR                                                            { $$ = yalang_exp_new_funct_call($1); }
        |	ID LPAR args RPAR                                                       { $$ = yalang_exp_new_funct_call_args($1, $3); }
        |	exp ASSIGN exp                                                          { $$ = yalang_exp_new_assign($1, $3); }
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
