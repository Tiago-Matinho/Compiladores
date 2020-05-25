#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "yalangapt.h"



/*********************************************************************|
|                               DECLS                                 |
|*********************************************************************/


t_decls t_decls_new(t_decl d, t_decls ds)
{   
    t_decls ret = (t_decls) malloc( sizeof(*ret));

    if(ds == NULL)
        ret->kind = DECLS_SINGLE;
    else
        ret->kind = DECLS_LIST;

    ret->d = d;
    ret->ds = ds;

    return ret;
}


/*********************************************************************|
|                                DECL                                 |
|*********************************************************************/


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


/*********************************************************************|
|                                STMS                                 |
|*********************************************************************/


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


/*********************************************************************|
|                                STM                                  |
|*********************************************************************/


t_stm t_stm_new_decls(t_decls decls)
{   
    t_stm ret = (t_stm) malloc( sizeof(*ret));

    ret->kind = STM_DECL;
    ret->u.decls = decls;

    return ret;
}

t_stm t_stm_new_exp(t_exp exp)
{
    t_stm ret = (t_stm) malloc( sizeof(*ret));

    ret->kind = STM_EXP;
    ret->u.exp = exp;

    return ret;
}

t_stm t_stm_new_ifelse(t_exp exp, t_stms stms1, t_stms stms2)
{
    t_stm ret = (t_stm) malloc( sizeof(*ret));

    if(stms2 == NULL)
    {
        ret->kind = STM_IF;
        ret->u._if.exp = exp;
        ret->u._if.stms = stms1;
    }
    else
    {
        ret->kind = STM_IFELSE;
        ret->u._ifelse.exp = exp;
        ret->u._ifelse.stms1 = stms1;
        ret->u._ifelse.stms2 = stms2;
    }

    return ret;
}

