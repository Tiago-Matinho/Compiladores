#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "yalangapt.h"

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
            t_stm stm;
        } _if;

        struct {
            t_exp exp;
            t_stm stm1;
            t_stm stm2;
        } _ifelse;

        struct {
            t_exp exp;
            t_stm stm;
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
            float float_val;
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
    enum { T_INT, T_FLOAT, T_STRING, T_BOOL, T_VOID, T_ID, T_ARRAY } kind;

    char *id;
    int size;
    t_type array_type;
};


/*********************************************************************/


t_decls t_decls_new(t_decl d, t_decls ds)
{
    t_decls ret = (t_decl) malloc( sizeof(*ret));

    if(ds == NULL)
        ret->kind = DECLS_SINGLE;
    else
        ret->kind = DECLS_LIST;

    ret->d = d;
    ret->ds = ds;

    return ret;
}

t_decl t_decl_new_var(t_ids ids, t_type type, t_exp exp)
{
    t_decl ret = (t_decl) malloc( sizeof(*ret));

    if(exp == NULL)
    {
        ret->kind = DECL_VAR;
        ret->u.var.ids = ids;
        ret->u.var.type = type;
    }
    
    else
    {
        ret->kind = DECL_VARINIT;
        ret->u.varinit.ids = ids;
        ret->u.varinit.type = type;
        ret->u.varinit.value = exp;
    }

    return ret;
}

t_decl t_decl_new_funct(char *id, t_argsdef args, t_type type, t_stms stms)
{
    t_decl ret = (t_decl) malloc( sizeof(*ret));

    if(args == NULL)
        ret->kind = DECL_FUNCT_VOID;
    else
        ret->kind = DECL_FUNCT;
    
    ret->u.funct.id = id;
    ret->u.funct.type = type;
    ret->u.funct.args = args;
    ret->u.funct.stms = stms;

    return ret;
}

t_decl t_decl_new_define(char *id, t_type type)
{
    t_decl ret = (t_decl) malloc( sizeof(*ret));

    ret->kind = DECL_DEFINE;

    ret->u.define.id = id;
    ret->u.define.type = type;

    return ret;
}

t_stms t_stms_new(t_stm s, t_stms ss)
{
    t_stms ret = (t_stms) malloc( sizeof(*ret));

    if(ss == NULL)
        ret->kind = STMS_SINGLE;
    else
        ret->kind = STMS_LIST;

    ret->s = s;
    ret->ss = ss;

    return ret;
}


t_stm t_stm_new_decl(t_decl decl)
{
    t_stm ret = (t_stm) malloc( sizeof(*ret));

    ret->u.decl = decl;

    return ret;
}

t_stm t_stm_new_exp(t_exp exp)
{
    t_stm ret = (t_stm) malloc( sizeof(*ret));

    ret->u.exp = exp;

    return ret;
}

t_stm t_stm_new_ifelse(t_exp exp, t_stm stm1, t_stm stm2)
{
    t_stm ret = (t_stm) malloc( sizeof(*ret));

    if(stm2 == NULL)
    {
        ret->kind = STM_IF;
        ret->u._if.exp = exp;
        ret->u._if.stm = stm1;
    }
    else
    {
        ret->kind = STM_IFELSE;
        ret->u._ifelse.exp = exp;
        ret->u._ifelse.stm1 = stm1;
        ret->u._ifelse.stm2 = stm2;
    }

    return ret;
}

t_stm t_stm_new_while(t_exp exp, t_stm stm)
{
    t_stm ret = (t_stm) malloc( sizeof(*ret));

    ret->kind = STM_WHILE;
    ret->u._while.exp = exp;
    ret->u._while.stm = stm;

    return ret;
}

t_stm t_stm_new_return(t_exp exp)
{
    t_stm ret = (t_stm) malloc( sizeof(*ret));

    ret->kind = STM_RET;

    ret->u.exp = exp;

    return ret;
}

t_stm t_stm_new_next()
{
    t_stm ret = (t_stm) malloc( sizeof(*ret));

    ret->kind = STM_NEXT;

    return ret;
}


t_exp t_exp_new_intlit(int i)
{
    t_exp ret = (t_exp) malloc( sizeof(*ret));

    ret->kind = EXP_INTLIT;
    ret->u.lit.int_val = i;

    return ret;
}

t_exp t_exp_new_floatlit(float f)
{
    t_exp ret = (t_exp) malloc( sizeof(*ret));

    ret->kind = EXP_FLOATLIT;
    ret->u.lit.float_val = f;

    return ret;
}

