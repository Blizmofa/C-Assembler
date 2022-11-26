#include <stdio.h>
#include "assembler.h"

/**
 * Project main function to run the assembler.
 */

int main(int argc, char *argv[]) {
	int i;
	bool assemble_process = TRUE;
	/* assemble given files */
	for (i = 1; i < argc; ++i) {
		if (!assemble_process) puts("");
		assemble_process = assemble(argv[i]);
	}
	return 0;
}


