#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "yalang_apt.h"

struct yalang_t_program_ 
{
    enum {EMPTY, NOT_EMPTY} kind;

    yalang_t_decls d;    
};

struct yalang_t_decls_
{
    enum {EMPTY, NOT_EMPTY} kind;

    yalang_t_decl d;
    yalang_t_decls ds;
};

struct yalang_t_decl_
{
    enum {VAR, VARINIT, FUNCT, FUNCT_ARGS, DEFINE } kind;

    union {
        yalang_t_decl_var v;
        yalang_t_decl_varinit vi;
        yalang_t_decl_funct f;
        yalang_t_decl_define d;
    } u;
};

struct yalang_t_decl_var_
{
    yalang_t_ids i;
    yalang_t_type t;
};

struct yalang_t_decl_varinit_
{
    yalang_t_ids i;
    yalang_t_type t;
    yalang_t_exp e;
};

struct yalang_t_decl_funct_
{
    enum { EMPTY, ARGS } kind;

    yalang_t_funct_head h;
    yalang_t_stm s;
};

struct yalang_t_decl_define_
{
    char *id;
    yalang_t_type t;
};

struct yalang_t_funct_head_
{
    enum { EMPTY, ARGS } kind;

    char *id;
    yalang_t_type t;
    yalang_t_argsdef a;
};

struct yalang_t_argdef_
{
    char *id;
    yalang_t_type t;
};

struct yalang_t_argsdef_
{
    yalang_t_argdef a;
    yalang_t_argsdef as;
};

typedef struct yalang_t_args_
{
    enum { EMPTY, MULT } kind;

    yalang_t_exp e;
    yalang_t_args a;
};

struct yalang_t_stms_
{
    enum { EMPTY, MULT } kind;
    
    yalang_t_stm s;
    yalang_t_stms ns;
};

struct yalang_t_stm_
{
    enum { DECL, EXP, RET, IF, IFELSE, WHILE, NEXT } kind;

    union{
        yalang_t_decl d;
        yalang_t_exp e;
        struct{
            yalang_t_exp e;
            yalang_t_stm s;
        } _iw;
        struct{
            yalang_t_exp e;
            yalang_t_stm s1;
            yalang_t_stm s2;
        } _ifelse;
    } u;
};

struct yalang_t_ids_
{
    enum { EMPTY, MULT } kind;
    
    char *id;
    yalang_t_ids ids;
};

struct yalang_t_type_
{
    enum {INT, FLOAT, STRING, BOOL, VOID, ID, ARRAY} kind;

    char *id;
    int size;
    yalang_t_type arr_t;
};

struct yalang_t_exp_
{
    enum {EXP_NUM, EXP_ID, EXP_BINOP, EXP_UNOP, EXP_ASSIGN} kind;

    union {
        int num;
        char *id;
        struct {
            char op[OP_LIM];
            yalang_t_exp arg1;
            yalang_t_exp arg2;
        }binop;
        struct {
            char op[OP_LIM];
            yalang_t_exp arg;
        }unop;
        struct {
            char *id;
            yalang_t_exp rvalue;
        }assign;
    } u;
};

struct yalang_t_lit_;

/*********************************************************************/

yalang_t_program yalang_program_empty()
{
    yalang_t_program ret = (yalang_t_program) malloc (sizeof (*ret));

    ret->kind = EMPTY;
    ret->d = NULL;

    return ret;
}

yalang_t_program yalang_program_new(yalang_t_decls ds)
{
    yalang_t_program ret = (yalang_t_program) malloc (sizeof (*ret));

    ret->kind = MULT;
    ret->d = ds;

    return ret;
}

/*********************************************************************/

yalang_t_decls yalang_decls_empty(yalang_t_decl d)
{
    yalang_t_decls ret = (yalang_t_decls) malloc (sizeof (*ret));

    ret->kind = EMPTY;
    ret->d = d;
    ret->ds = NULL;

    return ret;
}

yalang_t_decls yalang_decls_new(yalang_t_decl d, yalang_t_decls ds)
{
    yalang_t_decls ret = (yalang_t_decls) malloc (sizeof (*ret));

    ret->kind = MULT;
    ret->d = d;
    ret->ds = ds;

    return ret;
}

