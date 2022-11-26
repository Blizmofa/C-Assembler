#ifndef _SIGN_TABLE_H
#define _SIGN_TABLE_H

#include "structs.h"

/**
* Generic table functions to use when needed throughout the project.
* All functions have indicative names.
*
* Function "add_new_table_element" adds new element to table.
* Function "get_table_specific_type" returns the wanted sign type.
* Function "get_table_sign_types" returns the tables sign types.
* Function "set_table_sign_position" sets sign position in table.
*/

void add_new_table_element(table_element *head, char *sign_to_add, long curr_position, sign_types type);

sign_table *get_table_specific_type(table_element head, char *type_to_get, int counter, ...);

table_element get_table_sign_types(table_element head, sign_types type_to_get);

void set_table_sign_position(table_element head, long new_position, sign_types type_to_set);

#endif
