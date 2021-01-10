%{
#include <stdio.h>
#include <string.h>
#define MAX_VARS 50
int yylex (void);
void yyerror (char const *);

struct {
    char *var;
    double val;
} array[MAX_VARS];
int array_c = -1;

void set_var (char *var, double val) {
    array[++array_c].var = var;
    array[array_c].val = val;
}

double get_var (char *var) {
    for (int i = 0; i < MAX_VARS; i++) {
        if (strcmp(array[i].var, var) == 0)
            return array[i].val;
    }
    return 0;
}

%}

%union {
    double val;
    char *id;
}

/* Bison declarations.  Typed (with value) and untyped tokens */
%token  <val>           INTLIT  FLOATLIT
%token  <id>            ID
%token                  NL  /* newline */

/* Precedence - the (vertical) order matters! */
%right                  ASSIGN
%left                   SUB ADD
%left                   MUL DIV
%left                   NEG     /* negation--unary minus */
%token                  LPAR RPAR

/* Types for the grammar rules */
%type   <val>           line exp
%%

input:      /* empty */
        |   line input
;

line:       NL      { $$ = 0; }
        |   exp NL  { $$ = $1; printf ("%.2lf\n", $$); }
;

exp:        INTLIT              { $$ = $1; }
        |   FLOATLIT            { $$ = $1; }
        |   ID                  { $$ = get_var($1); }
        |   ID ASSIGN exp       { $$ = $3; set_var($1, $3); }
        |   exp ADD exp         { $$ = $1 + $3; }
        |   exp SUB exp         { $$ = $1 - $3; }
        |   exp MUL exp         { $$ = $1 * $3; }
        |   exp DIV exp         { $$ = $1 / $3; }
        |   SUB exp %prec NEG   { $$ = (0 - $2); }
        |   LPAR exp RPAR       { $$ = $2; }


%%

/* Called by yyparse on error. */
void yyerror (char const *s)
{
    fprintf (stderr, "%s\n", s);
}

int main (void)
{
    return yyparse();
}