#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "yalangapt.h"


/*********************************************************************|
|                              STRUCTS                                |
|*********************************************************************/


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
    enum { DECL_VAR, DECL_VARINIT, DECL_FUNCT_VOID, DECL_FUNCT,
    DECL_DEFINE } kind;

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
    enum { STM_DECL, STM_EXP, STM_RET, STM_IF, STM_IFELSE, STM_WHILE,
    STM_NEXT } kind;

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
    enum { EXP_INTLIT, EXP_FLOATLIT, EXP_STRINGLIT, EXP_BOOLLIT, EXP_ID,
    EXP_ARRAY, EXP_BINOP, EXP_UNOP, EXP_ASSIGN, EXP_FUNCT } kind;

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
    enum { T_INT, T_FLOAT, T_STRING, T_BOOL, T_VOID, T_ID, T_ARRAY, T_ANY } kind;

    char *id;
    int size;
    t_type array_type;
};

// st_bucket
typedef struct st_bucket_
{
    enum {ST_VAR, ST_FUNCT, ST_DEFINE} kind;

    char *id;
    int scope;
    ST_Bucket next;

    union {
        struct {
            enum {VARloc, VARarg} kind;
            t_type yatype;
        } var;

        struct {
            t_type return_type;
            t_argsdef args;
        } func;

        struct {
            t_type yatype;
        } define;
    } u;
};

// st
typedef struct st_
{
    int curScope;
    ST_Bucket array[MAX_CONTEXT];
};


/*********************************************************************|
|                              BUCKETS                                |
|*********************************************************************/


ST_Bucket st_bucket_new_var(char *id, t_type yatype, int scope)
{
    ST_Bucket ret = (ST_Bucket) malloc(sizeof(*ret));

    ret->id = id;
    ret->scope = scope;
    ret->next = NULL;
    ret->kind = ST_VAR;
    ret->u.var.yatype = yatype;

    return ret;
}

ST_Bucket st_bucket_new_funct(char *id, t_argsdef args,
t_type return_type, int scope)
{
    ST_Bucket ret = (ST_Bucket) malloc(sizeof(*ret));

    ret->id = id;
    ret->scope = scope;
    ret->next = NULL;
    ret->kind = ST_FUNCT;
    ret->u.func.args = args;
    ret->u.func.return_type = return_type;

    return ret;
}

ST_Bucket st_bucket_new_define(char *id, t_type yatype, int scope)
{
    ST_Bucket ret = (ST_Bucket) malloc(sizeof(*ret));

    ret->id = id;
    ret->scope = scope;
    ret->next = NULL;
    ret->kind = ST_DEFINE;
    ret->u.define.yatype = yatype;

    return ret;
}

ST_Bucket st_bucket_new_print()
{
    ST_Bucket ret = (ST_Bucket) malloc(sizeof(*ret));

    t_type any = (t_type) malloc(sizeof(*any));
    any->kind = T_ANY;
    t_type _void = t_type_new('v');

    t_argdef arg = t_argdef_new("", any);
    t_argsdef args = t_argsdef_new(arg, NULL);

    ret->id = "print";
    ret->scope = 0;
    ret->next = NULL;
    ret->kind = ST_FUNCT;
    ret->u.func.args = args;
    ret->u.func.return_type = _void;

    return ret;
}

ST_Bucket st_bucket_new_input()
{
    ST_Bucket ret = (ST_Bucket) malloc(sizeof(*ret));

    t_type any = (t_type) malloc(sizeof(*any));
    any->kind = T_ANY;
    t_type _void = t_type_new('v');

    t_argdef arg = t_argdef_new("", any);
    t_argsdef args = t_argsdef_new(arg, NULL);

    ret->id = "input";
    ret->scope = 0;
    ret->next = NULL;
    ret->kind = ST_FUNCT;
    ret->u.func.args = args;
    ret->u.func.return_type = _void;

    return ret;
}


/*********************************************************************|
|                            SYMBOL TABLE                             |
|*********************************************************************/

ST st_new()
{
    ST ret = (ST) malloc(sizeof(*ret));

    if(ret != NULL){
        ret->curScope = 0;
        for(int i = 0; i < MAX_CONTEXT; i++)
            ret->array[i] = NULL;
    }
    return ret;
}

unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void st_insert(ST_Bucket new, ST st)
{
    int key = (int) hash(new->id) % MAX_CONTEXT;

    printf("insert %s\n", new->id);

    if(st->array[key] == NULL){
        st->array[key] = new;
        return;
    }

    if(strcmp(st->array[key]->id, new->id) != 0){
        while(st->array[key] != NULL)
            key = (key + 1) % MAX_CONTEXT;
        st->array[key] = new;
    }

    new->next = st->array[key];
    st->array[key] = new;

}

