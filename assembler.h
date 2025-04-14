#ifndef ASSEMBLER_H
#define ASSEMBLER_H

/*
*     Author: Griffin Short
*	  Date: 04/2025
*/

#define ROM_SIZE 128

//Holds the names for the VHDL program's ROM fields.
typedef struct ROM {
	char name[32];
	char type[32];
	int size; //Bytes
} ROM;

//Holds a line's instruction and argument if necessary
typedef struct instruction {
	char insr[16];
	char arg[8];
} instruction;

extern const char* arg_insrs[];
extern const char* no_arg_insrs[];
extern const int arg_insrs_size;
extern const int no_arg_insrs_size;

void prompt_user(char in_file[128]);
void parse_name(ROM* program_ROM, char line[32]);
void parse_type(ROM* program_ROM, char line[32]);
void parse_line(ROM* program_ROM, char line[32], int line_num, instruction** insr_list);
void free_on_exit(instruction** insr_list);

#endif
