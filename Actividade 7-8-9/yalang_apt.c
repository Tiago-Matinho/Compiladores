#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "yalang_apt.h"

struct yalang_t_decls_
{
	enum {DECLS_EMPTY, DECLS_MUL} kind;

	union {
        yalang_t_decl d;
		struct {
		    yalang_t_decl d;
		    yalang_t_decls s;
		} b;
	} u;
};

struct yalang_t_decl_
{
    enum {DECL_VAR, DECL_VARINIT, DECL_FUNC, DECL_FUNC_ARGS, DECL_DEFINE} kind;

	union {
		yalang_t_vars vars;
        yalang_t_varsinit varsinit;
        struct {
            yalang_t_func_head h;
        } func;
	} u;
};

/*************************************/

struct yalang_t_vars_
{
    yalang_t_ids ids; 
    yalang_t_type type;
    yalang_t_vars vars;
};

struct yalang_t_varsinit_
{
    yalang_t_vars vars;
    yalang_t_value val;
};

struct yalang_t_func_head_
{
    enum {VOID, ARGS} kind;

    yalang_t_vars args;
    yalang_t_type return_t;
};

/*************************************/

struct yalang_t_ids_
{
    enum {UNI, MUL} kind;
    
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

struct yalang_t_value_
{
    enum {INT, FLOAT, STRING, BOOL, VOID, ID} kind;

    union {
        int i;
        float f;
        char *string;
        bool b;
        char *id;
    } u;
};

/*********************************************************************/

yalang_t_decls yalang_decls_new_empty(yalang_t_decl d)
{
    yalang_t_decls ret = (yalang_t_decls) malloc (sizeof (*ret));

  	ret->kind = DECLS_EMPTY;
	ret->u.d = d;

  	return ret;
}

yalang_t_decls yalang_decls_new(yalang_t_decl d, yalang_t_decls ds)
{
    yalang_t_decls ret = (yalang_t_decls) malloc (sizeof (*ret));

  	ret->kind = DECLS_MUL;
	ret->u.b.d = d;
    ret->u.b.s = ds;

  	return ret;
}

/*********************************************************************/

yalang_t_decl yalang_decl_new_var(yalang_t_vars v)
{
    yalang_t_decl ret = (yalang_t_decl) malloc (sizeof (*ret));

    ret->kind = DECL_VAR;
    ret->u.vars = v;

    return ret;
}

yalang_t_decl yalang_decl_new_varinit(yalang_t_varsinit v)
{
    yalang_t_decl ret = (yalang_t_decl) malloc (sizeof (*ret));

    ret->kind = DECL_VARINIT;
    ret->u.varsinit = v;

    return ret;
}

yalang_t_decl yalang_decl_new_func()
{

}
// TODO Add the rest

/*********************************************************************/

yalang_t_vars yalang_vars_new(yalang_t_ids ids, yalang_t_type t)
{
    yalang_t_vars ret = (yalang_t_vars) malloc (sizeof (*ret));

    ret->ids = ids;
    ret->type = t;

    return ret;
}

yalang_t_varinit yalang_varinit_new(yalang_t_var var,
    yalang_t_value val)
{
    yalang_t_varinit ret = (yalang_t_varinit) malloc (sizeof (*ret));

    ret->v = var;
    ret->val = val;

    return ret;
}

/*********************************************************************/

// TODO Test
// Funciona como uma lista ligada
yalang_t_ids yalang_ids_new_empty(char *id)
{
    yalang_t_ids ret = (yalang_t_ids) malloc (sizeof (*ret));

    ret->kind = UNI;
    ret->id = id;
    ret->ids = NULL;

    return ret;
}

yalang_t_ids yalang_ids_new_(char *id, yalang_t_ids ids)
{
    yalang_t_ids ret = (yalang_t_ids) malloc (sizeof (*ret));

    ret->kind = MUL;
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
