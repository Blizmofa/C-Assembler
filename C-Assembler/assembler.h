#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H

#include "utils.h"
#include "first_scan.h"
#include "second_scan.h"
#include "output_files.h"

/**
* Function to assemble a given file into assembly code.
* This function uses functions from the majority of project files.
*/

bool assemble(char *input_file);

#endif
