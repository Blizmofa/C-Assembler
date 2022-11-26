#ifndef _INSTRUCTION_ANALYZER_H
#define _INSTRUCTION_ANALYZER_H

#include "sign_table.h"
#include "structs.h"

/**
 * This file contain the necessary function to analyze and encode instructions into code words according to Course booklet.
 * All functions have indicative names.
 *
 * Function "opcode_or_funct_value" returns the wanted opcode or funct value.
 * Function "get_addressing_type" returns the wanted addressing method.
 * Function "instruction_analyzer" analyze operands instructions and checks for errors.
 * Function "opcode_or_funct_auxiliary_check" helps validates opcode of funct values.
 * Function "addressing_type_opcode_or_funct_validation" validates opcode or funct values according to their addressing methods.
 * Function "encode_line" encodes opcode and funct values into code lines.
 * Function "encode_are_value" encodes wanted ARE values.
 * Function "get_sentence_type" associates sentence type to a given index.
 * Function "analyze_data_to_code_line" analyzes the ".data" instruction.
 * Function "analyze_string_to_code_line" analyzes the ".string" instruction.
 */

void opcode_or_funct_value(char *element, opcode *opc, funct *fun);

addressing_methods get_addressing_type(char* address_type);

bool instruction_analyzer(detect_line_errors line_to_check, int given_index, char** operands, int* counter, char* command);

bool opcode_or_funct_auxiliary_check(detect_line_errors line_to_check, addressing_methods source_address, addressing_methods destination_address, int source_address_count, int destination_address_count, ...);

bool addressing_type_opcode_or_funct_validation(detect_line_errors line_to_check, addressing_methods source_operand, addressing_methods destination_operand, opcode opcode_to_check, int counter);

first_machine_word_bytes *encode_line(detect_line_errors line_to_check, opcode opcode_to_check, funct funct_to_check, int counter, char *operands[2]);

encode_word *encode_are_value(sentance_types type, bool external_sign);

sentance_types get_sentence_type(detect_line_errors line_to_check, int *index);

bool analyze_data_to_code_line(detect_line_errors line_to_check, int index, long *data_pointer, long *DC_COUNTER);

bool analyze_string_to_code_line(detect_line_errors line_to_check, int index, long *data_pointer, long *DC_COUNTER);

#endif
