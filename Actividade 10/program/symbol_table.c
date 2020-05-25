#include "symbol_table.h"
#define MAX_CONTEXT 1000


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


ST_Bucket st_bucket_new_funct(char *id, t_argsdef args, t_type return_type,
    int scope)
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


ST_Bucket st_bucket_new_var_define(char *id, t_type yatype, int scope)
{
    ST_Bucket ret = (ST_Bucket) malloc(sizeof(*ret));

    ret->id = id;
    ret->scope = scope;
    ret->next = NULL;
    ret->kind = ST_DEFINE;
    ret->u.define.yatype = yatype;

    return ret;
}


/*********************************************************************|
|                              BUCKETS                                |
|*********************************************************************/


ST st_new()
{
    ST ret = (ST) malloc(sizeof(*ret));

    ret->curScope = 0;

    for(int i = 0; i < MAX_CONTEXT; i++)
        ret->array[i] = NULL;

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


void st_insert(ST st, ST_Bucket new)
{
    int key = (int) hash(new->id) % MAX_CONTEXT;

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


ST_Bucket st_lookup(ST st, char *id)
{
    int key = (int) hash(id) % MAX_CONTEXT;

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


void st_new_scope(ST st)
{
    st->curScope++;
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
}


/*********************************************************************|
|                              BUCKETS                                |
|*********************************************************************/


t_type t_exp_ant(t_exp node, ST st)
{
    if(node == NULL)
        exit(1);

    t_type ret;

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
            ST_Bucket search = st_lookup(st, node->u.id);

            if(search == NULL)
                printf("ERROR: %s not previously defined\n", node->u.id);

            switch(search->kind){
                case ST_VAR:
                    ret = search->u.var.yatype;
                    break;

                case ST_FUNCT:
                    ret = search->u.func.return_type;
                    break;

                default:
                    ret = search->u.define.yatype;
                    break;
            }
        break;
    
    case EXP_ARRAY:
        
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


void t_decl_ant(t_decl node, ST st)
{
    if(node == NULL)
        exit(1);

    switch(node->kind) {
        case DECL_VAR:
            t_type type = node->u.var.type;
            t_ids cur_id = node->u.var.ids;
            
            ST_Bucket search, new;

            // ciclo para casos de multiplos ids definidos
            while(cur_id != NULL){
                search = st_lookup(st, cur_id->id); //procura se existe algo com o mesmo nome e scope
                printf("lookup(%s)\n", cur_id->id);

                if(search != NULL){
                    if(search->scope == st->curScope)
                        printf("ERROR: Variable %s already defined\n", cur_id->id); // erro
                }
                else{   // adiciona a symbol table
                    new = st_bucket_new_var(cur_id->id, type, st->curScope);
                    st_insert(st, new);
                    printf("insert(%s)\n", cur_id->id);
                }
                cur_id = cur_id->ids;
            }
            break;

        case DECL_VARINIT:
            t_type type = node->u.varinit.type;
            t_type exp_type = t_exp_ant(node->u.varinit.value, st);

            // TODO comparar exp

            t_ids cur_id = node->u.varinit.ids;

            ST_Bucket search, new;
            
            // ciclo para casos de multiplos ids definidos
            while(cur_id != NULL){
                search = st_lookup(st, cur_id->id); //procura se existe algo com o mesmo nome e scope
                printf("lookup(%s)\n", cur_id->id);

                if(search != NULL){
                    if(search->scope == st->curScope)
                        printf("ERROR: Variable %s already defined\n", cur_id->id); // erro
                }
                else{   // adiciona a symbol table
                    new = st_bucket_new_var(cur_id->id, type, st->curScope);
                    st_insert(st, new);
                    printf("insert(%s)\n", cur_id->id);
                }
                cur_id = cur_id->ids;
            }
            break;

        case DECL_FUNCT_VOID:
            t_argsdef args = node->u.funct.args;
            t_type return_type = node->u.funct.type;
            
            ST_Bucket search, new;
            search = st_lookup(st, node->u.funct.id); //procura se existe algo com o mesmo nome
                printf("lookup(%s)\n", node->u.funct.id);

            if(search != NULL)
                printf("ERROR: Function %s already defined\n", node->u.funct.id);

            else{   // adiciona a symbol table
                new = st_bucket_new_funct(node->u.funct.id, args, return_type, st->curScope);
                st_insert(st, new);
                printf("insert(%s)\n", node->u.funct.id);
            }

            st_new_scope(st);

            t_stms_ant(node->u.funct.stms, st);

            break;

        case DECL_FUNCT:
            t_argsdef args = node->u.funct.args;
            t_type return_type = node->u.funct.type;
            
            ST_Bucket search, new;
            search = st_lookup(st, node->u.funct.id); //procura se existe algo com o mesmo nome
                printf("lookup(%s)\n", node->u.funct.id);

            if(search != NULL)
                printf("ERROR: Function %s already defined\n", node->u.funct.id);

            else{   // adiciona a symbol table
                new = st_bucket_new_funct(node->u.funct.id, args, return_type, st->curScope);
                st_insert(st, new);
                printf("insert(%s)\n", node->u.funct.id);
            }

            st_new_scope(st);

            t_argdef cur_arg;

            while(args != NULL){
                cur_arg = args->a;

                new = st_bucket_new_var(cur_arg->id, cur_arg->type, st->curScope);

                st_insert(st, new);
                printf("insert(%s)\n", cur_arg->id);

                args = args->as;
            }
            t_stms_ant(node->u.funct.stms, st);
            break;
        
        default:
            ST_Bucket search, new;

            search = st_lookup(st, node->u.define.id); //procura se existe algo com o mesmo nome
                printf("lookup(%s)\n", node->u.define.id);

            if(search != NULL)
                printf("ERROR: Function %s already defined\n", node->u.define.id);

            else{   // adiciona a symbol table
                new = st_bucket_new_define(node->u.define.id, node->u.define.type, st->curScope);
                st_insert(st, new);
                printf("insert(%s)\n", node->u.define.id);
            }
            break;
    }

}


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


