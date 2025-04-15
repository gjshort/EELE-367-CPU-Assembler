/*
*     Author: Griffin Short
*	  Date: 04/2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"

//Instruction types
const char* arg_insrs[] = {
	"LDA_IMM", "LDB_IMM", "LDA_DIR", "LDB_DIR", "STA_DIR", "STB_DIR",
	"BRA", "BEQ", "BCS", "BVS", "BMI"
};
const int arg_insrs_size = sizeof(arg_insrs) / sizeof(arg_insrs[0]);

const char* no_arg_insrs[] = {
	"ADD_AB", "SUB_AB", "AND_AB", "OR_AB", "INCA", "DECA", "INCB", "DECB", "HALT"
};
const int no_arg_insrs_size = sizeof(no_arg_insrs) / sizeof(no_arg_insrs[0]);

//Checks if the given token is an instruction
//that requires an argument
int is_arg_insr(char *token) {
	for (int i = 0; i < arg_insrs_size; i++) {
		if (strcmp(token, arg_insrs[i]) == 0) {
			return 1;
		}
	}
	return 0;
}

//Checks if the given token is an instruction
//that doesn't require an argument
int is_no_arg_insr(char* token) {
	for (int i = 0; i < no_arg_insrs_size; i++) {
		if (strcmp(token, no_arg_insrs[i]) == 0) {
			return 1;
		}
	}
	return 0;
}

//Initial UI
void prompt_user(char in_file[128]) {
	printf("EELE 367 8 BIT CPU ASSEMBLER\n");
	printf("----------------------------\n");
	printf("Enter the path and name (no quotes) of the text file to be assembled: ");
	if (fgets(in_file, 128, stdin) == NULL) {
		printf("Error reading ASM file name.\nMake sure:\n");
		printf("	-> There are no quotes wrapping the path.\n");
		printf("	-> The path name is less than 127 chars.\n");
		printf("	-> The file type is .txt\n");
		exit(EXIT_FAILURE);
	}
	in_file[strcspn(in_file, "\n")] = '\0'; //Overwrite '\n' from fgets
	if (strstr(in_file, ".txt") == NULL) {
		printf("Error: Invalid file type. Must be a .txt file.\n");
		exit(EXIT_FAILURE);
	}
}

//Extracts the name used for the VHDL array
void parse_name(ROM* program_ROM, char line[LINE_LEN]) {
	char* token = strtok(line, " ");
	if (token == NULL) {
		printf("Expected \"name=\" at start of ASM file.\n");
		exit(EXIT_FAILURE);
	}
	if (strcmp(token, "name=") == 0) {
		token = strtok(NULL, " ");
		if (token == NULL) {
			printf("Expected name of ROM for VHDL constant.\n");
			exit(EXIT_FAILURE);
		}
		strncpy(program_ROM->name, token, 32); //Store name in program_ROM
	}
	else {
		printf("Expected \"name=\" at start of ASM file.\n");
		exit(EXIT_FAILURE);
	}
}

//Extracts the type used for the VHDL array
void parse_type(ROM* program_ROM, char line[LINE_LEN]) {
	char* token = strtok(line, " ");
	if (token == NULL) {
		printf("Expected \"type=\" at start of ASM file.\n");
		exit(EXIT_FAILURE);
	}
	if (strcmp(token, "type=") == 0) {
		token = strtok(NULL, " ");
		if (token == NULL) {
			printf("Expected type of ROM for VHDL constant.\n");
			exit(EXIT_FAILURE);
		}
		strncpy(program_ROM->type, token, 32); //Store name in program_ROM
	}
	else {
		printf("Expected \"type=\" at start of ASM file.\n");
		exit(EXIT_FAILURE);
	}
}

//Parses a line of ASM and separates it into an
//instruction and an argument if necessary
void parse_line(ROM* program_ROM, char line[LINE_LEN], int line_num, instruction **insr_list) {
	
	
	//Allocate memory for new instruction and init fields to NULL
	instruction* insr = malloc(sizeof(instruction));
	if (insr == NULL) {
		printf("Error: could not allocate memory for instruction.\n");
		exit(EXIT_FAILURE);
	}
	memset(insr->insr, '\0', sizeof(insr->insr));		//Zero out and add to array of ptrs
	memset(insr->arg, '\0', sizeof(insr->arg));
	insr_list[line_num - 3] = insr;
	
	//Parse instruction token
	char* token = strtok(line, " ");
	if (token == NULL) {
		printf("Expected instruction on line %d\n", line_num);
		free_on_exit(insr_list);
		exit(EXIT_FAILURE);
	}

	//Find instruction and put in inctruction struct
	if (is_arg_insr(token)) { //Instruction requires an argument
		for (int i = 0; i < arg_insrs_size; i++) {
			if (strcmp(token, arg_insrs[i]) == 0) {
				strncpy(insr->insr, token, 16);
				break;
			}
		}
		//Extract argument for instruction
		token = strtok(NULL, " ");
		if (token == NULL) {
			printf("Expected argument for instruction on line %d\n", line_num);
			free_on_exit(insr_list);
			exit(EXIT_FAILURE);
		}

		//Check if argument is within bounds
		long int value;
		char* endptr;
		value = strtol(token, &endptr, 16);

		//Argument is not a value. Throw error
		if (endptr == token || *endptr != '\0') {
			printf("Invalid argument on line %d. Must be a hex value between 00 and FF\n", line_num);
			free_on_exit(insr_list);
			exit(EXIT_FAILURE);
		}
		//Argument is out of bounds. Throw error 
		if (value < 0 || value > 0xFF) {
			printf("Invalid argument on line %d. Must be a hex value between 00 and FF\n", line_num);
			free_on_exit(insr_list);
			exit(EXIT_FAILURE);
		}

		strncpy(insr->arg, token, 8);

	}

	else if (is_no_arg_insr(token)) {	//Instruction does NOT require an argument
		for (int i = 0; i < no_arg_insrs_size; i++) {
			if (strcmp(token, no_arg_insrs[i]) == 0) {
				strncpy(insr->insr, token, 16);
				break;
			}
		}
	}

	else { //Invalid instruction
		printf("Expected instruction on line %d\n", line_num);
		free_on_exit(insr_list);
		exit(EXIT_FAILURE);
	}
	
}

//Output machine code instructions formatted 
//for a VHDL ROM to a text file
void write_to_file(ROM* program_ROM, int line_num, instruction** insr_list) {
	printf("\nWriting instructions to ROM_load.txt...\n\n");

	//Output VHDL ROM entries to text file
	FILE* out_file = fopen("ROM_load.txt", "w");
	if (out_file == NULL) {
		printf("Error: could not open file for output.\n");
		free_on_exit(insr_list);
		exit(EXIT_FAILURE);
	}
	fprintf(out_file, "constant %s : %s := (\n\n", program_ROM->name, program_ROM->type);

	//Load Instructions (not including halt)
	int memory_slot = 0;
	for (int i = 0; i < (line_num - 3) && memory_slot < ROM_SIZE; i++) {
		fprintf(out_file, "%d => %s,\n", memory_slot, insr_list[i]->insr);
		memory_slot++;
		if (insr_list[i]->arg[0] != NULL) {
			fprintf(out_file, "%d => x\"%s\",\n", memory_slot, insr_list[i]->arg);
			memory_slot++;
		}
	}

	fprintf(out_file, "others => x\"00\");");
	fclose(out_file);
}

//Frees the array of pointers to instructions
void free_on_exit(instruction **insr_list) {
	for (int i = 0; insr_list[i] != NULL && i < ROM_SIZE; i++) {
		free(insr_list[i]);
	}
}