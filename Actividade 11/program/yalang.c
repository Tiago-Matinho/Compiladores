#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "yalang.h"


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
    enum {ST_VAR, ST_FUNCT, ST_DEFINE, CODEGEN_VAR} kind;

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

        struct {
            short size;
            int address;
        } codegen_var;

    } u;
};

// st
typedef struct st_
{
    bool error;
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

ST_Bucket st_bucket_new_codegen(char *id, t_type yatype, int scope,
    int address)
{
    ST_Bucket ret = (ST_Bucket) malloc(sizeof(*ret));

    ret->id = id;
    ret->scope = scope;
    ret->next = NULL;
    ret->kind = CODEGEN_VAR;
    
    ret->u.codegen_var.address = address;

    switch(){
    case /* constant-expression */:
        /* code */
        break;
    
    default:
        break;
    }
    return ret;
}


/*********************************************************************|
|                            SYMBOL TABLE                             |
|*********************************************************************/


ST st_new()
{
    ST ret = (ST) malloc(sizeof(*ret));

    if(ret != NULL){
        ret->error = false;
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

    // DEBUG SYMBOL TABLE
    // printf("insert %s\n", new->id);

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

    // DEBUG SYMBOL TABLE
    // printf("lookup %s\n", id);

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
    // DEBUG SYMBOL TABLE
    // printf("new scope\n");
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
    // DEBUG SYMBOL TABLE
    // printf("drop scope\n");
}


/*********************************************************************|
|                             UTILITIES                               |
|*********************************************************************/


void strprint(FILE* fp, char *str)
{
    while(*str != '\0') {
        switch(*str) {
        case ' ':
            fprintf(fp, "\\ ");
            break;
        
        case '\\':
            fprintf(fp, "\\textbackslash ");
            break;
            
        default:
            fprintf(fp, "%c", *str);
            break;
        }

        str++;
    }
    fprintf(fp, "");
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
void t_decls_print(FILE* fp, t_decls this)
{
    if(this == NULL)
        exit(1);

    fprintf(fp, "[.DECLs ");
    
    switch(this->kind) {
    case DECLS_SINGLE:
        t_decl_print(fp, this->d);
        printf(fp, "$empty$ ");
        break;
    
    case DECLS_LIST:
        t_decl_print(fp, this->d);
        t_decls_print(fp, this->ds);
        break;

    default:
        //TODO
    }

    fprintf(fp, "]\n");    
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

// TODO NAME
void t_decls_codegen(FILE* fp, t_decls node, ST st)
{
    if(node == NULL)
        exit(1);
    
    switch(node->kind){
        case DECLS_SINGLE:
            t_decl_codegen(fp, node->d, st);
            break;
    
    default:
        t_decl_codegen(fp, node->d, st);
        t_decls_codegen(fp, node->ds, st);
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
void t_decl_print(FILE* fp, t_decl this)
{
    if(this == NULL)
        exit(1);

    switch(this->kind) {
    case DECL_VAR:
        fprintf(fp, "[.DECL(var) ");
        t_ids_print(fp, this->u.var.ids);
        t_type_print(fp, this->u.var.type);
        break;

    case DECL_VARINIT:
        fprintf(fp, "[.DECL(varinit) ");
        t_ids_print(fp, this->u.varinit.ids);
        t_type_print(fp, this->u.varinit.type);
        fprintf(fp, "[.VALUE ");
        t_exp_print(fp, this->u.varinit.value);
        fprintf(fp, "] ");
        break;

    case DECL_FUNCT_VOID:
        fprintf(fp, "[.DECL(funct) [.ID $%s$ ] [.ARGs $empty$ ] ", this->u.funct.id);
        t_type_print(fp, this->u.funct.type);
        fprintf(fp, "[.BODY ");
        t_stms_print(fp, this->u.funct.stms);
        fprintf(fp, "] ");
        break;

    case DECL_FUNCT:
        fprintf(fp, "[.DECL(funct) [.ID $%s$ ] ", this->u.funct.id);
        t_argsdef_print(fp, this->u.funct.args);
        t_type_print(fp, this->u.funct.type);
        fprintf(fp, "[.BODY ");
        t_stms_print(fp, this->u.funct.stms);
        fprintf(fp, "] ");
        break;
    
    case DECL_DEFINE:
        fprintf(fp, "[.DECL(define) [.ID $%s$ ] ", this->u.define.id);
        t_type_print(fp, this->u.define.type);
        break;
    
    default:
        //TODO
    }

    fprintf(fp, "]\n");
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
                    if(search->scope == st->curScope){
                        printf("ERROR: Variable %s already defined\n", cur_id->id);
                        st->error = true;
                    }
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

            if(!compatible_types(type, exp_type, st)){
                printf("ERROR: types missmatch\n");
                st->error = true;
            }

            cur_id = node->u.varinit.ids;

            // ciclo para casos de multiplos ids definidos
            while(cur_id != NULL){
                search = st_lookup(cur_id->id, st); //procura se existe algo com o mesmo nome e scope

                if(search != NULL){
                    if(search->scope == st->curScope)
                        printf("ERROR: Variable %s already defined\n", cur_id->id); // erro
                        st->error = true;
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

            if(search != NULL){
                printf("ERROR: Function %s already defined\n", node->u.funct.id);
                st->error = true;
            }

            else{   // adiciona a symbol table
                new = st_bucket_new_funct(node->u.funct.id, args, return_type, st->curScope);
                st_insert(new, st);
            }

            st_new_scope(st);

            t_stms_ant(node->u.funct.stms, st);

            st_drop_scope(st);

            break;

        case DECL_FUNCT:
            args = node->u.funct.args;
            return_type = node->u.funct.type;
            
            search = st_lookup(node->u.funct.id, st); //procura se existe algo com o mesmo nome

            if(search != NULL){
                printf("ERROR: Function %s already defined\n", node->u.funct.id);
                st->error = true;    
            }
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

            if(search != NULL){
                printf("ERROR: Function %s already defined\n", node->u.define.id);
                st->error = true;
            }

            else{   // adiciona a symbol table
                new = st_bucket_new_define(node->u.define.id, node->u.define.type, st->curScope);
                st_insert(new, st);
            }
            break;
    }

}

// FIXME
void t_decl_codegen(FILE* fp, t_decl node, ST st)
{
    if(node == NULL)
        exit(1);

    switch(node->kind) {
        case DECL_VAR:
            
            break;

        case DECL_VARINIT:
            
            break;

        case DECL_FUNCT_VOID:
            
            break;

        case DECL_FUNCT:
            fprintf(fp, "funct_%s_decl:\n", node->u.funct.id);
            fprintf(fp, "move $fp, $sp\n");  // Novo FP
            fprintf(fp, "sw $ra, 0($sp)\n"); // Guarda o Return Add anterior
            fprintf(fp, "addiu $sp, $sp, -4\n");

            t_stms_codegen(fp, node->u.funct.stms);

            fprintf(fp, "funct_%s_exit:\n", node->u.funct.id);
            fprintf(fp, "lw $ra, 4 ($sp)\n");
            break;
        
        case DECL_DEFINE:

            break;

        default:
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
void t_stms_print(FILE* fp, t_stms this)
{
    if(this == NULL)
        exit(1);
    
    fprintf(fp, "[.STMs ");

    switch(this->kind) {
    case STMS_SINGLE:
        t_stm_print(fp, this->s);
        fprintf(fp, "$empty$ ");
        break;
    
    case STMS_LIST:
        t_stm_print(fp, this->s);
        t_stms_print(fp, this->ss);
        break;
    
    default:
        //TODO
    }

    fprintf(fp, "]\n");
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

//
void t_stms_codegen(FILE* fp, t_stms node, ST st)
{
    if(node == NULL)
        exit(1);
    
    switch(node->kind){
        case STMS_SINGLE:
            t_stm_codegen(node->s, st);
            break;
    
    default:
            t_stm_codegen(node->s, st);
            t_stms_codegen(node->ss, st);
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
void t_stm_print(FILE* fp, t_stm this)
{
    if(this == NULL)
        exit(1);
        

    switch(this->kind) {
    case STM_DECL:
        t_decl_print(fp, this->u.decl);
        break;

    case STM_EXP:
        fprintf(fp, "[.STM ");
        t_exp_print(fp, this->u.exp);
        fprintf(fp, "]\n");
        break;
    
    case STM_RET:
        fprintf(fp, "[.STM [.RETURN ");
        t_exp_print(fp, this->u.exp);
        fprintf(fp, "] ]\n");
        break;

    case STM_IF:
        fprintf(fp, "[.STM [.IF [.CONDITION ");
        t_exp_print(fp, this->u._if.exp);
        fprintf(fp, "] [.THEN ");
        t_stms_print(fp, this->u._if.stms);
        fprintf(fp, "] ] ]\n");
        break;

    case STM_IFELSE:
        fprintf(fp, "[.STM [.IFELSE [.CONDITION ");
        t_exp_print(fp, this->u._ifelse.exp);
        fprintf(fp, "] [.THEN ");
        t_stms_print(fp, this->u._ifelse.stms1);
        fprintf(fp, "] [.ELSE ");
        t_stms_print(fp, this->u._ifelse.stms2);
        fprintf(fp, "] ] ]\n");
        break;

    case STM_WHILE:
        fprintf(fp, "[.STM [.WHILE [.CONDITION ");
        t_exp_print(fp, this->u._while.exp);
        fprintf(fp, "] [.DO ");
        t_stms_print(fp, this->u._while.stms);
        fprintf(fp, "] ] ]\n");
        break;

    case STM_NEXT:
        fprintf(fp, "[.STM $next$ ]\n");
        break;
    
    default:
        //TODO
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
            if(!compatible_types(boolean, t_exp_ant(node->u._if.exp, st), st)){
                printf("ERROR: expression not boolean\n");
                st->error = true;
            }
            t_stms_ant(node->u._if.stms, st);
            break;
        case STM_IFELSE:
            if(!compatible_types(boolean, t_exp_ant(node->u._ifelse.exp, st), st)){
                printf("ERROR: expression not boolean\n");
                st->error = true;
            }
            t_stms_ant(node->u._ifelse.stms1, st);
            t_stms_ant(node->u._ifelse.stms2, st);
            break;
        case STM_WHILE:
            if(!compatible_types(boolean, t_exp_ant(node->u._while.exp, st), st)){
                printf("ERROR: expression not boolean\n");
                st->error = true;
            }
            t_stms_ant(node->u._while.stms, st);
        default:
            break;
            
    }
}

//
void t_stm_codegen(FILE* fp, t_stm node, ST st)
{
    if(node == NULL)
        exit(1);
    
    switch(node->kind){
        case STM_DECL:
            t_decl_codegen(node->u.decl, st);
            break;
        case STM_EXP:
            t_exp_codegen(node->u.exp, st);
            break;
        case STM_RET:
            t_exp_codegen(node->u.exp, st);
            break;
        case STM_IF:

            break;
        case STM_IFELSE:
           
            break;
        case STM_WHILE:
            
        default:
            break;
            
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
void t_exp_print(FILE* fp, t_exp this)
{
    if(this == NULL)
        exit(1);

    fprintf(fp, "[.EXP ");

    switch(this->kind) {
    case EXP_INTLIT:
        fprintf(fp, "[.INTLIT $%d$ ] ", this->u.lit.int_val);
        break;
    
    case EXP_FLOATLIT:
        fprintf(fp, "[.FLOATLIT $%f$ ] ", this->u.lit.float_val);
        break;
    
    case EXP_STRINGLIT:
        fprintf(fp, "[.STRINGLIT $");
        strprint(fp, this->u.lit.string_val);
        fprintf(fp, "$ ] ");
        break;
    
    case EXP_BOOLLIT:
        fprintf(fp, "[.BOOLLIT ");
        this->u.lit.bool_val ? fprintf(fp, "$true$ ") : fprintf(fp, "$false$ ");
        fprintf(fp, "] ");
        break;
    
    case EXP_ID:
        fprintf(fp, "[.ID $%s$ ] ", this->u.id);
        break;
    
    case EXP_ARRAY:
        fprintf(fp, "[.ARRAY ");
        t_exp_print(fp, this->u.array.exp);
        fprintf(fp, "[.POS $%d$ ] ] ", this->u.array.pos);
        break;
    
    case EXP_BINOP:
        fprintf(fp, "[.BINOP $%s$ ", this->u.binop.op);
        t_exp_print(fp, this->u.binop.exp1);
        t_exp_print(fp, this->u.binop.exp2);
        fprintf(fp, "] ");
        break;
    
    case EXP_UNOP:
        fprintf(fp, "[.UNOP $%s$ ", this->u.unop.op);
        t_exp_print(fp, this->u.unop.exp);
        fprintf(fp, "] ");
        break;
    
    case EXP_ASSIGN:
        fprintf(fp, "[.ASSIGN ");
        t_exp_print(fp, this->u.assign.exp1);
        t_exp_print(fp, this->u.assign.exp2);
        fprintf(fp, "] ");
        break;
    
    case EXP_FUNCT:
        fprintf(fp, "[.CALL [.ID $%s$ ] ", this->u.funct.id);
        t_args_print(fp, this->u.funct.args);
        fprintf(fp, "] ");
        break;

    default:
        //TODO
    }

    fprintf(fp, "]\n");
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

            if(ret == NULL){
                printf("ERROR: %s not previously defined\n", node->u.id);
                st->error = true;
            }           
            break;
    
        case EXP_ARRAY:
            ret = t_exp_ant(node->u.array.exp, st);
            break;
    
        case EXP_BINOP:

            l_type = t_exp_ant(node->u.binop.exp1, st);
            r_type = t_exp_ant(node->u.binop.exp2, st);

            if(!compatible_types(l_type, r_type, st)){
                printf("ERROR: incompatible types\n");
                st->error = true;
            }
                
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
                        st->error = true;
                        ret =  t_type_new('v');
                    }
                    break;

                default:    //not
                    if(l_type->kind == T_BOOL)
                        ret = t_type_new('b');
                    else{
                        printf("ERROR: incompatible types\n");
                        st->error = true;
                        ret =  t_type_new('v');
                    }
                    break;

            }
            break;
        
        case EXP_ASSIGN:
            l_type = t_exp_ant(node->u.assign.exp1, st);
            r_type = t_exp_ant(node->u.assign.exp2, st);

            if(!compatible_types(l_type, r_type, st)){
                printf("ASSIGN ERROR: incompatible types\n");
                st->error = true;
            }

            ret = l_type;
            break;
        
        case EXP_FUNCT:
            search = st_lookup(node->u.funct.id, st);

            if(search == NULL){
                printf("ERROR: function %s not previously defined\n", node->u.funct.id);
                st->error = true;
            }
            else{
                cur_argdef = search->u.func.args;
                cur_arg = node->u.funct.args;
                while(cur_argdef != NULL){
                    if(cur_arg == NULL){
                        printf("ERROR: few arguments in function %s\n", node->u.funct.id);
                        st->error = true;
                        break;
                    }

                    l_type = t_exp_ant(cur_arg->exp, st);

                    if(!compatible_types(cur_argdef->a->type, l_type, st)){
                        printf("ERROR: wrong arg type %s\n", cur_argdef->a->id);
                        st->error = true;
                    }

                    cur_argdef = cur_argdef->as;
                    cur_arg = cur_arg->as;
                }

                if(cur_arg != NULL){
                    printf("ERROR: too many arguments in function %s\n", node->u.funct.id);
                    st->error = true;
                }
                ret = search->u.func.return_type;
            }
        default:
            break;
    }

    return ret;
}

//
void t_exp_codegen(FILE* fp, t_exp node, ST st)
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
            printf("li $t0, %d\n");
            break;
    
        case EXP_FLOATLIT:
            printf("li $f0, %d\n"); //FIXME
            break;
    
        case EXP_STRINGLIT:
            ret = t_type_new('s'); //FIXME
            break;
    
        case EXP_BOOLLIT:
            printf("li $t0, ");
            node->u.lit.bool_val ? printf("1\n") : printf("0\n");
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
            t_exp_codegen(node->u.binop.exp1);
            printf("sw $t0, 0($sp)\n");
            printf("addiu $sp, $sp, -4\n");
            t_exp_codegen(node->u.binop.exp2);
            printf("lw $t1, 4($sp)\n");

            switch(node->u.binop.op[0]){
                case '+':
                    printf("add "); break;
                case '-':
                    printf("sub "); break;
                case '/':
                    printf("div "); break; //FIXME
                case '*':
                    printf("mul "); break;
                case '^':
                    printf("add "); break; //FIXME
                case 'm':
                    printf("add "); break; //FIXME

                default:
                    ret = t_type_new('b'); //FIXME
                    break;
            }

            printf("$t0, $t1, $t0\n");
            printf("addiu $sp, $sp, 4\n");
            break;
    
        case EXP_UNOP:
            t_exp_codegen(node->u.unop.exp);

            switch(node->u.unop.op[0]){
                case '-':
                    printf("mul $t0, $t0, -1\n");
                    break;

                default:    //not
                    printf("not $t0, $t0\n");
                    break;

            }
            break;
        
        case EXP_ASSIGN:    //FIXME
            l_type = t_exp_ant(node->u.assign.exp1, st);
            r_type = t_exp_ant(node->u.assign.exp2, st);

            if(!compatible_types(l_type, r_type, st))
                printf("ASSIGN ERROR: incompatible types\n");

            ret = l_type;
            break;
        
        case EXP_FUNCT: //FIXME
            printf("sw $fp, 0($sp)\n");
            printf("addiu $sp, $sp, -4\n");

            t_args_codegen(node->u.funct.args);

            printf("jal funct_%s_decl\n", node->u.funct.id);

        default:
            break;
    }

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

// argsdef_print
void t_argsdef_print(FILE* fp, t_argsdef this)
{
    if(this == NULL)
        exit(1);

    switch(this->kind) {
    case ARGS_SINGLE:
        fprintf(fp, "[.ARG ");
        t_argdef_print(fp, this->a);
        break;
    
    case ARGS_LIST:
        fprintf(fp, "[.ARGs [.ARG ");
        t_argdef_print(fp, this->a);
        fprintf(fp, "] ");
        t_argsdef_print(fp, this->as);
        break;

    default:
    //TODO
    }

    fprintf(fp, "]\n");
}

//
void t_argsdef_codegen(FILE* fp, t_argsdef node, ST st);


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
void t_argdef_print(FILE* fp, t_argdef this)
{
    if(this == NULL)
        exit(1);

    fprintf(fp, "[.ID $%s$ ] ", this->id);
    t_type_print(fp, this->type);
}

//
void t_argdef_codegen(FILE* fp, t_argdef node, ST st);


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
void t_args_print(FILE* fp, t_args this)
{
    if(this == NULL)
        exit(1);

   if(this->as == NULL) {
        fprintf(fp, "[.ARG ");
        t_exp_print(fp, this->exp);
    }

    else {
        fprintf(fp, "[.ARGs [.ARG ");
        t_exp_print(fp, this->exp);
        fprintf(fp, "] ");
        t_args_print(fp, this->as);
    }

    fprintf(fp, "]\n");
}

//
void t_args_codegen(FILE* fp, t_args node, ST st)
{
    if(node == NULL)
        exit(1);

   if(node->as == NULL) {
        t_exp_codegen(node->exp);
        printf("sw $t0, 0($sp)\n");
        printf("addiu $sp, $sp, -4\n");
    }

    else {
        t_args_codegen(node->as);
        t_exp_codegen(node->exp);
        printf("sw $t0, 0($sp)\n");
        printf("addiu $sp, $sp, -4\n");
    }
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
void t_ids_print(FILE* fp, t_ids this)
{
    if(this == NULL)
        exit(1);


    switch(this->kind) {
    case IDS_SINGLE:
        fprintf(fp, "[.ID $%s$ ]", this->id);
        break;
    
    case IDS_LIST:
        fprintf(fp, "[.IDs ");
        fprintf(fp, "[.ID $%s$ ] ", this->id);
        t_ids_print(fp, this->ids);
        fprintf(fp, "]\n");
        break;

    default:
    //TODO
    }

}

//
void t_ids_codegen(FILE* fp, t_ids node, ST st)
{

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
void t_type_print(FILE* fp, t_type this)
{
    if(this == NULL)
        exit(1);

    fprintf(fp, "[.TYPE ");

    switch(this->kind) {
    case T_INT:
        fprintf(fp, "$int$ ");
        break;

    case T_FLOAT:
        fprintf(fp, "$float$ ");
        break;

    case T_STRING:
        fprintf(fp, "$string$ ");
        break;

    case T_BOOL:
        fprintf(fp, "$bool$ ");
        break;

    case T_VOID:
        fprintf(fp, "$void$ ");
        break;

    case T_ID:
        fprintf(fp, "$%s$ ", this->id);
        break;
    
    default:
        fprintf(fp, "[.ARRAY ");
        t_type_print(fp, this->array_type);
        fprintf(fp, "[.SIZE $%d$ ] ]", this->size);
        break;
    }

    fprintf(fp, "]\n");
}

//
void t_type_codegen(FILE* fp, t_type node, ST st)
{

}
