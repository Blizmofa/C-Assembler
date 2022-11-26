#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "structs.h"
#include "sign_table.h"

#define START_FROM 100

/* Auxiliary function to create ".ob" output file */
bool create_ob_output_file(machine_binary_word **code_word_pointer, long *data_word_pointer, int FINAL_IC_COUNTER, int FINAL_DC_COUNTER, char *input_file) {
	int i;
	unsigned int bytes_value;
	char *output_file_extension = strcat_memory_check(input_file, ".ob");
    FILE *new_output_file;
	new_output_file = fopen(output_file_extension, "w"); /* open file and start writing */
	free(output_file_extension);
	/* for errors */
	if (new_output_file == NULL) {
		printf("Error! file is not accessible.");
		return FALSE;
	}
	/* for IC and DC counters on top of ".ob" file */
	fprintf(new_output_file, "%d %d", FINAL_IC_COUNTER-START_FROM, FINAL_DC_COUNTER);
	/* encodes the wanted output according to instructions */
	for (i = 0; i < FINAL_IC_COUNTER-START_FROM; i++) {
		if (code_word_pointer[i]->binary_word_length > 0) {
            bytes_value = (code_word_pointer[i]->word_to_encode.code_word->opcode << 8) | (code_word_pointer[i]->word_to_encode.code_word->source_addr_method << 2) |
                          (code_word_pointer[i]->word_to_encode.code_word->destination_addr_method << 2) | (code_word_pointer[i]->word_to_encode.code_word->funct << 4);
            fprintf(new_output_file, "\n%.4d %.3X A", i+START_FROM, bytes_value);
        }
        else {
            bytes_value = (code_word_pointer[i]->word_to_encode.code_word->destination_addr_method<< 2) | (code_word_pointer[i]->word_to_encode.code_word->source_addr_method << 2) |
                          (code_word_pointer[i]->word_to_encode.code_word->funct << 4) | (code_word_pointer[i]->word_to_encode.code_word->opcode << 4);
            fprintf(new_output_file, "\n%.4d %.3X R", i+START_FROM, bytes_value);
		}
	}
	/* encode again */
	for (i = 0; i < FINAL_DC_COUNTER; i++) {
		bytes_value = (code_word_pointer[i]->word_to_encode.code_word->opcode << 4) | (code_word_pointer[i]->word_to_encode.code_word->source_addr_method << 2) |
                      (code_word_pointer[i]->word_to_encode.code_word->destination_addr_method << 2) | (code_word_pointer[i]->word_to_encode.code_word->funct << 4);
        /* prints the output */
		fprintf(new_output_file, "\n%.4d %.3X E", FINAL_IC_COUNTER+i, bytes_value);
	}
	/* close file if no errors was found */
	fclose(new_output_file);
	return TRUE;
}

/* Auxiliary function to copy wanted signs from sign table to the output file */
bool copy_sign_to_output_file(table_element head, char *input_file, char *input_file_extension) {
	char *file_title_with_extension = strcat_memory_check(input_file, input_file_extension);
    FILE *new_output_file;
	new_output_file = fopen(file_title_with_extension, "w");
	free(file_title_with_extension);
	/* for errors */
	if (new_output_file == NULL) {
		printf("Error! file is not accessible.");
		return FALSE;
	}
	/* check if the sign table is empty */
	if (head == NULL) {
        return TRUE;
	}
	/* encode output lines */
	fprintf(new_output_file, "%s %.4d", head->sign_flag, head->sign_position); /* for first line */
	while ((head = head->next_element) != NULL) { /* for the rest of the lines */
		fprintf(new_output_file, "\n%s %.4d", head->sign_flag, head->sign_position);
	}
	/* close file if no errors was found */
	fclose(new_output_file);
	return TRUE;
}

/* Function to create the wanted output assembler files */
int create_output_files(machine_binary_word **code_word_pointer, long *data_word_pointer, long FINAL_IC_COUNTER, long FINAL_DC_COUNTER, char *input_file, table_element sign_table_pointer) {
	bool output_file;
    table_element entry_signs = get_table_sign_types(sign_table_pointer, entry_sign);
	table_element external_signs = get_table_sign_types(sign_table_pointer, external_pointer);
    /* creates ".ob", ".ent" and ".ext" files and free sign table memory */
	output_file = create_ob_output_file(code_word_pointer, data_word_pointer, FINAL_IC_COUNTER, FINAL_DC_COUNTER, input_file) &&
                                        copy_sign_to_output_file(entry_signs, input_file, ".ent") && copy_sign_to_output_file(external_signs, input_file, ".ext");
	/* free tables memory */
	free_table_malloc(external_signs);
	free_table_malloc(entry_signs);
	return output_file;
}