t_exp t_exp_new_stringlit(char *s)
{
    t_exp ret = (t_exp) malloc( sizeof(*ret));

    ret->kind = EXP_STRINGLIT;
    ret->u.lit.string_val = s;

    return ret;
}

t_exp t_exp_new_boollit(bool b)
{
    t_exp ret = (t_exp) malloc( sizeof(*ret));

    ret->kind = EXP_BOOLLIT;
    ret->u.lit.bool_val = b;

    return ret;
}

t_exp t_exp_new_id(char *id)
{
    t_exp ret = (t_exp) malloc( sizeof(*ret));

    ret->kind = EXP_ID;
    ret->u.id = id;
    
    return ret;
}

t_exp t_exp_new_op(char op[OP_LIM], t_exp exp1, t_exp exp2)
{
    t_exp ret = (t_exp) malloc( sizeof(*ret));

    if(exp2 == NULL)
    {
        ret->kind = EXP_UNOP;
        memset(ret->u.unop.op, '\0', OP_LIM);
        strcpy(ret->u.unop.op, op);
        ret->u.unop.exp = exp1;
    }
    else
    {
        ret->kind = EXP_BINOP;
        memset(ret->u.binop.op, '\0', OP_LIM);
        strcpy(ret->u.binop.op, op);
        ret->u.binop.exp1 = exp1;
        ret->u.binop.exp2 = exp2;
    }
    
    return ret;
}

t_exp t_exp_new_assign(t_exp exp1, t_exp exp2)
{
    t_exp ret = (t_exp) malloc( sizeof(*ret));

    ret->kind = EXP_ASSIGN;
    ret->u.assign.exp1 = exp1;
    ret->u.assign.exp2 = exp2;

    return ret;
}

t_exp t_exp_new_array(t_exp exp, int pos)
{
    t_exp ret = (t_exp) malloc( sizeof(*ret));

    ret->kind = EXP_ARRAY;
    ret->u.array.exp = exp;
    ret->u.array.pos = pos;

    return ret;
}

t_exp t_exp_new_funct(char *id, t_args args)
{
    t_exp ret = (t_exp) malloc( sizeof(*ret));

    ret->kind = EXP_FUNCT;
    ret->u.funct.id = id;
    ret->u.funct.args = args;

    return ret;
}

t_argsdef t_argsdef_new(t_argdef a, t_argsdef as)
{
    t_argsdef ret = (t_argsdef) malloc( sizeof(*ret));

    if(as == NULL)
        ret->kind = ARGS_SINGLE;
    else
        ret->kind = ARGS_LIST;

    ret->a = a;
    ret->as = as;

    return ret;
}

t_argdef t_argdef_new(char *id, t_type type)
{
    t_argdef ret = (t_argdef) malloc( sizeof(*ret));

    ret->id = id;
    ret->type = type;

    return ret;
}

t_args t_args_new(t_exp exp, t_args a)
{
    t_args ret = malloc( sizeof(*ret));

    ret->exp = exp;
    ret->as = a;

    return ret;
}

t_ids t_ids_new(char *id, t_ids ids)
{
    t_ids ret = (t_ids) malloc( sizeof(*ret));

    if(ids == NULL)
        ret->kind = IDS_SINGLE;
    else
        ret->kind = IDS_LIST;

    ret->id = id;
    ret->ids = ids;

    return ret;
}

t_type t_type_new(char c)
{
    t_type ret = (t_type) malloc( sizeof(*ret));

    ret->size = -1;
    ret->array_type = NULL;
    ret->id = NULL;

    switch(c) {
    case 'i':
        ret->kind = T_INT;
        break;

    case 'f':
        ret->kind = T_FLOAT;
        break;

    case 's':
        ret->kind = T_STRING;
        break;

    case 'b':
        ret->kind = T_BOOL;
        break;
    
    default:
        ret->kind = T_VOID;
        break;
    }

    return ret;
}

t_type t_type_new_id(char *id)
{
    t_type ret = (t_type) malloc( sizeof(*ret));

    ret->kind = T_ID;
    ret->array_type = NULL;
    ret->id = id;
    ret->size = -1;

    return ret;
}

t_type t_type_new_array(t_type type, int size)
{
    t_type ret = (t_type) malloc( sizeof(*ret));

    ret->kind = T_ARRAY;
    ret->array_type = type;
    ret->size = size;
    ret->id = NULL;

    return ret;
}
