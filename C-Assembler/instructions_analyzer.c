#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "instructions_analyzer.h"
#include "utils.h"
#include "structs.h"

#define LINE_LENGTH 80

/* Struct table to get opcode or funct value */
static struct opcode_or_funct_element opcode_funct_table[] = {
		{"mov", mov_opcode, funct_null},
		{"cmp",cmp_opcode, funct_null},
		{"add",add_opcode, add_funct},
		{"sub",sub_opcode, sub_funct},
		{"lea",lea_opcode, funct_null},
		{"clr",clr_opcode, clr_funct},
		{"not",not_opcode, not_funct},
		{"inc",inc_opcode, inc_funct},
		{"dec",dec_opcode, dec_funct},
		{"jmp",jmp_opcode, jmp_funct},
		{"bne",bne_opcode, bne_funct},
		{"jsr",jsr_opcode, jsr_funct},
		{"red",red_opcode, funct_null},
		{"prn",prn_opcode, funct_null},
		{"rts",rts_opcode, funct_null},
		{"stop",stop_opcode, funct_null},
		{NULL, opcode_null, funct_null}
};

/* Function to get opcode or funct value based on the above struct table */
void opcode_or_funct_value(char *element, opcode *opc, funct *fun) {
	struct opcode_or_funct_element *ptr;
	*opc = opcode_null;
	*fun = funct_null;
    /* let's distinguish between opcodes and functs with the same value */
	for (ptr = opcode_funct_table; ptr->element != NULL; ptr++) {
		if (strcmp(ptr->element, element) == 0) {
			*opc = ptr->wanted_opcode_value;
			*fun = ptr->wanted_funct_value;
			return;
		}
	}
}

/* Function to get the wanted addressing method according to the given instructions */
addressing_methods get_addressing_type(char *address_type) {
	if (address_type[0] == '\0') { /* empty array check */
        return addr_mode_null;
	}
	if (address_type[0] == 'r' && address_type[1] >= '0' && address_type[1] <= '7' && address_type[2] == '\0') { /* for registers */
        return register_direct_addr_mode;
	}
	else if (address_type[0] == '#' && check_number(address_type + 1)) { /* for numbers */
        return immediate_addr_mode;
	}
	else if (address_type[0] && address_type[0] == '%' && check_jump(address_type + 1)) { /* for "jump" commands */
        return relative_addr_mode;
	}
	else if (sign_name_validation(address_type)) { /* for lables */
        return direct_addr_mode;
	}
	else return addr_mode_null;
}

/* Function to analyze operands according to given instructions */
bool instruction_analyzer(detect_line_errors line_to_check, int given_index, char **operands, int *counter, char *command) {
	int i;
	*counter = 0;
	operands[0] = operands[1] = NULL;
	CLEAR_WHITE_SPACES(line_to_check.line_input, given_index)
	if (line_to_check.line_input[given_index] == ',') { /* for errors */
		error_detection(line_to_check, "Error! invalid sign after command.");
		return FALSE;
	}
	for (*counter = 0; line_to_check.line_input[given_index] != EOF && line_to_check.line_input[given_index] != '\n' && line_to_check.line_input[given_index];) {
		if (*counter == 2) { /* for more then 2 operands errors */
			error_detection(line_to_check, "Error! too many operands.");
			free(operands[0]);
			free(operands[1]);
			return FALSE;
		}
		operands[*counter] = check_malloc(LINE_LENGTH);
		/* validates the current operand */
		for (i = 0; line_to_check.line_input[given_index] && line_to_check.line_input[given_index] != '\t' && line_to_check.line_input[given_index] != ' ' &&
            line_to_check.line_input[given_index] != '\n' && line_to_check.line_input[given_index] != EOF && line_to_check.line_input[given_index] != ','; given_index++, i++) {
			operands[*counter][i] = line_to_check.line_input[given_index];
		}
		operands[*counter][i] = '\0';
		(*counter)++;
		CLEAR_WHITE_SPACES(line_to_check.line_input, given_index)
        /* check if analyze reached end of file */
		if (line_to_check.line_input[given_index] == '\n' || line_to_check.line_input[given_index] == EOF || !line_to_check.line_input[given_index]) {
            break;
		}
		else if (line_to_check.line_input[given_index] != ',') { /* for ',' errors */
			error_detection(line_to_check, "Error! missing ',' between operands.");
			free(operands[0]);
			if (*counter > 1) {
				free(operands[1]);
			}
			return FALSE;
		}
		given_index++;
		CLEAR_WHITE_SPACES(line_to_check.line_input, given_index)
		/* for white spaces errors in code lines */
		if (line_to_check.line_input[given_index] == '\n' || line_to_check.line_input[given_index] == EOF || !line_to_check.line_input[given_index]) {
			error_detection(line_to_check, "Error! missing operands, to much unwanted white spaces.");
		}
		else if (line_to_check.line_input[given_index] == ',') {
            error_detection(line_to_check, "Error! too many operands, add between them white spaces.");
		}
		else {
            continue;
		}
		{ /* final errors check */
			free(operands[0]);
			if (*counter > 1) {
				free(operands[1]);
			}
			return FALSE;
		}
	}
	return TRUE;
}

