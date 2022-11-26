#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "sign_table.h"
#include "utils.h"

/* Function to add new table element */
void add_new_table_element(table_element *head, char *sign_to_add, long curr_position, sign_types type) {
	char *temp_element_arr;
	table_element previous_element, current_element, new_element;
	new_element = (table_element)check_malloc(sizeof(sign_table)); /* memory allocation for new table element */
	temp_element_arr = (char *)check_malloc(strlen(sign_to_add)+1); /* memory allocation for new table */
	/* copy new sign to sign table */
	strcpy(temp_element_arr, sign_to_add);
	new_element->sign_flag = temp_element_arr;
	new_element->sign_position = curr_position;
	new_element->type = type;
	/* for empty tables */
	if ((*head) == NULL || (*head)->sign_position > curr_position) {
		new_element->next_element = (*head);
		(*head) = new_element;
		return;
	}
	/* creating new table element */
	current_element = (*head)->next_element;
	previous_element = *head;
	/* sorting */
	while (current_element != NULL && current_element->sign_position < curr_position) {
		previous_element = current_element;
		current_element = current_element->next_element;
	}

	new_element->next_element = current_element;
	previous_element->next_element = new_element;
}

/* Function to find a specific table sign type using va-list */
sign_table *get_table_specific_type(table_element head, char *type_to_get, int counter, ...) {
	int i;
	sign_types *types_to_check_arr = check_malloc((counter)*sizeof(int));
	/* start of va list to store the types in a list */
	va_list types_list;
	va_start(types_list, counter);
	/* storing */
	for (i = 0; i < counter; i++) {
		types_to_check_arr[i] = va_arg(types_list, sign_types);
	}
	va_end(types_list);
	/* for errors */
	if (head == NULL) {
		free(types_to_check_arr);
		return NULL;
	}
	/* let's return the wanted sign type */
	do {
		for (i = 0; i < counter; i++) {
			if (types_to_check_arr[i] == head->type && strcmp(type_to_get, head->sign_flag) == 0) {
				free(types_to_check_arr);
				return head; /* the wanted specific type */
			}
		}
	}
	while ((head = head->next_element) != NULL);
	free(types_to_check_arr);
	return NULL;
}

/* Function to find all table sign types */
table_element get_table_sign_types(table_element head, sign_types type_to_get) {
	table_element new_table = NULL;
	/* start sorting table sign types */
	do {
		if (head->type == type_to_get) {
			add_new_table_element(&new_table, head->sign_flag, head->sign_position, head->type);
		}
	} while ((head = head->next_element) != NULL);
	/* return sorted types table */
	return new_table;
}

/* Function to set tables signs values */
void set_table_sign_position(table_element head, long new_position, sign_types type_to_set) {
	table_element current_element;
    /* set the given sign new position */
	for (current_element = head; current_element != NULL; current_element = current_element->next_element) {
		if (current_element->type == type_to_set) {
			current_element->sign_position += new_position;
		}
	}
}


