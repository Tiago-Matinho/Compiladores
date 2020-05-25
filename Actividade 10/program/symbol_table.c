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

ST st;

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

/*
ST st_new()
{
    ST ret = (ST) malloc(sizeof(*ret));

    ret->curScope = 0;

    for(int i = 0; i < MAX_CONTEXT; i++)
        ret->array[i] = NULL;

    return ret;
}
*/

unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}


void st_insert(ST_Bucket new)
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


ST_Bucket st_lookup(char *id)
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


t_type st_lookup_type(char *id)
{
    ST_Bucket search = st_lookup(id);

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


void st_new_scope()
{
    st->curScope++;
}


void st_drop_scope()
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


t_type t_exp_ant(t_exp node)
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
            ret = st_lookup_type(node->u.id);

            if(ret == NULL)
                printf("ERROR: %s not previously defined\n", node->u.id);            
            break;
    
        case EXP_ARRAY:
            ret = t_exp_ant(node->u.array.exp);
            break;
    
        case EXP_BINOP:
            t_type l_type = t_exp_ant(node->u.binop.exp1);
            t_type r_type = t_exp_ant(node->u.binop.exp2);

            if(!compatible_types(l_type, r_type))
                printf("ERROR: incompatible types\n");

            ret = l_type;
            break;
    
    case EXP_UNOP:
            // TODO verificar se operação é válida
            ret = t_exp_ant(node->u.unop.exp);
            break;
    
    case EXP_ASSIGN:
            t_type l_type = t_exp_ant(node->u.assign.exp1);
            t_type r_type = t_exp_ant(node->u.assign.exp2);

            if(!compatible_types(l_type, r_type))
                printf("ERROR: incompatible types\n");

            ret = l_type;
            break;
    
    default:
            break;
    }

    printf("]\n");
}


void t_stm_ant(t_stm node)
{
    if(node == NULL)
        exit(1);

    t_type boolean = t_type_new('b');
    
    switch(node->kind){
        case STM_DECL:
            t_decl_ant(node->u.decl);
            break;
        case STM_EXP:
            t_exp_ant(node->u.exp);
            break;
        case STM_RET:
            t_exp_ant(node->u.exp);
            break;
        case STM_IF:
            if(!compatible_types(boolean, t_exp_ant(node->u._if.exp)))
                printf("ERROR: expression not boolean\n");
            t_stms_ant(node->u._if.stms);
            break;
        case STM_IFELSE:
            if(compatible_types(boolean, t_exp_ant(node->u._ifelse.exp)))
                printf("ERROR: expression not boolean\n");
            t_stms_ant(node->u._ifelse.stms1);
            t_stms_ant(node->u._ifelse.stms2);
            break;
        case STM_WHILE:
            if(compatible_types(boolean, t_exp_ant(node->u._while.exp)))
                printf("ERROR: expression not boolean\n");
            t_stms_ant(node->u._while.stms);
        default:
            break;
            
    }
}


void t_stms_ant(t_stms node)
{
    if(node == NULL)
        exit(1);
    
    switch(node->kind){
        case STMS_SINGLE:
            t_stm_ant(node->s);
            break;
    
    default:
        t_stm_ant(node->s);
        t_stms_ant(node->ss);
        break;
    }
}