/* Auxiliary function to help associate the wanted opcode of funct to it's legal addressing method using va_list*/
bool opcode_or_funct_auxiliary_check(detect_line_errors line_to_check, addressing_methods source_address, addressing_methods destination_address, int source_address_count, int destination_address_count, ...) {
	int i;
	bool flag;
	va_list addresses_list;
	addressing_methods source_address_array[4], destination_address_array[4]; /* got 4 addresses types */
	/* using memset to fill up the array memory */
	memset(source_address_array, addr_mode_null, sizeof(source_address_array));
	memset(destination_address_array, addr_mode_null, sizeof(destination_address_array));
	va_start(addresses_list, destination_address_count);
    /* store wanted elements inside va_list */
	for (i = 0; i < source_address_count && i <= 3 ;i++) {
		source_address_array[i] = va_arg(addresses_list, int);
	}
	for (; source_address_count > 5; va_arg(addresses_list, int), source_address_count--);
	for (i = 0; i < destination_address_count && i <= 3 ;i++) {
		destination_address_array[i] = va_arg(addresses_list, int);
	}
	va_end(addresses_list);  /* end of va_list */
	/* validates operands addressing methods */
	flag = source_address_count == 0 && source_address == addr_mode_null;
	for (i = 0; i < source_address_count && !flag; i++) {
		if (source_address_array[i] == source_address) {
			flag = TRUE;
		}
	}
	if (!flag) { /* for errors */
		error_detection(line_to_check, "Error! invalid addressing method for the first operand.");
		return FALSE;
	}
	flag = destination_address_count == 0 && destination_address == addr_mode_null;
	for (i = 0; i < destination_address_count && !flag; i++) {
		if (destination_address_array[i] == destination_address) {
			flag = TRUE;
		}
	}
	if (!flag) { /* for errors */
		error_detection(line_to_check, "Error! invalid addressing method for the second operand.");
		return FALSE;
	}
	return TRUE;
}

/* Function to associate the wanted opcode or funct to it's legal addressing method according to the given instructions table */
bool addressing_type_opcode_or_funct_validation(detect_line_errors line_to_check, addressing_methods source_operand, addressing_methods destination_operand, opcode opcode_to_check, int counter) {
	/* also source and destination operands are required  */
	if (opcode_to_check >= mov_opcode && opcode_to_check <= lea_opcode) {
		if (counter != 2) { /* for errors */
			error_detection(line_to_check, "Error! missing source or destination operands.");
			return FALSE;
		}
		/* distinguish between the commands according to instructions table */
		if (opcode_to_check == cmp_opcode) { /* only for "cmp" command */
			return opcode_or_funct_auxiliary_check(line_to_check, source_operand, destination_operand, 3, 3,
                                                    immediate_addr_mode, direct_addr_mode, register_direct_addr_mode, /* for source operands */
                                                    immediate_addr_mode, direct_addr_mode, register_direct_addr_mode); /* for destination operands */
		}
		else if (opcode_to_check == add_opcode || opcode_to_check == mov_opcode) { /* for "mov", "add" and "sub" commands  */
			return opcode_or_funct_auxiliary_check(line_to_check, source_operand, destination_operand, 3, 2,
                                                    immediate_addr_mode, direct_addr_mode, register_direct_addr_mode, /* for source operands */
                                                    direct_addr_mode, register_direct_addr_mode); /* for destination operands */
		}
		else if (opcode_to_check == lea_opcode) { /* only for "lea" command */
			return opcode_or_funct_auxiliary_check(line_to_check, source_operand, destination_operand, 1, 2,
                                                    direct_addr_mode, /* for source operand */
                                                    direct_addr_mode, register_direct_addr_mode); /* for destination operands */
		}
	} /* end of 2 operands requirements */
	/* only destination operands is required */
	else if (opcode_to_check >= clr_opcode && opcode_to_check <= prn_opcode) {
		if (counter != 1) { /* for errors */
			if (counter < 1) error_detection(line_to_check, "Error! only destination operand is required.");
			return FALSE;
		}
		/* again distinguish between the commands according to instructions table */
		if (opcode_to_check == red_opcode || opcode_to_check == clr_opcode) { /* for "clr", "not", "inc", "dec" and "red" commands */
			return opcode_or_funct_auxiliary_check(line_to_check, source_operand, addr_mode_null, 2, 0, direct_addr_mode, register_direct_addr_mode);
		}
		else if (opcode_to_check == jmp_opcode) { /* for "jmp", "bne" and "jsr" commands */
			return opcode_or_funct_auxiliary_check(line_to_check, source_operand, addr_mode_null, 2, 0, direct_addr_mode, relative_addr_mode);
		}
		else { /* only for "prn" command */
			return opcode_or_funct_auxiliary_check(line_to_check, source_operand, addr_mode_null, 3, 0, immediate_addr_mode, direct_addr_mode, register_direct_addr_mode);
		}
	} /* end of only destination operands requirements */
	/* no operands is required */
	else if (opcode_to_check <= stop_opcode && opcode_to_check >= rts_opcode) { /* for "rts" and "stop" commands */
		if (counter > 0) { /* for errors */
			error_detection(line_to_check, "Error! no operands are required.");
			return FALSE;
		}
	}
	return TRUE;
}

