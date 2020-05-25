#include "symbol_table.h"

typedef struct st_data_
{
    enum {ST_VAR, ST_FUNC, ST_TYPE} kind;

    union {
        struct {
            t_type yatype; /* _Type pode ser o tipo definido na análisesintáctica */

            enum {VARloc, VARarg} kind;
            /*. . .  mais tarde, precisaremos de mais info*/
        } var;

        struct {
            t_type yatype; /* tipo de retorno */
            t_args arg; /*"lista" de tipos dos argumentos, porordem*/
            /*. . .  mais tarde, precisaremos de mais info*/
        } func;
        
        t_type type; /*para este caso só precisamos do tipo destino*/
    } u;
};