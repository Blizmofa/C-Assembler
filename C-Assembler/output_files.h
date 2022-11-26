#ifndef _OUTPUT_FILES_H
#define _OUTPUT_FILES_H

#include "structs.h"
#include "sign_table.h"

/**
* Functions to create the wanted output assembler files.
* All functions have indicative names.
*
* Function "create_ob_output_file" creates the wanted ".ob" file.
* Function "copy_sign_to_output_file" copies signs from table to file.
* Function "create_output_files" creates the assembler files.
*/

bool create_ob_output_file(machine_binary_word **code_word_pointer, long *data_word_pointer, int FINAL_IC_COUNTER, int FINAL_DC_COUNTER, char *input_file);

bool copy_sign_to_output_file(table_element head, char *input_file, char *input_file_extension);

int create_output_files(machine_binary_word **code_word_pointer, long *data_word_pointer, int FINAL_IC_COUNTER, int FINAL_DC_COUNTER, char *input_file, table_element sign_table_pointer);

#endif
