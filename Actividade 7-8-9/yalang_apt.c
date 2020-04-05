#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "yalang_apt.h"

struct yalang_t_decls_
{
	enum {DECLS_EMPTY, DECLS_BIN} kind;

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
    enum {DECL_VAR, DECL_VARINIT, DECL_FUNC_VOID, DECL_FUNC_ARGS, DECL_DEFINE} kind;

	union {
		yalang_t_var var;
	} u;
};

struct yalang_t_var_
{
    yalang_t_ids ids; 
    yalang_t_type type;
    yalang_t_value val;
};

struct yalang_t_varinit_
{
    yalang_t_var v;
    yalang_t_value val;
};

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

yalang_t_decls yalang_decls_new_(yalang_t_decl d, yalang_t_decls ds)
{
    yalang_t_decls ret = (yalang_t_decls) malloc (sizeof (*ret));

  	ret->kind = DECLS_BIN;
	ret->u.b.d = d;
    ret->u.b.s = ds;

  	return ret;
}

/*********************************************************************/

yalang_t_decl yalang_decl_new_var(yalang_t_var v)
{
    yalang_t_decl ret = (yalang_t_decl) malloc (sizeof (*ret));

    ret->kind = DECL_VAR;
    ret->u.var = v;

    return ret;
}

// TODO Add the rest

/*********************************************************************/

yalang_t_var yalang_var_new(yalang_t_ids ids, yalang_t_type t)
{
    yalang_t_var ret = (yalang_t_var) malloc (sizeof (*ret));

    ret->ids = ids;
    ret->type = t;
    ret->val = NULL;

    return ret;
}

yalang_t_varinit yalang_varinit_new(yalang_t_var var,
    yalang_t_value val)
{
    yalang_t_var ret = (yalang_t_var) malloc (sizeof (*ret));

    ret->ids = ids;
    ret->type = t;
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

yalang_t_type yalang_type_new_int()
{
    yalang_t_type ret = (yalang_t_type) malloc (sizeof (*ret));

    ret->kind = INT;
    ret->size = -1;
    ret->id = NULL;
    ret->arr_t = NULL;

    return ret;
}

yalang_t_type yalang_type_new_float()
{
    yalang_t_type ret = (yalang_t_type) malloc (sizeof (*ret));

    ret->kind = FLOAT;
    ret->size = -1;
    ret->id = NULL;
    ret->arr_t = NULL;

    return ret;
}

yalang_t_type yalang_type_new_string()
{
    yalang_t_type ret = (yalang_t_type) malloc (sizeof (*ret));

    ret->kind = STRING;
    ret->size = -1;
    ret->id = NULL;
    ret->arr_t = NULL;

    return ret;
}

yalang_t_type yalang_type_new_bool()
{
    yalang_t_type ret = (yalang_t_type) malloc (sizeof (*ret));

    ret->kind = BOOL;
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