ST_Bucket st_lookup(char *id, ST st)
{
    int key = (int) hash(id) % MAX_CONTEXT;

    printf("lookup %s\n", id);

    if(st->array[key] == NULL)
        return NULL;

    ST_Bucket cur = st->array[key];

    while(cur != NULL){
        if(strcmp(cur->id, id) == 0)
            return cur;

        cur = cur->next;
    }

    return NULL;
}

t_type st_lookup_type(char *id, ST st)
{
    ST_Bucket search = st_lookup(id, st);

    if(search == NULL)
        return NULL;

    switch(search->kind){
        case ST_VAR:
            return search->u.var.yatype;
        case ST_FUNCT:
            return search->u.func.return_type;
        case ST_DEFINE:
            return search->u.define.yatype;
        default:
            printf("ERROR: wrong kind @ st_lookup_type\n");
            exit(1);
            return NULL;
    }
}

void st_new_scope(ST st)
{
    st->curScope++;
    printf("new scope\n");
}

void st_drop_scope(ST st)
{
    ST_Bucket aux;

    for(int i = 0; i < MAX_CONTEXT; i++){
        if(st->array[i] != NULL){
            if(st->array[i]->scope == st->curScope){
                aux = st->array[i];
                st->array[i] = aux->next;
                free(aux);
            }
        }
    }

    st->curScope--;
    printf("drop scope\n");
}


