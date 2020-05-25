#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include "yalangapt.h"
#include <string.h>

typedef struct st_bucket_ *ST_Bucket;
typedef struct st_ *ST;


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
        t_decl decl;
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

        struct {
            t_args args;
        } print;
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
    enum { T_INT, T_FLOAT, T_STRING, T_BOOL, T_VOID, T_ID, T_ARRAY } kind;

    char *id;
    int size;
    t_type array_type;
};

#endif