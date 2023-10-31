maman14: error.h assembler.h pre_ass.h assembler.c error.c first_pass.c functions.c pre_ass.c second_pass.c 
	gcc -g -ansi -pedantic -Wall assembler.c error.c first_pass.c functions.c pre_ass.c second_pass.c  -o assembler

