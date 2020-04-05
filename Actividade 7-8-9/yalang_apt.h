#ifndef YALANG_APT_H_
#define YALANG_APT_H_

typedef struct yalang_t_program_ *yalang_t_program;

typedef struct yalang_t_decls_ *yalang_t_decls;

typedef struct yalang_t_decl_ *yalang_t_decl;

typedef struct yalang_t_decl_var_ *yalang_t_decl_var;
typedef struct yalang_t_decl_varinit_ *yalang_t_decl_varinit;
typedef struct yalang_t_decl_funct_ *yalang_t_decl_funct;
typedef struct yalang_t_decl_define_ *yalang_t_decl_define;

typedef struct yalang_t_funct_head_ *yalang_t_funct_head;

typedef struct yalang_t_argsdef_ *yalang_t_argsdef;

typedef struct yalang_t_stms_ *yalang_t_stms;

typedef struct yalang_t_stm_ *yalang_t_stm;

typedef struct yalang_t_ids_ *yalang_t_ids;
typedef struct yalang_t_type_ *yalang_t_type;
typedef struct yalang_t_value_ *yalang_t_value;

typedef struct yalang_t_exp_ *yalang_t_exp;

typedef struct yalang_t_unop_ *yalang_t_unop;
typedef struct yalang_t_biop_ *yalang_t_biop;

typedef struct yalang_t_lit_ *yalang_t_lit;

/*********************************************************************/

yalang_t_program yalang_program_empty();
yalang_t_program yalang_program_new(yalang_t_decls ds);

/********************************/

yalang_t_decls yalang_decls_empty(yalang_t_decl d);
yalang_t_decls yalang_decls_new(yalang_t_decl d, yalang_t_decls ds);

/********************************/

yalang_t_decl yalang_decl_new();

/********************************/

yalang_t_decl yalang_decl_var_new(yalang_t_ids i, yalang_t_type t);
yalang_t_decl yalang_decl_varinit_new(yalang_t_ids i, yalang_t_type t,
	yalang_t_value v);
yalang_t_decl yalang_decl_funct_new(char *id, yalang_t_type ret_t,
	yalang_t_stms s);
yalang_t_decl yalang_decl_funct_args_new(char *id, yalang_t_type ret_t,
	yalang_t_argsdef a, yalang_t_stms s);
yalang_t_decl yalang_decl_define_new(char *id, yalang_t_type t);

/********************************/

yalang_t_funct_head yalang_funct_head_new();

/********************************/

yalang_t_argsdef yalang_argsdef_new_empty(char *id, yalang_t_type t);
yalang_t_argsdef yalang_argsdef_new(char *id, yalang_t_type t,
	yalang_t_argsdef a);

/********************************/

yalang_t_stms yalang_stms_new();

#endif
