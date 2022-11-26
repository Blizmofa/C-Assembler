#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "first_scan.h"
#include "second_scan.h"
#include "output_files.h"

#define START_FROM 100
#define LINE_LENGTH 80
#define ARR_LENGTH 1200

/* Function to assemble the given ".as" file into assembly code */
bool assemble(char *input_file) {
	/* assembler variables */
	int flag;
	long IC_COUNTER = START_FROM;
	long DC_COUNTER = 0;
	long FINAL_IC_COUNTER, FINAL_DC_COUNTER;
    long data_word_arr[ARR_LENGTH]; /* array for data words */
	char *check_file_name;
	char string_line_arr[LINE_LENGTH + 2];
    bool assemble_process = TRUE; /* assemble process errors flag */
	machine_binary_word *code_word_arr[ARR_LENGTH];
	table_element sign_table_pointer = NULL; /* start with empty table */
	detect_line_errors line_to_check;
    FILE *output_file;
	/* for ".as" file suffix */
	check_file_name = strcat_memory_check(input_file, ".as");
	output_file = fopen(check_file_name, "r"); /* open the file and start writing to it */
	/* for empty file errors */
	if (output_file == NULL) {
		printf("Error! the given .as file is empty.\n");
		free(check_file_name);
		return FALSE;
	}
	/* for first assembly scan */
	line_to_check.name_of_file = check_file_name;
	line_to_check.line_input = string_line_arr;
	/* start to assemble */
	for (line_to_check.num_of_line = 1;
	     fgets(string_line_arr, LINE_LENGTH + 2, output_file) != NULL; line_to_check.num_of_line++) {
		 /* for line length errors */
		if (strchr(string_line_arr, '\n') == NULL && !feof(output_file)) {
			error_detection(line_to_check, "Error! reached above allowed line length.");
			assemble_process = FALSE;
			do {
				flag = fgetc(output_file);
			} while (flag != '\n' && flag != EOF);
		}
		else {
			if (!first_scan_line_analyzer(line_to_check, &IC_COUNTER, &DC_COUNTER, code_word_arr, data_word_arr, &sign_table_pointer)) {
				if (assemble_process) {
					FINAL_IC_COUNTER = -1;
					assemble_process = FALSE; /* assemble process failed */
				}
			}
		}
	}
    /* initialize IC and DC values */
	FINAL_IC_COUNTER = IC_COUNTER;
	FINAL_DC_COUNTER = DC_COUNTER;
	/* for second assembly scan */
	if (assemble_process) {
        IC_COUNTER = START_FROM;
        set_table_sign_position(sign_table_pointer, FINAL_IC_COUNTER, data_sign); /* sets the signs positions */
        /* start assemble second scan from file beginning  */
        rewind(output_file);
        for (line_to_check.num_of_line = 1; !feof(output_file); line_to_check.num_of_line++) {
            int i = 0;
            fgets(string_line_arr, LINE_LENGTH, output_file);
            CLEAR_WHITE_SPACES(string_line_arr, i)
            if (code_word_arr[IC_COUNTER - START_FROM] != NULL || string_line_arr[i] == '.')
                assemble_process &= second_scan_line_analyzer(line_to_check, &IC_COUNTER, code_word_arr, &sign_table_pointer);
        }
        /* for ".ob", ".ent" and ".ext" file suffix's */
        if (assemble_process) {
            assemble_process = create_output_files(code_word_arr, data_word_arr, FINAL_IC_COUNTER, FINAL_DC_COUNTER, input_file, sign_table_pointer);
            }
	}
	/* free allocated memory for assemble process */
	free(check_file_name);
	free_table_malloc(sign_table_pointer);
	free_malloc(code_word_arr, FINAL_IC_COUNTER);
	/* assemble process succeeded and no errors was found */
	return assemble_process;
}
