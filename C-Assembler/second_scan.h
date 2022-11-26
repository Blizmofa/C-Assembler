#ifndef _SECOND_SCAN_H
#define _SECOND_SCAN_H

#include "structs.h"
#include "sign_table.h"

/**
* Functions for the second assembly code scan.
* All functions have indicative names.
*
* Function "encode_second_word_second_scan" encodes the second code word on the second assembler scan.
* Function "second_scan_line_analyzer" analyzes given code lines on the second scan according to suggested algorithm in Course booklet.
* Function "encode_extra_data_word" encodes the extra operands data word according to suggested second pass algorithm section 6 in Course booklet.
*/

int encode_second_word_second_scan(detect_line_errors line_to_check, long *IC_POINTER, long *IC_COUNTER, char *given_operand, machine_binary_word **code_word_pointer, table_element *sign_table_pointer);

bool second_scan_line_analyzer(detect_line_errors line_to_check, long *IC_COUNTER, machine_binary_word **code_word_pointer, table_element *sign_table_pointer);

bool encode_extra_data_word(detect_line_errors line_to_check, long *IC_COUNTER, machine_binary_word **code_word_pointer, table_element *sign_table_pointer);

#endif
