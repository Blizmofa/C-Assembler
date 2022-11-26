#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "instructions_analyzer.h"
#include "second_scan.h"

#define START_FROM 100

/* Function that encodes another code word on the second pass */
int encode_second_word_second_scan(detect_line_errors line_to_check, long *IC_POINTER, long *IC_COUNTER, char *given_operand, machine_binary_word **code_word_pointer, table_element *sign_table_pointer) {
	addressing_methods addr = get_addressing_type(given_operand);
	machine_binary_word *output_word; /* the code word to be written */
	/* increasing IC according to addressing methods */
	if (addr == immediate_addr_mode) {
        (*IC_POINTER)++;
	}
	if (addr == relative_addr_mode) {
        given_operand++;
	}
	if (direct_addr_mode == addr || relative_addr_mode == addr) {
		long extra_data_word_content; /* variable for the content of the extra date word */
		sign_table *head = get_table_specific_type(*sign_table_pointer, given_operand, 3, data_sign, other_code_sign, external_sign);
		/* for errors */
		if (head == NULL) {
			error_detection(line_to_check, "Error! can't find sign.");
			return FALSE;
		}
		extra_data_word_content = head->sign_position;
		if (addr == relative_addr_mode) {
			/* for errors */
			if (head->type != other_code_sign) {
				error_detection(line_to_check, "Error! invalid code sign.");
				return FALSE;
			}
			extra_data_word_content = extra_data_word_content-*IC_COUNTER;
		}
		/* 6. increase IC if sign is external */
		if (head->type == external_sign) {
			add_new_table_element(sign_table_pointer, given_operand, (*IC_POINTER) + 1, external_pointer);
		}
		/* encode and set IC counter value*/
		output_word = (machine_binary_word *)check_malloc(sizeof(machine_binary_word));
		output_word->binary_word_length = 0;
		output_word->word_to_encode.data_word = encode_are_value(addr, head->type == external_sign);
		code_word_pointer[(++(*IC_POINTER))-START_FROM] = output_word;

	}
	return TRUE;
}

/* Function to analyze code line on the second scan, according to the suggested algorithm in Course booklet */
bool second_scan_line_analyzer(detect_line_errors line_to_check, long *IC_COUNTER, machine_binary_word **code_word_pointer, table_element *sign_table_pointer) {
    long i = 0;
	char *comments_check_array; /* array to check code comments */
	char *flag;
	CLEAR_WHITE_SPACES(line_to_check.line_input, i)
	/* check for empty or commented out lines */
	if (line_to_check.line_input[i] == ';' || line_to_check.line_input[i] == '\n') {
        return TRUE;
	}
	comments_check_array = strchr(line_to_check.line_input, ':');
	/* 2. check for labels */
	if (comments_check_array != NULL) {
		i = comments_check_array - line_to_check.line_input;
		i++;
	}
	CLEAR_WHITE_SPACES(line_to_check.line_input, i)
	/* 3. for sentence type */
	if (line_to_check.line_input[i] == '.') {
		/* 4. for ".entry" type */
		if (strncmp(".entry", line_to_check.line_input, 6) == 0) {
			i += 6;
			CLEAR_WHITE_SPACES(line_to_check.line_input, i)
			flag = strtok(line_to_check.line_input + i, " \n\t");
			/* for errors */
			if (flag == NULL) {
				error_detection(line_to_check, "Error! unspecified label name for .entry type.");
				return FALSE;
			}
			if (get_table_specific_type(*sign_table_pointer, flag, 1, entry_sign) == NULL) {
				sign_table *entry;
				flag = strtok(line_to_check.line_input + i, "\n"); /* initialize flag to pointed sign name */
				if (flag[0] == '%') {
                    flag++;
				}
				/* error check */
				if ((entry = get_table_specific_type(*sign_table_pointer, flag, 2, data_sign, other_code_sign)) == NULL) {
					if ((entry = get_table_specific_type(*sign_table_pointer, flag, 1, external_sign)) != NULL) {
						error_detection(line_to_check, "Error! aliasing signs error! check sign.");
						return FALSE;
					}
					error_detection(line_to_check, "Error! unspecified sign %s for .entry type.");
					return FALSE;
				}
				/* 5. encoding and adds new sign to table if scan finished with no errors*/
				add_new_table_element(sign_table_pointer, flag, entry->sign_position, entry_sign);
			}
		}
		return TRUE;
	}
	return encode_extra_data_word(line_to_check, IC_COUNTER, code_word_pointer, sign_table_pointer);
}

/* Function to encode the extra operands data word according to suggested second pass algorithm section 6 in Course booklet */
bool encode_extra_data_word(detect_line_errors line_to_check, long *IC_COUNTER, machine_binary_word **code_word_pointer, table_element *sign_table_pointer) {
    int i = 0, counter;
    int word_length = code_word_pointer[(*IC_COUNTER)-START_FROM]->binary_word_length; /* initialize word length variable to current IC value */
    long IC_POINTER = *IC_COUNTER;
	char code_instructions_arr[80];
	char *operands[2];
	bool flag = TRUE;
	/* IC has only the word */
	if (word_length > 1) {
		CLEAR_WHITE_SPACES(line_to_check.line_input, i)
		signs_validation(line_to_check, code_instructions_arr);
		/* check for signs */
		if (code_instructions_arr[0] != '\0') {
			for (; line_to_check.line_input[i] && line_to_check.line_input[i] != '\n' && line_to_check.line_input[i] != EOF && line_to_check.line_input[i] != ' ' && line_to_check.line_input[i] != '\t'; i++);
			i++;
		}
		CLEAR_WHITE_SPACES(line_to_check.line_input, i)
        /* checks the lines until end of file */
		for (; line_to_check.line_input[i] && line_to_check.line_input[i] != ' ' && line_to_check.line_input[i] != '\t' && line_to_check.line_input[i] != '\n' && line_to_check.line_input[i] != EOF; i++);
		CLEAR_WHITE_SPACES(line_to_check.line_input, i)
		/* analyzing operands */
		instruction_analyzer(line_to_check, i, operands, &counter, NULL);
		/* for errors */
		if (counter--) {
			flag = encode_second_word_second_scan(line_to_check, &IC_POINTER, IC_COUNTER, operands[0], code_word_pointer, sign_table_pointer);
			free(operands[0]);
			if (!flag) {
                return FALSE;
			}
			if (counter) {
				flag = encode_second_word_second_scan(line_to_check, &IC_POINTER, IC_COUNTER, operands[1], code_word_pointer, sign_table_pointer);
				free(operands[1]);
				if (!flag) return FALSE;
			}
		}
	}
	/* increase IC counter */
	(*IC_COUNTER) = (*IC_COUNTER)+word_length;
	return TRUE;
}