/*********************************************************************/

yalang_t_decl yalang_decl_new()
{
    yalang_t_decl ret = (yalang_t_decl) malloc (sizeof (*ret));

    return ret;
}

/*********************************************************************/

yalang_t_decl yalang_decl_var_new(yalang_t_ids i, yalang_t_type t)
{
    yalang_t_decl_var aux = (yalang_t_decl_var) malloc (sizeof (*aux));

    aux->i = i;
    aux->t = t;

    yalang_t_decl ret = yalang_decl_new();
    ret->kind = VAR;
    ret->u.v = aux;

    return ret;
}

yalang_t_decl yalang_decl_varinit_new(yalang_t_ids i, yalang_t_type t,
	yalang_t_exp e)
{
    yalang_t_decl_varinit aux = (yalang_t_decl_varinit) malloc (sizeof (*aux));

    aux->i = i;
    aux->t = t;
    aux->e = e;

    yalang_t_decl ret = yalang_decl_new();
    ret->kind = VARINIT;
    ret->u.vi = aux;

    return ret;
}   

yalang_t_decl yalang_decl_funct_new(char *id, yalang_t_type ret_t,
	yalang_t_stms s)
{
    yalang_t_decl_funct aux = (yalang_t_decl_funct) malloc (sizeof (*aux));
    yalang_t_funct_head head = yalang_funct_head_new(id, ret_t);

    head->kind = EMPTY;
    head->a = NULL;

    aux->kind = EMPTY;
    aux->h = head;
    aux->s = s;

    yalang_t_decl ret = yalang_decl_new();
    ret->kind = FUNCT;
    ret->u.f = aux;

    return ret;
}

yalang_t_decl yalang_decl_funct_args_new(char *id, yalang_t_type ret_t,
	yalang_t_argsdef a, yalang_t_stms s)
{
    yalang_t_decl_funct aux = (yalang_t_decl_funct) malloc (sizeof (*aux));
    yalang_t_funct_head head = yalang_funct_head_new(id, ret_t);

    head->kind = MULT;
    head->a = a;

    aux->kind = MULT;
    aux->h = head;
    aux->s = s;

    yalang_t_decl ret = yalang_decl_new();
    ret->kind = FUNCT;
    ret->u.f = aux;

    return ret;
}

yalang_t_decl yalang_decl_define_new(char *id, yalang_t_type t)
{
    yalang_t_decl_define aux = (yalang_t_decl_define) malloc (sizeof (*aux));

    aux->id = id;
    aux->t = t;

    yalang_t_decl ret = yalang_decl_new();
    ret->kind = DEFINE;
    ret->u.d = aux;

    return ret;
}

/*********************************************************************/

yalang_t_funct_head yalang_funct_head_new(char *id, yalang_t_type ret_t)
{
    yalang_t_funct_head ret = (yalang_t_funct_head) malloc (sizeof (*ret));

    ret->id = id;
    ret->t = ret_t;

    return ret;
}

/*********************************************************************/

yalang_t_argdef yalang_argdef_new(char *id, yalang_t_type t)
{
    yalang_t_argdef ret = (yalang_t_argdef) malloc (sizeof (*ret));

    ret->id = id;
    ret->t = t;

    return ret;
}

yalang_t_argsdef yalang_argsdef_new(yalang_t_argdef a,
	yalang_t_argsdef as)
{
    yalang_t_argsdef ret = (yalang_t_argsdef) malloc (sizeof (*ret));

    ret->a = a;
    ret->as = as;

    return ret;
}

/*********************************************************************/

yalang_t_args yalang_args_new_empty(yalang_t_exp e)
{
    yalang_t_args ret = (yalang_t_args) malloc (sizeof (*ret));

    ret->kind = EMPTY;
    ret->e = e;
    ret->a = NULL;

    return ret;
}

yalang_t_args yalang_args_new(yalang_t_exp e, yalang_t_args a)
{
    yalang_t_args ret = (yalang_t_args) malloc (sizeof (*ret));

    ret->kind = MULT;
    ret->e = e;
    ret->a = a;

    return ret;
}

/*********************************************************************/

