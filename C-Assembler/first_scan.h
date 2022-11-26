#ifndef _FIRST_SCAN_H
#define _FIRST_SCAN_H

#include "structs.h"

/**
* Functions for the first assembly code scan.
* All functions have indicative names.
*
* Function "encode_second_word_first_scan" encodes the second code word on the first assembler scan.
* Function "line_analyzer" analyzes given code lines in general.
* Function "first_scan_line_analyzer" analyzes given code lines on the first scan according to suggested algorithm in Course booklet
*/

void encode_second_word_first_scan(machine_binary_word **code_word_pointer, long *IC_COUNTER, char *sign) ;

bool line_analyzer(detect_line_errors line_to_check, int index, long *IC_COUNTER, machine_binary_word **code_word_pointer);

bool first_scan_line_analyzer(detect_line_errors line_to_check, long *IC_COUNTER, long *DC_COUNTER, machine_binary_word **code_word_pointer, long *data_word_pointer, table_element *sign_table_pointer);

#endif