/*********************************************************************|
|                             UTILITIES                               |
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

bool compatible_types(t_type l_type, t_type r_type, ST st)
{
    t_type search = NULL;
    switch(l_type->kind){
        case T_INT:
        case T_FLOAT:
            switch(r_type->kind){
                case T_INT:
                case T_FLOAT:
                case T_VOID:
                    return true;
                case T_STRING:
                case T_BOOL:
                case T_ID:
                    return false;
                case T_ARRAY:
                    return compatible_types(l_type, r_type->array_type, st);
                default:
                    printf("ERROR: wrong type @ compatible_types\n");
                    exit(1);
                    return false;
            }
        case T_STRING:
            switch(r_type->kind){
                case T_INT:
                case T_FLOAT:
                case T_STRING:
                case T_VOID:
                    return true;
                case T_BOOL:
                case T_ID:
                    return false;
                case T_ARRAY:
                    return compatible_types(l_type, r_type->array_type, st);
                default:
                    printf("ERROR: wrong type @ compatible_types\n");
                    exit(1);
                    return false;
            }
        case T_BOOL:
            switch(r_type->kind){
                case T_INT:
                case T_BOOL:
                case T_VOID:
                    return true;
                case T_FLOAT:
                case T_STRING:
                case T_ID:
                    return false;
                case T_ARRAY:
                    return compatible_types(l_type, r_type->array_type, st);
                default:
                    printf("ERROR: wrong type @ compatible_types\n");
                    exit(1);
                    return false;
            }
        case T_VOID:
            return true;
        case T_ID:
            return strcmp(l_type->id, r_type->id) == 0;
        case T_ARRAY:
            return compatible_types(l_type->array_type, r_type, st);

        case T_ANY:
            return true;
        default:
            printf("ERROR: wrong type @ compatible_types\n");
            exit(1);
            return false;
    }
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

// exp_print
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
        printf("[.POS $%d$ ] ] ", this->u.array.pos);
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

// semantic analysis
t_type t_exp_ant(t_exp node, ST st)
{
    if(node == NULL)
        exit(1);

    t_type ret = NULL;
    t_type l_type = NULL;
    t_type r_type = NULL;
    ST_Bucket search = NULL;
    t_argsdef cur_argdef = NULL;
    t_args cur_arg = NULL;

    switch(node->kind) {
        case EXP_INTLIT:
            ret = t_type_new('i');
            break;
    
        case EXP_FLOATLIT:
            ret = t_type_new('f');
            break;
    
        case EXP_STRINGLIT:
            ret = t_type_new('s');
            break;
    
        case EXP_BOOLLIT:
            ret = t_type_new('b');
            break;
    
        case EXP_ID:
            ret = st_lookup_type(node->u.id, st);

            if(ret == NULL)
                printf("ERROR: %s not previously defined\n", node->u.id);            
            break;
    
        case EXP_ARRAY:
            ret = t_exp_ant(node->u.array.exp, st);
            break;
    
        case EXP_BINOP:

            l_type = t_exp_ant(node->u.binop.exp1, st);
            r_type = t_exp_ant(node->u.binop.exp2, st);

            if(!compatible_types(l_type, r_type, st))
                printf("ERROR: incompatible types\n");

                
            switch(node->u.binop.op[0]){
                case '+':
                case '-':
                case '/':
                case '*':
                case '^':
                case 'm':
                    if(l_type->kind == T_FLOAT ||
                        r_type->kind == T_FLOAT )
                        ret = t_type_new('f');
                
                    ret = t_type_new('i');
                    break;

                default:
                    ret = t_type_new('b');
                    break;

            }
            break;
    
        case EXP_UNOP:

            l_type = t_exp_ant(node->u.unop.exp, st);

            switch(node->u.unop.op[0]){
                case '-':
                    if(l_type->kind == T_FLOAT)
                        ret = t_type_new('f');
                    else if(l_type->kind == T_INT)
                        ret = t_type_new('i');
                    else{
                        printf("ERROR: incompatible types\n");
                        ret =  t_type_new('v');
                    }
                    break;

                default:    //not
                    if(l_type->kind == T_BOOL)
                        ret = t_type_new('b');
                    else{
                        printf("ERROR: incompatible types\n");
                        ret =  t_type_new('v');
                    }
                    break;

            }
            break;
        
        case EXP_ASSIGN:
            l_type = t_exp_ant(node->u.assign.exp1, st);
            r_type = t_exp_ant(node->u.assign.exp2, st);

            if(!compatible_types(l_type, r_type, st))
                printf("ASSIGN ERROR: incompatible types\n");

            ret = l_type;
            break;
        
        case EXP_FUNCT:
            search = st_lookup(node->u.funct.id, st);

            if(search == NULL)
                printf("ERROR: function %s not previously defined\n", node->u.funct.id);
            else{
                cur_argdef = search->u.func.args;
                cur_arg = node->u.funct.args;
                while(cur_argdef != NULL){
                    if(cur_arg == NULL){
                        printf("ERROR: few arguments in function %s\n", node->u.funct.id);
                        break;
                    }

                    l_type = t_exp_ant(cur_arg->exp, st);

                    if(!compatible_types(cur_argdef->a->type, l_type, st))
                        printf("ERROR: wrong arg type %s\n", cur_argdef->a->id);

                    cur_argdef = cur_argdef->as;
                    cur_arg = cur_arg->as;
                }

                if(cur_arg != NULL)
                    printf("ERROR: too many arguments in function %s\n", node->u.funct.id);

                ret = search->u.func.return_type;
                }
        default:
            break;
    }

    return ret;
}


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

// decls_print
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

// semantic analysis
void t_decls_ant(t_decls node, ST st)
{
    if(node == NULL)
        exit(1);
    
    switch(node->kind){
        case DECLS_SINGLE:
            t_decl_ant(node->d, st);
            break;
    
    default:
        t_decl_ant(node->d, st);
        t_decls_ant(node->ds, st);
        break;
    }
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

// decl_print
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

// semantic analysis
void t_decl_ant(t_decl node, ST st)
{
    if(node == NULL)
        exit(1);

    t_type type = NULL;
    t_type exp_type = NULL;
    t_type return_type = NULL;
    t_ids cur_id = NULL;
    ST_Bucket search = NULL;
    ST_Bucket new = NULL;
    t_argsdef args = NULL;
    t_argdef cur_arg = NULL;

    switch(node->kind) {
        case DECL_VAR:
            type = node->u.var.type;
            cur_id = node->u.var.ids;
            
            // ciclo para casos de multiplos ids definidos
            while(cur_id != NULL){
                search = st_lookup(cur_id->id, st); //procura se existe algo com o mesmo nome e scope

                if(search != NULL){
                    if(search->scope == st->curScope)
                        printf("ERROR: Variable %s already defined\n", cur_id->id); // erro
                    else{
                        new = st_bucket_new_var(cur_id->id, type, st->curScope);
                        st_insert(new, st);
                    }
                }
                else{   // adiciona a symbol table
                    new = st_bucket_new_var(cur_id->id, type, st->curScope);
                    st_insert(new, st);
                }
                cur_id = cur_id->ids;
            }
            break;

        case DECL_VARINIT:
            type = node->u.varinit.type;
            exp_type = t_exp_ant(node->u.varinit.value, st);

            if(!compatible_types(type, exp_type, st))
                printf("ERROR: types missmatch\n");

            cur_id = node->u.varinit.ids;

            // ciclo para casos de multiplos ids definidos
            while(cur_id != NULL){
                search = st_lookup(cur_id->id, st); //procura se existe algo com o mesmo nome e scope

                if(search != NULL){
                    if(search->scope == st->curScope)
                        printf("ERROR: Variable %s already defined\n", cur_id->id); // erro
                    else{
                        new = st_bucket_new_var(cur_id->id, type, st->curScope);
                        st_insert(new, st);
                    }
                    
                }
                else{   // adiciona a symbol table
                    new = st_bucket_new_var(cur_id->id, type, st->curScope);
                    st_insert(new, st);
                }
                cur_id = cur_id->ids;
            }
            break;

        case DECL_FUNCT_VOID:
            args = node->u.funct.args;
            return_type = node->u.funct.type;
            
            search = st_lookup(node->u.funct.id, st); //procura se existe algo com o mesmo nome

            if(search != NULL)
                printf("ERROR: Function %s already defined\n", node->u.funct.id);

            else{   // adiciona a symbol table
                new = st_bucket_new_funct(node->u.funct.id, args, return_type, st->curScope);
                st_insert(new, st);
            }

            st_new_scope(st);

            t_stms_ant(node->u.funct.stms, st);

            break;

        case DECL_FUNCT:
            args = node->u.funct.args;
            return_type = node->u.funct.type;
            
            search = st_lookup(node->u.funct.id, st); //procura se existe algo com o mesmo nome

            if(search != NULL)
                printf("ERROR: Function %s already defined\n", node->u.funct.id);

            else{   // adiciona a symbol table
                new = st_bucket_new_funct(node->u.funct.id, args, return_type, st->curScope);
                st_insert(new, st);
            }

            st_new_scope(st);

            while(args != NULL){
                cur_arg = args->a;

                new = st_bucket_new_var(cur_arg->id, cur_arg->type, st->curScope);

                st_insert(new, st);

                args = args->as;
            }
            t_stms_ant(node->u.funct.stms, st);

            st_drop_scope(st);

            break;
        
        default:
            search = st_lookup(node->u.define.id, st); //procura se existe algo com o mesmo nome

            if(search != NULL)
                printf("ERROR: Function %s already defined\n", node->u.define.id);

            else{   // adiciona a symbol table
                new = st_bucket_new_define(node->u.define.id, node->u.define.type, st->curScope);
                st_insert(new, st);
            }
            break;
    }

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

// stms_print
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

// semantic analysis
void t_stms_ant(t_stms node, ST st)
{
    if(node == NULL)
        exit(1);
    
    switch(node->kind){
        case STMS_SINGLE:
            t_stm_ant(node->s, st);
            break;
    
    default:
            t_stm_ant(node->s, st);
            t_stms_ant(node->ss, st);
        break;
    }
}


/*********************************************************************|
|                                STM                                  |
|*********************************************************************/


