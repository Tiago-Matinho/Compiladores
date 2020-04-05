#ifndef YALANG_APT_H_
#define YALANG_APT_H_

typedef struct yalang_t_decls_ *yalang_t_decls;

typedef struct yalang_t_decl_ *yalang_t_decl;

typedef struct yalang_t_vars_ *yalang_t_vars;
typedef struct yalang_t_varsinit_ *yalang_t_varsinit;

typedef struct yalang_t_func_head_ *yalang_t_func_head;

typedef struct yalang_t_ids_ *yalang_t_ids;
typedef struct yalang_t_type_ *yalang_t_type;
typedef struct yalang_t_value_ *yalang_t_value;

typedef struct yalang_t_lit_ *yalang_t_lit;


yalang_t_decls yalang_decls_new_empty(yalang_t_decl d);
yalang_t_decls yalang_decls_new(yalang_t_decl d, yalang_t_decls ds);

yalang_t_decl yalang_decl_new_vars(yalang_t_vars v);
yalang_t_decl yalang_decl_new_varsinit(yalang_t_varsinit v);
yalang_t_decl yalang_decl_new_func_void();
// TODO yalang_t_decl yalang_decl_new_func_args();
// TODO yalang_t_decl yalang_decl_new_define();

yalang_t_var yalang_vars_new(yalang_t_ids ids, yalang_t_type t);
yalang_t_varinit yalang_varsinit_new(yalang_t_vars vars,
    yalang_t_value val);

yalang_t_ids yalang_ids_new_empty(char *id);
yalang_t_ids yalang_ids_new(char *id, yalang_t_ids ids);
yalang_t_type yalang_type_new_int();
yalang_t_type yalang_type_new_float();
yalang_t_type yalang_type_new_string();
yalang_t_type yalang_type_new_bool();
yalang_t_type yalang_type_new_id(char *id);
yalang_t_type yalang_type_new_array(yalang_t_type t, int s);
yalang_t_value yalang_value_new();

#endif
