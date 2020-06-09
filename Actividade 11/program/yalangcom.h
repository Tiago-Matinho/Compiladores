#ifndef YALANGCOM_H_
#define YALANGCOM_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define OP_LIM 3
#define MAX_CONTEXT 53


/*********************************************************************/


typedef struct yalang_t_decls_ *t_decls;
typedef struct yalang_t_decl_ *t_decl;
typedef struct yalang_t_stms_ *t_stms;
typedef struct yalang_t_stm_ *t_stm;
typedef struct yalang_t_exp_ *t_exp;
typedef struct yalang_t_argsdef_ *t_argsdef;
typedef struct yalang_t_argdef_ *t_argdef;
typedef struct yalang_t_args_ *t_args;
typedef struct yalang_t_ids_ *t_ids;
typedef struct yalang_t_type_ *t_type;


/*********************************************************************/


t_decls t_decls_new(t_decl d, t_decls ds);

t_decl t_decl_new_var(t_ids ids, t_type type, t_exp exp);
t_decl t_decl_new_funct(char *id, t_argsdef args, t_type type, t_stms stms);
t_decl t_decl_new_define(char *id, t_type type);

t_stms t_stms_new(t_stm s, t_stms ss);

t_stm t_stm_new_decl(t_decl decl);
t_stm t_stm_new_exp(t_exp exp);
t_stm t_stm_new_ifelse(t_exp exp, t_stms stms1, t_stms stms2);
t_stm t_stm_new_while(t_exp exp, t_stms stms);
t_stm t_stm_new_return(t_exp exp);
t_stm t_stm_new_next();

t_exp t_exp_new_intlit(int i);
t_exp t_exp_new_floatlit(double f);
t_exp t_exp_new_stringlit(char *s);
t_exp t_exp_new_boollit(bool b);
t_exp t_exp_new_id(char *id);
t_exp t_exp_new_op(char op[OP_LIM], t_exp exp1, t_exp exp2);
t_exp t_exp_new_assign(t_exp exp1, t_exp exp2);
t_exp t_exp_new_array(t_exp exp, int pos);
t_exp t_exp_new_funct(char *id, t_args args);
t_exp t_exp_new_print(t_args args);
t_exp t_exp_new_input(char *id);

t_argsdef t_argsdef_new(t_argdef a, t_argsdef as);

t_argdef t_argdef_new(char *id, t_type type);

t_args t_args_new(t_exp exp, t_args a);

t_ids t_ids_new(char *id, t_ids ids);

t_type t_type_new(char c);
t_type t_type_new_id(char *id);
t_type t_type_new_array(t_type type, int size);


/*********************************************************************/


void t_decls_print(FILE* fp, t_decls this);
void t_decl_print(FILE* fp, t_decl this);
void t_stms_print(FILE* fp, t_stms this);
void t_stm_print(FILE* fp, t_stm this);
void t_exp_print(FILE* fp, t_exp this);
void t_argsdef_print(FILE* fp, t_argsdef this);
void t_argdef_print(FILE* fp, t_argdef this);
void t_args_print(FILE* fp, t_args this);
void t_ids_print(FILE* fp, t_ids this);
void t_type_print(FILE* fp, t_type this);


/*********************************************************************/


typedef struct st_bucket_ *ST_Bucket;
typedef struct st_ *ST;

ST st_new();
ST_Bucket st_bucket_new_print();
ST_Bucket st_bucket_new_input();

void st_insert(ST_Bucket new, ST st);
void st_new_scope(ST st);
void st_drop_scope(ST st);

void t_decls_ant(t_decls node, ST st);


/*********************************************************************/


void t_decls_ant(t_decls node, ST st);
void t_decl_ant(t_decl node, ST st);
void t_stms_ant(t_stms node, ST st);
void t_stm_ant(t_stm node, ST st);
t_type t_exp_ant(t_exp node, ST st);


/*********************************************************************/


typedef struct frame_ *frame;



/*********************************************************************/


void t_decls_codegen(FILE* fp, t_decls node, ST st);
void t_decl_codegen(FILE* fp, t_decl node, ST st);
void t_stms_codegen(FILE* fp, t_stms node, ST st);
void t_stm_codegen(FILE* fp, t_stm node, ST st);
void t_exp_codegen(FILE* fp, t_exp node, ST st);
void t_argsdef_codegen(FILE* fp, t_argsdef node, ST st);
void t_argdef_codegen(FILE* fp, t_argdef node, ST st);
void t_args_codegen(FILE* fp, t_args node, ST st);
void t_ids_codegen(FILE* fp, t_ids node, ST st);
void t_type_codegen(FILE* fp, t_type node, ST st);


#endif