yalang_t_stms yalang_stms_new_empty(yalang_t_stm s)
{
    yalang_t_stms ret = (yalang_t_stms) malloc (sizeof (*ret));

    ret->kind = EMPTY;
    ret->s = s;
    ret->ns = NULL;

    return ret;
}

yalang_t_stms yalang_stms_new(yalang_t_stm s, yalang_t_stms ss)
{
    yalang_t_stms ret = (yalang_t_stms) malloc (sizeof (*ret));

    ret->kind = MULT;
    ret->s = s;
    ret->ns = ss;

    return ret;
}

/*********************************************************************/

yalang_t_stm yalang_stm_new()
{
    yalang_t_stm ret = (yalang_t_stm) malloc (sizeof (*ret));

    return ret;
}

/********************************/

yalang_t_stm yalang_t_stm_decl_new(yalang_t_decl d)
{
    yalang_t_stm ret = yalang_stm_new();

    ret->kind = DECL;
    ret->u.d = d;

    return ret;
}

yalang_t_stm yalang_t_stm_exp_new(yalang_t_exp e)
{
    yalang_t_stm ret = yalang_stm_new();

    ret->kind = EXP;
    ret->u.d = e;

    return ret;
}

yalang_t_stm yalang_t_stm_return_new(yalang_t_exp e)
{
    yalang_t_stm ret = yalang_stm_new();

    ret->kind = RET;
    ret->u.e = e;

    return ret;
}

yalang_t_stm yalang_t_stm_if_new(yalang_t_exp e, yalang_t_stms s)
{
    yalang_t_stm ret = yalang_stm_new();

    ret->kind = IF;
    ret->u._iw.e = e;
    ret->u._iw.s = s;

    return ret;
}

yalang_t_stm yalang_t_stm_ifelse_new(yalang_t_exp e, yalang_t_stms s1,
    yalang_t_stms s2)
{
    yalang_t_stm ret = yalang_stm_new();

    ret->kind = IFELSE;
    ret->u._ifelse.e = e;
    ret->u._ifelse.s1 = s1;
    ret->u._ifelse.s2 = s2;

    return ret;
}

yalang_t_stm yalang_t_stm_while(yalang_t_exp e, yalang_t_stms s)
{
    yalang_t_stm ret = yalang_stm_new();

    ret->kind = WHILE;
    ret->u._iw.e = e;
    ret->u._iw.s = s;

    return ret;
}


/*********************************************************************/

yalang_t_ids yalang_ids_new_empty(char *id)
{
    yalang_t_ids ret = (yalang_t_ids) malloc (sizeof (*ret));

    ret->kind = EMPTY;
    ret->id = id;
    ret->ids = NULL;

    return ret;
}

yalang_t_ids yalang_ids_new(char *id, yalang_t_ids ids)
{
    yalang_t_ids ret = (yalang_t_ids) malloc (sizeof (*ret));

    ret->kind = MULT;
    ret->id = id;
    ret->ids = ids;

    return ret;
}

/*********************************************************************/

yalang_t_type yalang_type_new(char c)
{
    yalang_t_type ret = (yalang_t_type) malloc (sizeof (*ret));

    bool isArr = false;

    switch(c){
        case 'i':
        ret->kind = INT;
        break;

        case 'f':
        ret->kind = FLOAT;
        break;

        case 's':
        ret->kind = STRING;
        break;

        case 'b':
        ret->kind = BOOL;
        break;

        case 'v':
        ret->kind = VOID;
        break;

        default:
        printf("ERROR: wrong type detected!\n");
        exit(1);
        break;
    }

    ret->size = -1;
    ret->id = NULL;
    ret->arr_t = NULL;
    return ret;
}

yalang_t_type yalang_type_new_id(char *id)
{
    yalang_t_type ret = (yalang_t_type) malloc (sizeof (*ret));

    ret->kind = ID;
    ret->size = -1;
    ret->id = id;

    return ret;
}

yalang_t_type yalang_type_new_array(yalang_t_type t, int s)
{
    yalang_t_type ret = (yalang_t_type) malloc (sizeof (*ret));

    ret->kind = ARRAY;
    ret->size = s;
    ret->id = NULL;
    ret->arr_t = t;

    return ret;
}

/*********************************************************************/
