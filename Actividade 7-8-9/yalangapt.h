#ifndef YALANG_APT_H_
#define YALANG_APT_H_

#define OP_LIM 3


typedef struct yalang_t_decls_ *t_decls;

typedef struct yalang_t_decl_ *t_decl;

typedef struct yalang_t_stms_ *t_stms;

typedef struct yalang_t_stm_ *t_stm;

typedef struct yalang_t_exp_ *t_exp;

typedef struct yalang_t_argsdef_ *t_argsdef;

typedef struct yalang_t_argdef_ *t_argdef;

typedef struct yalang_t_args_ *t_args;

typedef struct yalang_t_ids_ *t_ids;

typedef struct yalang_t_type_ *t_type;


/*********************************************************************/


t_decls t_decls_new(t_decl d, t_decls ds);

t_decl t_decl_new_var(t_ids ids, t_type type, t_exp exp);
t_decl t_decl_new_funct(char *id, t_argsdef args, t_type type, t_stms stms);
t_decl t_decl_new_define(char *id, t_type type);

t_stms t_stms_new(t_stm s, t_stms ss);

t_stm t_stm_new_decl(t_decl decl);
t_stm t_stm_new_exp(t_exp exp);
t_stm t_stm_new_ifelse(t_exp exp, t_stm stm1, t_stm stm2);
t_stm t_stm_new_while(t_exp exp, t_stm stm);
t_stm t_stm_new_next();

t_exp t_exp_new_intlit(int i);
t_exp t_exp_new_floatlit(float f);
t_exp t_exp_new_stringlit(char *s);
t_exp t_exp_new_boollit(bool b);
t_exp t_exp_new_id(char *id);
t_exp t_exp_new_op(char op[OP_LIM], t_exp exp1, t_exp exp2);
t_exp t_exp_new_assign(t_exp exp1, t_exp exp2);
t_exp t_exp_new_array(t_exp exp, int pos);
t_exp t_exp_new_funct(char *id, t_args args);

t_ids t_ids_new(char *id, t_ids ids);

t_type t_type_new(char c);
t_type t_type_new_id(char *id);
t_type t_type_new_array(t_type type, int size);

#endif
