#ifndef _STRUCTS_H
#define _STRUCTS_H

/**
* Project Instructions from the course booklet.
*
* ENUM for CPU Registers.
* Struct for Word structures.
* ENUM for opcodes representation.
* ENUM for funct representation.
* Struct for opcode and funct pointers.
* Struct for addressing types representation.
* Struct for sentence types representation.
* Struct for sentence types pointers.
*/

typedef enum cpu_registers {
	r0 = 0,
	r1,
	r2,
	r3,
	r4,
	r5,
	r6,
	r7,
	reg_null = -1 /* for errors */
} cpu_registers;

typedef struct first_machine_word_bytes {
    unsigned int destination_addr_method: 2;
    unsigned int source_addr_method: 2;
    unsigned int funct: 4;
    unsigned int opcode: 4;
	unsigned int desination_register: 3;
	unsigned int source_register: 3;
} first_machine_word_bytes;

typedef enum opcodes {
	mov_opcode = 0,
	cmp_opcode = 1,
	add_opcode = 2,
	sub_opcode = 2,
	lea_opcode = 4,
	clr_opcode = 5,
	not_opcode = 5,
	inc_opcode = 5,
	dec_opcode = 5,
	jmp_opcode = 9,
	bne_opcode = 9,
	jsr_opcode = 9,
	red_opcode = 12,
	prn_opcode = 13,
	rts_opcode = 14,
	stop_opcode = 15,
	opcode_null = -1 /* for errors */
} opcode;

typedef enum funct {
	add_funct = 10,
	sub_funct = 11,
	clr_funct = 10,
	not_funct = 11,
	inc_funct = 12,
	dec_funct = 13,
	jmp_funct = 10,
	bne_funct = 11,
	jsr_funct = 12,
	funct_null = 0 /* for errors */
} funct;

struct opcode_or_funct_element {
	char *element;
	opcode wanted_opcode_value;
	funct wanted_funct_value;
};

typedef enum addressing_methods {
	immediate_addr_mode = 0,
	direct_addr_mode = 1,
	relative_addr_mode = 2,
	register_direct_addr_mode = 3,
	addr_mode_null = -1
} addressing_methods;

typedef enum sentance_types {
	date_type,
	string_type,
	entry_type,
    extern_type,
	sentance_type_null,
	sentance_type_error /* for errors that starts with '.' but ins't a valid sentence type */
} sentance_types;

struct sentance_types_element {
	char *type;
	sentance_types type_value;
};

/**
* Data structures for the sign table and for tables all around the project in general.
*
* Struct for table element pointer.
* ENUM for sign table sign types representation.
* Struct for generic table variables for global use throughout the entire project where needed.
*/

typedef struct sign_table* table_element;

typedef enum sign_types {
    data_sign,
    string_sign,
    entry_sign,
    external_sign,
	other_code_sign,
	external_pointer
} sign_types;

typedef struct sign_table {
	table_element next_element;
	int sign_position;
	char *sign_flag;
	sign_types type;
} sign_table;

/**
* Data structures for encoding machine code and error detection.
*/

typedef struct encode_word {
	char ARE[2];
	unsigned long binary_code;
} encode_word;

typedef struct machine_binary_word {
	short binary_word_length;
	union word_to_encode {
		encode_word *data_word;
		first_machine_word_bytes *code_word;
	} word_to_encode;
} machine_binary_word;

typedef struct detect_line_errors {
	int num_of_line;
	char *name_of_file;
	char *line_input;
} detect_line_errors;

/* easier with boolean functions */
typedef enum booleans {
	FALSE = 0, TRUE = 1
} bool;

#endif