t_stm t_stm_new_while(t_exp exp, t_stms stms)
{
    t_stm ret = (t_stm) malloc( sizeof(*ret));

    ret->kind = STM_WHILE;
    ret->u._while.exp = exp;
    ret->u._while.stms = stms;

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


/*********************************************************************|
|                                EXP                                  |
|*********************************************************************/


t_exp t_exp_new_intlit(int i)
{
    t_exp ret = (t_exp) malloc( sizeof(*ret));

    ret->kind = EXP_INTLIT;
    ret->u.lit.int_val = i;

    return ret;
}

t_exp t_exp_new_floatlit(double f)
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


/*********************************************************************|
|                              ARGSDEF                                |
|*********************************************************************/


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


/*********************************************************************|
|                               ARGDEF                                |
|*********************************************************************/


t_argdef t_argdef_new(char *id, t_type type)
{
    t_argdef ret = (t_argdef) malloc( sizeof(*ret));

    ret->id = id;
    ret->type = type;

    return ret;
}


/*********************************************************************|
|                                ARGS                                 |
|*********************************************************************/


t_args t_args_new(t_exp exp, t_args a)
{
    t_args ret = malloc( sizeof(*ret));

    ret->exp = exp;
    ret->as = a;

    return ret;
}


/*********************************************************************|
|                                 IDS                                 |
|*********************************************************************/


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


/*********************************************************************|
|                                TYPE                                 |
|*********************************************************************/


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
    
    case 'a':
        ret->kind =T_ANY;
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


/*********************************************************************|
|                               PRINTS                                |
|*********************************************************************/


void strprint(char *str)
{
    while(*str != '\0') {
        switch(*str) {
        case ' ':
            printf("\\ ");
            break;
        
        case '\\':
            printf("\\textbackslash ");
            break;
            
        default:
            printf("%c", *str);
            break;
        }

        str++;
    }
    printf("");
}

void t_ids_print(t_ids this)
{
    if(this == NULL)
        exit(1);


    switch(this->kind) {
    case IDS_SINGLE:
        printf("[.ID $%s$ ]", this->id);
        break;
    
    default:
        printf("[.IDs ");
        printf("[.ID $%s$ ] ", this->id);
        t_ids_print(this->ids);
        printf("]\n");
        break;
    }

}

void t_type_print(t_type this)
{
    if(this == NULL)
        exit(1);

    printf("[.TYPE ");

    switch(this->kind) {
    case T_INT:
        printf("$int$ ");
        break;

    case T_FLOAT:
        printf("$float$ ");
        break;

    case T_STRING:
        printf("$string$ ");
        break;

    case T_BOOL:
        printf("$bool$ ");
        break;

    case T_VOID:
        printf("$void$ ");
        break;

    case T_ID:
        printf("$%s$ ", this->id);
        break;

    case T_ANY:
        printf("$any$ ");
        break;
    
    default:
        printf("[.ARRAY ");
        t_type_print(this->array_type);
        printf("[.SIZE $%d$ ] ]", this->size);
        break;
    }

    printf("]\n");
}

void t_exp_print(t_exp this)
{
    if(this == NULL)
        exit(1);

    printf("[.EXP ");

    switch(this->kind) {
    case EXP_INTLIT:
        printf("[.INTLIT $%d$ ] ", this->u.lit.int_val);
        break;
    
    case EXP_FLOATLIT:
        printf("[.FLOATLIT $%f$ ] ", this->u.lit.float_val);
        break;
    
    case EXP_STRINGLIT:
        printf("[.STRINGLIT $");
        strprint(this->u.lit.string_val);
        printf("$ ] ");
        break;
    
    case EXP_BOOLLIT:
        printf("[.BOOLLIT ");
        this->u.lit.bool_val ? printf("$true$ ") : printf("$false$ ");
        printf("] ");
        break;
    
    case EXP_ID:
        printf("[.ID $%s$ ] ", this->u.id);
        break;
    
    case EXP_ARRAY:
        printf("[.ARRAY ");
        t_exp_print(this->u.array.exp);
        printf("[.POS $%d$ ] ] ");
        break;
    
    case EXP_BINOP:
        printf("[.BINOP $%s$ ", this->u.binop.op);
        t_exp_print(this->u.binop.exp1);
        t_exp_print(this->u.binop.exp2);
        printf("] ");
        break;
    
    case EXP_UNOP:
        printf("[.UNOP $%s$ ", this->u.unop.op);
        t_exp_print(this->u.unop.exp);
        printf("] ");
        break;
    
    case EXP_ASSIGN:
        printf("[.ASSIGN ");
        t_exp_print(this->u.assign.exp1);
        t_exp_print(this->u.assign.exp2);
        printf("] ");
        break;
    
    default:
        printf("[.CALL [.ID $%s$ ] ", this->u.funct.id);
        t_args_print(this->u.funct.args);
        printf("] ");
        break;
    }

    printf("]\n");
}

void t_args_print(t_args this)
{
    if(this == NULL)
        exit(1);

   if(this->as == NULL) {
        printf("[.ARG ");
        t_exp_print(this->exp);
    }

    else {
        printf("[.ARGs [.ARG ");
        t_exp_print(this->exp);
        printf("] ");
        t_args_print(this->as);
    }

    printf("]\n");
}

void t_decls_print(t_decls this)
{
    if(this == NULL)
        exit(1);

    printf("[.DECLs ");
    
    switch(this->kind) {
    case DECLS_SINGLE:
        t_decl_print(this->d);
        printf("$empty$ ");
        break;
    
    default:
        t_decl_print(this->d);
        t_decls_print(this->ds);
        break;
    }

    printf("]\n");    
}

void t_decl_print(t_decl this)
{
    if(this == NULL)
        exit(1);

    switch(this->kind) {
    case DECL_VAR:
        printf("[.DECL(var) ");
        t_ids_print(this->u.var.ids);
        t_type_print(this->u.var.type);
        break;

    case DECL_VARINIT:
        printf("[.DECL(varinit) ");
        t_ids_print(this->u.varinit.ids);
        t_type_print(this->u.varinit.type);
        printf("[.VALUE ");
        t_exp_print(this->u.varinit.value);
        printf("] ");
        break;

    case DECL_FUNCT_VOID:
        printf("[.DECL(funct) [.ID $%s$ ] [.ARGs $empty$ ] ", this->u.funct.id);
        t_type_print(this->u.funct.type);
        printf("[.BODY ");
        t_stms_print(this->u.funct.stms);
        printf("] ");
        break;

    case DECL_FUNCT:
        printf("[.DECL(funct) [.ID $%s$ ] ", this->u.funct.id);
        t_argsdef_print(this->u.funct.args);
        t_type_print(this->u.funct.type);
        printf("[.BODY ");
        t_stms_print(this->u.funct.stms);
        printf("] ");
        break;
    
    default:
        printf("[.DECL(define) [.ID $%s$ ] ", this->u.define.id);
        t_type_print(this->u.define.type);
        break;
    }

    printf("]\n");
}

void t_stms_print(t_stms this)
{
    if(this == NULL)
        exit(1);
    
    printf("[.STMs ");

    switch(this->kind) {
    case STMS_SINGLE:
        t_stm_print(this->s);
        printf("$empty$ ");
        break;
    
    default:
        t_stm_print(this->s);
        t_stms_print(this->ss);
        break;
    }

    printf("]\n");
}

void t_stm_print(t_stm this)
{
    if(this == NULL)
        exit(1);
        

    switch(this->kind) {
    case STM_DECL:
        t_decls_print(this->u.decls);
        break;

    case STM_EXP:
        printf("[.STM ");
        t_exp_print(this->u.exp);
        printf("]\n");
        break;
    
    case STM_RET:
        printf("[.STM [.RETURN ");
        t_exp_print(this->u.exp);
        printf("] ]\n");
        break;

    case STM_IF:
        printf("[.STM [.IF [.CONDITION ");
        t_exp_print(this->u._if.exp);
        printf("] [.THEN ");
        t_stms_print(this->u._if.stms);
        printf("] ] ]\n");
        break;

    case STM_IFELSE:
        printf("[.STM [.IFELSE [.CONDITION ");
        t_exp_print(this->u._ifelse.exp);
        printf("] [.THEN ");
        t_stms_print(this->u._ifelse.stms1);
        printf("] [.ELSE ");
        t_stms_print(this->u._ifelse.stms2);
        printf("] ] ]\n");
        break;

    case STM_WHILE:
        printf("[.STM [.WHILE [.CONDITION ");
        t_exp_print(this->u._while.exp);
        printf("] [.DO ");
        t_stms_print(this->u._while.stms);
        printf("] ] ]\n");
        break;

    default:
        printf("[.STM $next$ ]\n");
        break;
    }
}

void t_argsdef_print(t_argsdef this)
{
    if(this == NULL)
        exit(1);

    switch(this->kind) {
    case ARGS_SINGLE:
        printf("[.ARG ");
        t_argdef_print(this->a);
        break;
    
    default:
        printf("[.ARGs [.ARG ");
        t_argdef_print(this->a);
        printf("] ");
        t_argsdef_print(this->as);
        break;
    }

    printf("]\n");
}

void t_argdef_print(t_argdef this)
{
    if(this == NULL)
        exit(1);

    printf("[.ID $%s$ ] ", this->id);
    t_type_print(this->type);
}
