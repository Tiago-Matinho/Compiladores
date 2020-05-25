#ifndef YALANGAPT_H_
#define YALANGAPT_H_

#include <stdlib.h>
#include <stdbool.h>
#define OP_LIM 3

// t_decls
struct yalang_t_decls_
{
    enum { DECLS_SINGLE, DECLS_LIST } kind;

    t_decl d;
    t_decls ds;
};

// t_decl
struct yalang_t_decl_
{
    enum { DECL_VAR, DECL_VARINIT, DECL_FUNCT_VOID, DECL_FUNCT, DECL_DEFINE } kind;

    union {
        struct {
            t_ids ids;
            t_type type;
        } var;

        struct {
            t_ids ids;
            t_type type;
            t_exp value;
        } varinit;

        struct {
            char *id;
            t_type type;
            t_argsdef args;
            t_stms stms;
        } funct;

        struct {
            char *id;
            t_type type;
        } define;

    } u;
};

// t_stms
struct yalang_t_stms_
{
    enum { STMS_SINGLE, STMS_LIST } kind;
    
    t_stm s;
    t_stms ss;
};

// t_stm
struct yalang_t_stm_
{
    enum { STM_DECL, STM_EXP, STM_RET, STM_IF, STM_IFELSE, STM_WHILE, STM_NEXT } kind;

    union{
        t_decls decls;
        t_exp exp;
        struct {
            t_exp exp;
            t_stms stms;
        } _if;

        struct {
            t_exp exp;
            t_stms stms1;
            t_stms stms2;
        } _ifelse;

        struct {
            t_exp exp;
            t_stms stms;
        } _while;

    } u;
};

// t_exp
struct yalang_t_exp_
{
    enum { EXP_INTLIT, EXP_FLOATLIT, EXP_STRINGLIT, EXP_BOOLLIT, EXP_ID, EXP_ARRAY,
        EXP_BINOP, EXP_UNOP, EXP_ASSIGN, EXP_FUNCT } kind;

    union {
        union {
            int int_val;
            double float_val;
            char *string_val;
            bool bool_val;
        } lit;

        char *id;

        struct {
            t_exp exp;
            int pos;
        } array;

        struct {
            t_exp exp1;
            t_exp exp2;
            char op[OP_LIM];
        } binop;

        struct {
            t_exp exp;
            char op[OP_LIM];
        } unop;

        struct {
            t_exp exp1;
            t_exp exp2;
        } assign;

        struct {
            char *id;
            t_args args;
        } funct;
    } u;
};

// t_argsdef
struct yalang_t_argsdef_
{
    enum { ARGS_SINGLE, ARGS_LIST } kind;

    t_argdef a;
    t_argsdef as;
};

// t_argdef
struct yalang_t_argdef_
{
    char *id;
    t_type type;
};

// t_args
struct yalang_t_args_
{
    t_exp exp;
    t_args as;
};

// t_ids
struct yalang_t_ids_
{
    enum { IDS_SINGLE, IDS_LIST } kind;
    
    char *id;
    t_ids ids;
};

// t_type
struct yalang_t_type_
{
    enum { T_INT, T_FLOAT, T_STRING, T_BOOL, T_VOID, T_ID, T_ARRAY, T_ANY } kind;

    char *id;
    int size;
    t_type array_type;
};


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

t_stm t_stm_new_decls(t_decls decls);
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


void t_decls_print(t_decls this);

#endif
