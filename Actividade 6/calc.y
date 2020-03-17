%{
#include <stdio.h>
#include "calctypes.h"
#include "latex.h"  /* print_prologue(), etc. */

int yylex (void);
void yyerror (char const *);
%}

%union {
    double val;
    char *name;
    calc_t_exp exp;
    calc_t_seq seq;
}

%token     <val>        NUM
%token     <name>       ID

%right                  ASSIGN

%left                   SUB ADD
%left                   MUL DIV
%left                   NEG     /* negation--unary minus */
%token                  LPAR RPAR
%type       <exp>       exp
%type       <seq>       seq

%%
input:      seq         { print_prologue();
                          calc_seq_print($1);
                          print_epilogue(); }
;

seq:            /* empty */ { $$ = calc_seq_new_empty(); }
            | exp NL seq    { $$ = calc_seq_new_exp($1, $3); }
;

exp:            NUM                 { $$ = calc_exp_new_num($1); }
            |   ID                  { $$ = calc_exp_new_id($1); }
            |   exp ADD exp         { $$ =  }
            |   exp SUB exp         { $$ =  }
            |   exp MUL exp         { $$ =  }
            |   exp DIV exp         { $$ =  }
            |   SUB exp %prec NEG   { $$ =  }
            |   LPAR exp RPAR       { $$ = $2; }
            |   ID ASSIGN exp       { $$ =  }
;
%%
void yyerror (char const *s)
{
    fprintf (stderr, "%s\n", s);
}

int main (void)
{
    return yyparse();
}