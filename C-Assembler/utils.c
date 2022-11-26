#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "utils.h"
#include "instructions_analyzer.h"

#define LINE_LENGTH 80

/* Function to detect errors along the entire project with va_list, mainly for cleaner code and less code lines */
int error_detection(detect_line_errors line_to_check, char *error_input, ...) {
	int print_output;
	va_list errors_list;
	fprintf(stderr,"Error! in %s: %d: ", line_to_check.name_of_file, line_to_check.num_of_line); /* make it easier to find the error location */
	/* start of va_list */
	va_start(errors_list, error_input);
	print_output = vfprintf(stderr, error_input, errors_list);
	va_end(errors_list);
    /* end of va_list */
	fprintf(stderr, "\n");
	return print_output;
}

/* check for errors and validates signs*/
bool signs_validation(detect_line_errors line_to_check, char *sign) {
	int i, j;
	i = j = 0;
	CLEAR_WHITE_SPACES(line_to_check.line_input, i)
	/* loop to run on all the lines until the end of file */
	for (; line_to_check.line_input[i] && line_to_check.line_input[i] != ':' && line_to_check.line_input[i] != EOF && i <= LINE_LENGTH; i++, j++) {
		sign[j] = line_to_check.line_input[i];
	}
	sign[j] = '\0';
	/* for errors */
	if (line_to_check.line_input[i] == ':') {
		if (!sign_name_validation(sign)) { /* if not a valid sign name */
			error_detection(line_to_check, "Error! invalid sign name");
			sign[0] = '\0';
			return TRUE; /* errors found, validation has failed */
		}
		return FALSE;
	}
	sign[0] = '\0';
	/* no errors found, validation has succeeded */
	return FALSE;
}

/* Auxiliary function to help validates the sign name according to string definition */
bool sign_name_validation(char *sign) {
	return sign[0] && strlen(sign) <= 31 && isalpha(sign[0]) && check_string(sign+1) && !check_word(sign);
}

/* allocating new memory for strcat strings, mainly for cleaner code and less code lines */
char *strcat_memory_check(char *str1, char* str2) {
	char *str_to_allocate = (char *)check_malloc(strlen(str1)+strlen(str2)+1);
	strcpy(str_to_allocate, str1);
	strcat(str_to_allocate, str2);

	return str_to_allocate;
}

/* Struct for sentence types settings for utils function to use from */
struct sentance_types_element sentance_types_table[] = {
		{"string", string_type},
		{"data",   date_type},
		{"entry",  entry_type},
		{"extern", extern_type},
		{NULL, sentance_type_null}
};

/* Auxiliary function to help get the wanted sentence type */
sentance_types get_sentence_type_auxiliary(char *type) {
	struct sentance_types_element *element;
	for (element = sentance_types_table; element->type != NULL; element++) {
		if (strcmp(element->type, type) == 0) {
			return element->type_value;
		}
	}
	return sentance_type_null; /* in case we didn't find any */
}

/* Function to free dynamic allocated memory */
void *check_malloc(long num) {
	void *ptr = malloc(num);
	if (ptr == NULL) {
		printf("Error! memory allocation failed.");
		exit(1);
	}
	return ptr;
}

/* Function to free dynamic allocated memory, mainly for cleaner code */
void free_malloc(machine_binary_word **memory, long num) {
	long i;
	for (i = 0; i < num; i++) {
		machine_binary_word *word_to_free = memory[i];
		if (word_to_free != NULL) {
			if (word_to_free->binary_word_length > 0) {
				free(word_to_free->word_to_encode.code_word);
			} else {
				free(word_to_free->word_to_encode.data_word);
			}
			free(word_to_free);
			memory[i] = NULL;
		}
	}
}

/* Function to free table dynamic allocated memory */
void free_table_malloc(table_element head) {
	table_element prev_entry, curr_entry = head;

	while (curr_entry != NULL) {
		prev_entry = curr_entry;
		curr_entry = curr_entry->next_element;
		free(prev_entry->sign_flag);
		free(prev_entry);
	}
}

/* Function that checks if a given string is alphanumeric */
bool check_string(char *string_to_check) {
	int i;
	for (i = 0; string_to_check[i]; i++) {
		if (!isalpha(string_to_check[i]) && !isdigit(string_to_check[i])) {
            return FALSE;
		}
	}
	return TRUE;
}

/* Function that checks if a given word is a reserved word */
bool check_word(char *word_to_check) {
	int fun, opc;
	/* check if register or command */
	opcode_or_funct_value(word_to_check, &opc, (funct *) &fun);
	if (opc != opcode_null || check_register(word_to_check) != reg_null || get_sentence_type_auxiliary(word_to_check) != sentance_type_null) {
        return TRUE;
	}
	return FALSE;
}

/* Function that checks if a given string is a valid number */
bool check_number(char *string_to_check) {
	int i = 0;
	if (string_to_check[0] == '-' || string_to_check[0] == '+') {
        string_to_check++;
	}
	for (; string_to_check[i]; i++) {
		if (!isdigit(string_to_check[i])) {
			return FALSE;
		}
	}
	/* for empty string */
	return i > 0;
}

/* Function to calculate command jumps according to instructions */
bool check_jump (char* type) {
    return type[0] - (type[1]+1);
}

/* Function to check a register name */
cpu_registers check_register(char *register_to_check) {
	if (register_to_check[0] == 'r' && isdigit(register_to_check[1]) && register_to_check[2] == '\0') {
		int register_digit = register_to_check[1] - '0';
		if (register_digit >= 0 && register_digit <= 7) {
            return register_digit;
		}
	}
	/* didn't find any, for errors */
	return reg_null;
}

