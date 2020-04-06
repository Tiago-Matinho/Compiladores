#ifndef YALANG_APT_H_
#define YALANG_APT_H_

#define OP_LIM 3

typedef struct yalang_t_program_ *yalang_t_program;

typedef struct yalang_t_decls_ *yalang_t_decls;

typedef struct yalang_t_decl_ *yalang_t_decl;

typedef struct yalang_t_decl_var_ *yalang_t_decl_var;
typedef struct yalang_t_decl_varinit_ *yalang_t_decl_varinit;
typedef struct yalang_t_decl_funct_ *yalang_t_decl_funct;
typedef struct yalang_t_decl_define_ *yalang_t_decl_define;

typedef struct yalang_t_funct_head_ *yalang_t_funct_head;

typedef struct yalang_t_argdef_ *yalang_t_argdef;
typedef struct yalang_t_argsdef_ *yalang_t_argsdef;

typedef struct yalang_t_args_ *yalang_t_args;

typedef struct yalang_t_stms_ *yalang_t_stms;

typedef struct yalang_t_stm_ *yalang_t_stm;

typedef struct yalang_t_ids_ *yalang_t_ids;
typedef struct yalang_t_type_ *yalang_t_type;

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
	yalang_t_exp e);
yalang_t_decl yalang_decl_funct_new(char *id, yalang_t_type ret_t,
	yalang_t_stms s);
yalang_t_decl yalang_decl_funct_args_new(char *id, yalang_t_type ret_t,
	yalang_t_argsdef a, yalang_t_stms s);
yalang_t_decl yalang_decl_define_new(char *id, yalang_t_type t);

/********************************/

yalang_t_funct_head yalang_funct_head_new(char *id, yalang_t_type ret_t);

/********************************/

yalang_t_argdef yalang_argdef_new(char *id, yalang_t_type t);
yalang_t_argsdef yalang_argsdef_new_empty(yalang_t_argdef); //TODO
yalang_t_argsdef yalang_argsdef_new(yalang_t_argdef a,
	yalang_t_argsdef as);

/********************************/

yalang_t_args yalang_args_new_empty(yalang_t_exp e);
yalang_t_args yalang_args_new(yalang_t_exp e, yalang_t_args a);

/********************************/

yalang_t_stms yalang_stms_new_empty(yalang_t_stm s);
yalang_t_stms yalang_stms_new(yalang_t_stm s, yalang_t_stms ss);

/********************************/

yalang_t_stm yalang_stm_new();

/********************************/

yalang_t_stm yalang_t_stm_decl_new(yalang_t_decl d);
yalang_t_stm yalang_t_stm_exp_new(yalang_t_exp e);
yalang_t_stm yalang_t_stm_return_new(yalang_t_exp e);
yalang_t_stm yalang_t_stm_if_new(yalang_t_exp e, yalang_t_stms s);
yalang_t_stm yalang_t_stm_ifelse_new(yalang_t_exp e, yalang_t_stms s1,
    yalang_t_stms s2);
yalang_t_stm yalang_t_stm_while(yalang_t_exp e, yalang_t_stms s);
yalang_t_stm yalang_t_stm_next(); //TODO

/********************************/

yalang_t_ids yalang_ids_new_empty(char *id);
yalang_t_ids yalang_ids_new_(char *id, yalang_t_ids ids);

/********************************/

yalang_t_type yalang_type_new(char c);
yalang_t_type yalang_type_new_id(char *id);
yalang_t_type yalang_type_new_array(yalang_t_type t, int s);

/********************************/

//TODO
yalang_t_lit yalang_lit_int_new(int i);
yalang_t_lit yalang_lit_float_new(float f);
yalang_t_lit yalang_lit_string_new(char *s);
yalang_t_lit yalang_lit_bool_new(bool b);

/********************************/

//TODO EXP (binop e unop)
yalang_t_exp yalang_exp_new_lit(yalang_t_lit l);
yalang_t_exp yalang_exp_new_id(char *id);
yalang_t_exp yalang_exp_new_arr(yalang_t_exp e, int i);
yalang_t_exp yalang_exp_new_binop(yalang_t_exp e1, char op[OP_LIM], yalang_t_exp e2);
yalang_t_exp yalang_exp_new_unop(char op[OP_LIM], yalang_t_exp e);
yalang_t_exp yalang_exp_new_funct_call(char *id);
yalang_t_exp yalang_exp_new_funct_call_args(char *id, yalang_t_args a);
yalang_t_exp yalang_exp_new_assign(yalang_t_exp e1, yalang_t_exp e2);

#endif