void t_decl_ant(t_decl node)
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
                search = st_lookup(cur_id->id); //procura se existe algo com o mesmo nome e scope
                printf("lookup(%s)\n", cur_id->id);

                if(search != NULL){
                    if(search->scope == st->curScope)
                        printf("ERROR: Variable %s already defined\n", cur_id->id); // erro
                }
                else{   // adiciona a symbol table
                    new = st_bucket_new_var(cur_id->id, type, st->curScope);
                    st_insert(new);
                    printf("insert(%s)\n", cur_id->id);
                }
                cur_id = cur_id->ids;
            }
            break;

        case DECL_VARINIT:
            t_type type = node->u.varinit.type;
            t_type exp_type = t_exp_ant(node->u.varinit.value);

            // TODO comparar exp

            t_ids cur_id = node->u.varinit.ids;

            ST_Bucket search, new;
            
            // ciclo para casos de multiplos ids definidos
            while(cur_id != NULL){
                search = st_lookup(cur_id->id); //procura se existe algo com o mesmo nome e scope
                printf("lookup(%s)\n", cur_id->id);

                if(search != NULL){
                    if(search->scope == st->curScope)
                        printf("ERROR: Variable %s already defined\n", cur_id->id); // erro
                }
                else{   // adiciona a symbol table
                    new = st_bucket_new_var(cur_id->id, type, st->curScope);
                    st_insert(new);
                    printf("insert(%s)\n", cur_id->id);
                }
                cur_id = cur_id->ids;
            }
            break;

        case DECL_FUNCT_VOID:
            t_argsdef args = node->u.funct.args;
            t_type return_type = node->u.funct.type;
            
            ST_Bucket search, new;
            search = st_lookup(node->u.funct.id); //procura se existe algo com o mesmo nome
                printf("lookup(%s)\n", node->u.funct.id);

            if(search != NULL)
                printf("ERROR: Function %s already defined\n", node->u.funct.id);

            else{   // adiciona a symbol table
                new = st_bucket_new_funct(node->u.funct.id, args, return_type, st->curScope);
                st_insert(new);
                printf("insert(%s)\n", node->u.funct.id);
            }

            st_new_scope(st);

            t_stms_ant(node->u.funct.stms);

            break;

        case DECL_FUNCT:
            t_argsdef args = node->u.funct.args;
            t_type return_type = node->u.funct.type;
            
            ST_Bucket search, new;
            search = st_lookup(node->u.funct.id); //procura se existe algo com o mesmo nome
                printf("lookup(%s)\n", node->u.funct.id);

            if(search != NULL)
                printf("ERROR: Function %s already defined\n", node->u.funct.id);

            else{   // adiciona a symbol table
                new = st_bucket_new_funct(node->u.funct.id, args, return_type, st->curScope);
                st_insert(new);
                printf("insert(%s)\n", node->u.funct.id);
            }

            st_new_scope(st);

            t_argdef cur_arg;

            while(args != NULL){
                cur_arg = args->a;

                new = st_bucket_new_var(cur_arg->id, cur_arg->type, st->curScope);

                st_insert(new);
                printf("insert(%s)\n", cur_arg->id);

                args = args->as;
            }
            t_stms_ant(node->u.funct.stms);
            break;
        
        default:
            ST_Bucket search, new;

            search = st_lookup(node->u.define.id); //procura se existe algo com o mesmo nome
                printf("lookup(%s)\n", node->u.define.id);

            if(search != NULL)
                printf("ERROR: Function %s already defined\n", node->u.define.id);

            else{   // adiciona a symbol table
                new = st_bucket_new_define(node->u.define.id, node->u.define.type, st->curScope);
                st_insert(new);
                printf("insert(%s)\n", node->u.define.id);
            }
            break;
    }

}


void t_decls_ant(t_decls node)
{
    if(node == NULL)
        exit(1);
    
    switch(node->kind){
        case DECLS_SINGLE:
            t_decl_ant(node->d);
            break;
    
    default:
        t_decl_ant(node->d);
        t_decls_ant(node->ds);
        break;
    }
}


bool compatible_types(t_type l_type, t_type r_type)
{
    switch(l_type->kind){
        case T_INT:
        case T_FLOAT:
            switch(r_type->kind){
                case T_INT:
                case T_FLOAT:
                    return true;
                case T_STRING:
                case T_BOOL:
                case T_VOID:
                    return false;
                case T_ID:
                    t_type search = st_lookup_type(r_type->id);

                    if(search == NULL){
                        printf("ERROR: %s not previously defined\n");
                        return false;
                    }
                    return compatible_types(l_type, search);
                case T_ARRAY:
                    return compatible_types(l_type, r_type->array_type);
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
                    return true;
                case T_BOOL:
                case T_VOID:
                    return false;
                case T_ID:
                    t_type search = st_lookup_type(r_type->id);

                    if(search == NULL){
                        printf("ERROR: %s not previously defined\n");
                        return false;
                    }
                    return compatible_types(l_type, search);
                case T_ARRAY:
                    return compatible_types(l_type, r_type->array_type);
                default:
                    printf("ERROR: wrong type @ compatible_types\n");
                    exit(1);
                    return false;
            }
        case T_BOOL:
            switch(r_type->kind){
                case T_INT:
                case T_BOOL:
                    return true;
                case T_FLOAT:
                case T_STRING:
                case T_VOID:
                    return false;
                case T_ID:
                    t_type search = st_lookup_type(r_type->id);

                    if(search == NULL){
                        printf("ERROR: %s not previously defined\n");
                        return false;
                    }
                    return compatible_types(l_type, search);
                case T_ARRAY:
                    return compatible_types(l_type, r_type->array_type);
                default:
                    printf("ERROR: wrong type @ compatible_types\n");
                    exit(1);
                    return false;
            }
        case T_VOID:
            return true;
        case T_ID:
            t_type search = st_lookup_type(l_type->id);

            if(search == NULL){
                printf("ERROR: %s not previously defined\n");
                return false;
            }
            return compatible_types(search, r_type);
        case T_ARRAY:
            return compatible_types(l_type->array_type, r_type);
        default:
            printf("ERROR: wrong type @ compatible_types\n");
            exit(1);
            return false;
    }
}

