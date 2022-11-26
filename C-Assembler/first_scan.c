#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "utils.h"
#include "instructions_analyzer.h"
#include "first_scan.h"

#define LINE_LENGTH 80
#define START_FROM 100

/* Function that encodes another code word on the first pass */
void encode_second_word_first_scan(machine_binary_word **code_word_pointer, long *IC_COUNTER, char *sign) {
	addressing_methods operand_addressing = get_addressing_type(sign);
	/* encoding according to addressing methods and increases IC counter */
	if (operand_addressing != addr_mode_null && operand_addressing != register_direct_addr_mode) {
		(*IC_COUNTER)++;
		if (operand_addressing == immediate_addr_mode) {
			machine_binary_word *word_to_write;
			word_to_write = (machine_binary_word *)check_malloc(sizeof(machine_binary_word));
			word_to_write->binary_word_length = 0; /* for errors */
			(word_to_write->word_to_encode).data_word = encode_are_value(immediate_addr_mode, FALSE);

			code_word_pointer[(*IC_COUNTER)-START_FROM] = word_to_write;
		}
	}
}

/* Function to analyze code line in general */
bool line_analyzer(detect_line_errors line_to_check, int index, long *IC_COUNTER, machine_binary_word **code_word_pointer) {
    int i, counter;
    long IC_POINTER;
	char code_instructions_arr[8]; /* array to store code instructions for the line analyzer */
	char *operands[2];
	/* opcode and funct value pointers */
	opcode opcode_pointer;
	funct funct_pointer;
	first_machine_word_bytes *word_to_analyze; /* the code word to be checked */
	machine_binary_word *output_word;
	CLEAR_WHITE_SPACES(line_to_check.line_input, index)
	/* check line for errors */
	for (i = 0; line_to_check.line_input[index] && line_to_check.line_input[index] != '\t' && line_to_check.line_input[index] != ' ' && line_to_check.line_input[index] != '\n' && line_to_check.line_input[index] != EOF && i < 6; index++, i++) {
		code_instructions_arr[i] = line_to_check.line_input[index];
	}
	code_instructions_arr[i] = '\0';
	/* write the value of opcode or funct and check for errors */
	opcode_or_funct_value(code_instructions_arr, &opcode_pointer, &funct_pointer);
	if (opcode_pointer == opcode_null) {
		error_detection(line_to_check, "Error! unknown opcode or funct value.");
		return FALSE;
	}
	/* check operands count according to table */
	if (!instruction_analyzer(line_to_check, index, operands, &counter, code_instructions_arr))  {
		return FALSE;
	}
	/* encodes code word and free allocated memory*/
	if ((word_to_analyze = encode_line(line_to_check, opcode_pointer, funct_pointer, counter, operands)) == NULL) {
		if (operands[0]) {
			free(operands[0]);
			if (operands[1]) {
				free(operands[1]);
			}
		}
		return FALSE;
	}
	/* sets the counter to the new encoded code word */
	IC_POINTER = *IC_COUNTER;
	/* new wanted output word */
	output_word = (machine_binary_word *) check_malloc(sizeof(machine_binary_word));
	(output_word->word_to_encode).code_word = word_to_analyze;
	code_word_pointer[(*IC_COUNTER)-START_FROM] = output_word;
	/* encodes the second code word and free allocated memory */
	if (counter--) { /* for 1 operand */
		encode_second_word_first_scan(code_word_pointer, IC_COUNTER, operands[0]);
		free(operands[0]);
		/* for 2 operands */
		if (counter) {
			encode_second_word_first_scan(code_word_pointer, IC_COUNTER, operands[1]);
			free(operands[1]);
		}
	}
    /* increase IC counter */
	(*IC_COUNTER)++;
	code_word_pointer[IC_POINTER-START_FROM]->binary_word_length = (*IC_COUNTER)-IC_POINTER;
	return TRUE;
}

