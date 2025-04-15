/*
*
*  EELE 367 8 Bit CPU Assembler
* 
* Converts "assembly" style text file into machine code
* ROM entries for the EELE 367 CPU. This makes writing
* and testing programs much easier than manually inputting
* instructions into the array in VHDL.
* 
*     Author: Griffin Short
*	  Date: 04/2025
* 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"

int main() {

	//--------------INIT-----------------------

	ROM program_ROM;
	program_ROM.size = ROM_SIZE;
	char in_file[128];

	//Prompt user to enter ASM file name
	prompt_user(in_file);

	//Open input asm file
	FILE* asm_file = fopen(in_file, "r");
	if (asm_file == NULL) {
		printf("Error opening ASM file\n");
		exit(EXIT_FAILURE);
	}

	//-----------END INIT-----------------------



	//--------------Parse File------------------

	//Retrieve name following the expected "name="
	char line[LINE_LEN]; //Buffer to hold line of max length 32
	if (fgets(line, LINE_LEN, asm_file) == NULL) {
		printf("Expected \"name=\" at start of ASM file.\n");
		exit(EXIT_FAILURE);
	}
	line[strcspn(line, "\n")] = '\0'; //Overwrite '\n' from fgets

	parse_name(&program_ROM, line);


	//Retrieve type following the expected "type="
	if (fgets(line, LINE_LEN, asm_file) == NULL) {
		printf("Expected \"type=\" at second line of ASM file.\n");
		exit(EXIT_FAILURE);
	}
	line[strcspn(line, "\n")] = '\0'; //Overwrite '\n' from fgets

	parse_type(&program_ROM, line);


	// Parse the lines of assembly code to prepare for output
	instruction* insr_list[ROM_SIZE];
	memset(insr_list, NULL, ROM_SIZE);
	char* token;
	int line_num = 3; //Current line of ASM file being parsed

	while (fgets(line, LINE_LEN, asm_file) != EOF) {
		if (line == NULL) {
			printf("Error: Could not read line %d in ASM file\n", line_num);
			free_on_exit(insr_list);
			exit(EXIT_FAILURE);
		}
		line[strcspn(line, "\n")] = '\0'; //Overwrite '\n' from fgets

		//Parse line and put into instruction array
		parse_line(&program_ROM, line, line_num, insr_list);

		//if instruction is halt, stop translating
		//(the ptr to the halt instruction will be out of bounds
		//of the print loop so that it won't be included in the output file
		if (strcmp(insr_list[line_num - 3], "HALT") == 0) {
			break;
		}

		memset(line, '\0', LINE_LEN);
		line_num++;
		if (line_num > 130) {
			printf("Maximum ROM size exceeded.\n");
			break;
		}
	}

	//--------END FILE PARSING--------------------



	//---------WRITE INSRS TO FILE-----------------
	
	write_to_file(&program_ROM, line_num, insr_list);


	//--------Program Cleanup and Exit-------------
	
	free_on_exit(insr_list);
	fclose(asm_file);

	printf("%s successfully assembled into ROM_load.txt.\n\n\n\n", in_file);

	return 0;
}