t_stm t_stm_new_decl(t_decl decl)
{   
    t_stm ret = (t_stm) malloc( sizeof(*ret));

    ret->kind = STM_DECL;
    ret->u.decl = decl;

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

// stm_print
void t_stm_print(t_stm this)
{
    if(this == NULL)
        exit(1);
        

    switch(this->kind) {
    case STM_DECL:
        t_decl_print(this->u.decl);
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

// semantic analysis
void t_stm_ant(t_stm node, ST st)
{
    if(node == NULL)
        exit(1);

    t_type boolean = t_type_new('b');
    
    switch(node->kind){
        case STM_DECL:
            t_decl_ant(node->u.decl, st);
            break;
        case STM_EXP:
            t_exp_ant(node->u.exp, st);
            break;
        case STM_RET:
            t_exp_ant(node->u.exp, st);
            break;
        case STM_IF:
            if(!compatible_types(boolean, t_exp_ant(node->u._if.exp, st), st))
                printf("ERROR: expression not boolean\n");
            t_stms_ant(node->u._if.stms, st);
            break;
        case STM_IFELSE:
            if(!compatible_types(boolean, t_exp_ant(node->u._ifelse.exp, st), st))
                printf("ERROR: expression not boolean\n");
            t_stms_ant(node->u._ifelse.stms1, st);
            t_stms_ant(node->u._ifelse.stms2, st);
            break;
        case STM_WHILE:
            if(!compatible_types(boolean, t_exp_ant(node->u._while.exp, st), st))
                printf("ERROR: expression not boolean\n");
            t_stms_ant(node->u._while.stms, st);
        default:
            break;
            
    }
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

// argsdef_print
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

// argdef_print
void t_argdef_print(t_argdef this)
{
    if(this == NULL)
        exit(1);

    printf("[.ID $%s$ ] ", this->id);
    t_type_print(this->type);
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

// args_print
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

// ids_print
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

// type_print
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
    
    default:
        printf("[.ARRAY ");
        t_type_print(this->array_type);
        printf("[.SIZE $%d$ ] ]", this->size);
        break;
    }

    printf("]\n");
}