/* Function to analyze code line on the first scan, according to the suggested algorithm in Course booklet */
bool first_scan_line_analyzer(detect_line_errors line_to_check, long *IC_COUNTER, long *DC_COUNTER, machine_binary_word **code_word_pointer, long *data_word_pointer, table_element *sign_table_pointer) {
	int i, j;
	char sign_table_arr[LINE_LENGTH];
	sentance_types sentance_type;
	i = 0;
	CLEAR_WHITE_SPACES(line_to_check.line_input, i)
	/* check for empty or commented out lines */
	if (!line_to_check.line_input[i] || line_to_check.line_input[i] == '\n' || line_to_check.line_input[i] == EOF || line_to_check.line_input[i] == ';') {
		return TRUE;
	}
	/* 4. check for labels */
	if (signs_validation(line_to_check, sign_table_arr)) {
		return FALSE;
	}
	/* check if sign name is valid */
	if (sign_table_arr[0] && !sign_name_validation(sign_table_arr)) {
		error_detection(line_to_check, "Error! invalid sign name.");
		return FALSE;
	}
	/* if valid start to analyze */
	if (sign_table_arr[0] != '\0') {
		for (; line_to_check.line_input[i] != ':'; i++);
		i++;
	}
	CLEAR_WHITE_SPACES(line_to_check.line_input, i)
    /* check for empty line */
	if (line_to_check.line_input[i] == '\n') {
        return TRUE;
	}
	/* 5. check if sign is already exists */
	if (get_table_specific_type(*sign_table_pointer, sign_table_arr, 3, external_sign, data_sign, other_code_sign)) {
		error_detection(line_to_check, "Error! sign is already exists.");
		return FALSE;
	}
	/* check for sentence types error */
	sentance_type = get_sentence_type(line_to_check, &i);
	if (sentance_type == sentance_type_error) {
		return FALSE;
	}
	CLEAR_WHITE_SPACES(line_to_check.line_input, i)
	/* 6 - 16 validate the sentence type for first pass, encode it and increase DC counter */
	if (sentance_type != sentance_type_null) {
		/* for ".data" or ".string" types */
		if ((sentance_type == date_type || sentance_type == string_type) && sign_table_arr[0] != '\0') {
			add_new_table_element(sign_table_pointer, sign_table_arr, *DC_COUNTER, data_sign);
		}
		/* only for ".string" type */
		if (sentance_type == string_type) {
			return analyze_string_to_code_line(line_to_check, i, data_word_pointer, DC_COUNTER);
		}
		/* only for ".data" type */
		else if (sentance_type == date_type) {
			return analyze_data_to_code_line(line_to_check, i, data_word_pointer, DC_COUNTER);
		}
		/* for ".extern" type */
		else if (sentance_type == extern_type) {
			CLEAR_WHITE_SPACES(line_to_check.line_input, i)
			for (j = 0; line_to_check.line_input[i] && line_to_check.line_input[i] != '\n' && line_to_check.line_input[i] != '\t' && line_to_check.line_input[i] != ' ' && line_to_check.line_input[i] != EOF; i++, j++) {
				sign_table_arr[j] = line_to_check.line_input[i];
			}
			sign_table_arr[j] = 0;
			/* for errors */
			if (!sign_name_validation(sign_table_arr)) {
				error_detection(line_to_check, "Error! invalid external sign.");
				return TRUE;
			}
			/* counting stops because of the error */
			add_new_table_element(sign_table_pointer, sign_table_arr, 0, external_sign);
		}
		else if (sentance_type == entry_type && sign_table_arr[0] != '\0') {
			error_detection(line_to_check, "Error! invalid entry sign.");
			return FALSE;
		}
	}
	/* if no errors found add signs to sign table, analyze and increase counter */
	else {
		if (sign_table_arr[0] != '\0') {
			add_new_table_element(sign_table_pointer, sign_table_arr, *IC_COUNTER, other_code_sign);
		}
		return line_analyzer(line_to_check, i, IC_COUNTER, code_word_pointer);
	}
	return TRUE;
}
