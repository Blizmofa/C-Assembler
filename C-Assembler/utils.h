#ifndef _UTILS_H
#define _UTILS_H

#include "structs.h"
#include "sign_table.h"

#define CLEAR_WHITE_SPACES(string, index) \
        for (;string[(index)] && (string[(index)] == '\t' || string[(index)] == ' '); (++(index)))\
        ;

/**
 * Generic project related utility functions.
 */

int error_detection(detect_line_errors line_to_check, char *error_input, ...);

bool signs_validation(detect_line_errors line_to_check, char *sign);

bool sign_name_validation(char* sign);

char *strcat_memory_check(char *str1, char* str2);

sentance_types get_sentence_type_auxiliary(char *type);


/**
 * Dynamic allocated memory related utility functions.
 */

void *check_malloc(long num);

void free_malloc(machine_binary_word **memory, long num);

void free_table_malloc(table_element head);


/**
 * Variables check related utility functions.
 */

bool check_string(char *string_to_check);

bool check_word(char *word_to_check);

bool check_number(char* string_to_check);


/**
 * Instructions check related utility functions.
 */

bool check_jump (char* type);

cpu_registers check_register(char *register_to_check);

#endif