/* Function to encode code lines according to opcode or funct values */
first_machine_word_bytes *encode_line(detect_line_errors line_to_check, opcode opcode_to_check, funct funct_to_check, int counter, char *operands[2]) {
	first_machine_word_bytes *output_word;
	/* addressing methods, opcode or funct validation */
	addressing_methods first_addressing = counter >= 1 ? get_addressing_type(operands[0]) : addr_mode_null;
	addressing_methods second_addressing = counter == 2 ? get_addressing_type(operands[1]) : addr_mode_null;
	if (!addressing_type_opcode_or_funct_validation(line_to_check, first_addressing, second_addressing, opcode_to_check, counter)) {
		return NULL;
	}
	/* encoding code line */
	output_word = (first_machine_word_bytes *) check_malloc(sizeof(first_machine_word_bytes));
	output_word->opcode = opcode_to_check; /* encode opcode value */
	output_word->funct = funct_to_check; /* encode funct value */
	output_word->destination_addr_method = output_word->desination_register = output_word->source_addr_method = output_word->source_register = 0; /* encode address method value */
	/* for first commands group */
	if (opcode_to_check >= mov_opcode && opcode_to_check <= lea_opcode) {
		output_word->source_addr_method = first_addressing;
		output_word->destination_addr_method = second_addressing;
		/* for the wanted registers */
		if (first_addressing == register_direct_addr_mode) {
			output_word->source_register = check_register(operands[0]);
		}
		if (second_addressing == register_direct_addr_mode) {
			output_word->desination_register = check_register(operands[1]);
		}
	}
	/* for second commands group */
	else if (opcode_to_check >= clr_opcode && opcode_to_check <= prn_opcode) {
		output_word->destination_addr_method = first_addressing;
		/* for the wanted registers */
		if (first_addressing == register_direct_addr_mode) {
			output_word->desination_register = check_register(operands[0]);
		}
	}
	return output_word;
}

/* Function to encode are code words value */
encode_word *encode_are_value(sentance_types type, bool external_sign) {
	encode_word *encoded_word = check_malloc(sizeof(encode_word));
	if (type == date_type || type == string_type) {
		encoded_word->ARE[0] = 'A';

	}
	else if (type == entry_type) {
        encoded_word->ARE[1] = 'R';
	}
	else {
        encoded_word->ARE[2] = 'E';
	}
	/* for ARE line */
	return encoded_word;
}

/* Function to get the sentence type according to a given index, mainly to check sentence types errors */
sentance_types get_sentence_type(detect_line_errors line_to_check, int *index) {
    int i;
	char temp_sentence_type_arr[LINE_LENGTH];
	sentance_types output_type;
	CLEAR_WHITE_SPACES(line_to_check.line_input, *index)
	/* error check */
	if (line_to_check.line_input[*index] != '.') {
        return sentance_type_null;
	}

	for (i = 0; line_to_check.line_input[*index] && line_to_check.line_input[*index] != '\t' && line_to_check.line_input[*index] != ' '; (*index)++, i++) {
		temp_sentence_type_arr[i] = line_to_check.line_input[*index];
	}
	temp_sentence_type_arr[i] = '\0'; /* End of string */
	/* again error check */
	if ((output_type = get_sentence_type_auxiliary(temp_sentence_type_arr+1)) != sentance_type_null) {
        return output_type;
	}
	error_detection(line_to_check, "Error! invalid sentence type.");
	return sentance_type_error;
}

/* Function to analyze the ".data" instruction for the assembler code scans, mainly for errors */
bool analyze_data_to_code_line(detect_line_errors line_to_check, int index, long *data_pointer, long *DC_COUNTER) {
    int i;
	long result;
    char temp_data_arr[80], *ptr;
	CLEAR_WHITE_SPACES(line_to_check.line_input, index)
	if (line_to_check.line_input[index] == ',') { /* for char errors */
		error_detection(line_to_check, "Error! invalid character after .data instruction.");
	}
	/* start to analyze and look for errors */
	do {
		for (i = 0; line_to_check.line_input[index] && line_to_check.line_input[index] != EOF && line_to_check.line_input[index] != '\t' &&
		     line_to_check.line_input[index] != ' ' && line_to_check.line_input[index] != ',' && line_to_check.line_input[index] != '\n'; index++, i++) {
                temp_data_arr[i] = line_to_check.line_input[index];
		}
		temp_data_arr[i] = '\0';
		if (!check_number(temp_data_arr)) { /* for number errors */
			error_detection(line_to_check, "Error! missing number in .data instruction.");
			return FALSE;
		}
		/* initialize DC and increase counter */
		result = strtol(temp_data_arr, &ptr, 10);
		data_pointer[*DC_COUNTER] = result;
		(*DC_COUNTER)++;
		/* ',' is valid after command */
		CLEAR_WHITE_SPACES(line_to_check.line_input, index)
		if (line_to_check.line_input[index] == ',') {
            index++;
		}
		/* looking for '.' at the end of word until end of file */
		else if (!line_to_check.line_input[index] || line_to_check.line_input[index] == '\n' || line_to_check.line_input[index] == EOF) {
			break;
		}
		CLEAR_WHITE_SPACES(line_to_check.line_input, index)
		if (line_to_check.line_input[index] == ',') { /* for commas errors */
			error_detection(line_to_check, "Error! to much commas.");
			return FALSE;
		}
		else if (line_to_check.line_input[index] == EOF || line_to_check.line_input[index] == '\n' || !line_to_check.line_input[index]) { /* for end of line error */
			error_detection(line_to_check, "Error! missing data, not reached to end of line.");
			return FALSE;
		}
	}
	while (line_to_check.line_input[index] != '\n' && line_to_check.line_input[index] != EOF); /* everything is OK, no errors was found */
	return TRUE;
}

/* Function to analyze the ".string" instruction for the assembler code scans, mainly for errors */
bool analyze_string_to_code_line(detect_line_errors line_to_check, int index, long *data_pointer, long *DC_COUNTER) {
    char *flag = strrchr(line_to_check.line_input, '"'); /* flag to check '"' requirements in a string */
	char temp_string_arr[LINE_LENGTH];
	CLEAR_WHITE_SPACES(line_to_check.line_input, index)
	/* start to analyze and look for errors */
	if (line_to_check.line_input[index] != '"') { /* for first '"' errors */
		error_detection(line_to_check, "Error! string has no opening quote mark.");
		return FALSE;
	}
	else if (&line_to_check.line_input[index] == flag) { /* for last '"' errors */
		error_detection(line_to_check, "Missing closing quote of string");
		return FALSE;
	}
	else {
		int i;
		/* copy string to code line as is */
		for (i = 0;line_to_check.line_input[index] && line_to_check.line_input[index] != '\n' && line_to_check.line_input[index] != EOF; index++,i++) {
            temp_string_arr[i] = line_to_check.line_input[index];
		}
		temp_string_arr[flag - line_to_check.line_input] = '\0';
		/* sort and increase DC counter */
		for(i = 1;temp_string_arr[i] && temp_string_arr[i] != '"'; i++) {
			data_pointer[*DC_COUNTER] = temp_string_arr[i];
			(*DC_COUNTER)++;
		}
		data_pointer[*DC_COUNTER] = '\0'; /* reached end of string */
		(*DC_COUNTER)++;
	}
	return TRUE;
}
