#include "assembler.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


/*
 counting the number of lines that are created for the    *object file of the given file and in addition counting the data lines of the file. 
*/
void first_passage(char* file_name, int is_error)
{
	struct {/*struct for all the instruction as strings, opcode and decimal value*/
		char* name;
		int opcode;
		char* opcode_bin;
	} instruc[] = {
		{"mov", 0, "0000"},
		{"cmp", 1, "0001"},
		{"add", 2, "0010"},
		{"sub", 3, "0011"},
		{"not", 4, "0100"},
		{"clr", 5, "0101"},
		{"lea", 6, "0110"},
		{"inc", 7, "0111"},
		{"dec", 8, "1000"},
		{"jmp", 9, "1001"},
		{"bne", 10, "1010"},
		{"red", 11, "1011"},
		{"prn", 12, "1100"},
		{"jsr", 13, "1101"},
		{"rts", 14, "1110"},
		{"stop", 15, "1111"},
		{"null", -1, "0000"}
	};
	int i;
	int ic = FIRST_LINE;/*counter to the lines in the object file **not a counter of instructions line**  */
	int dc = 0;/*data counter*/
	int first_entry = 1;/*first defintion of .entry label*/
	int type = 0;/*three state boolean*/
	int is_instruct = 0;/*boolean*/
	int first_label = 1;/*boolean*/
	int label_scanned = 0;/*boolean*/
	char colon = ':';
	char word[MAX_LABEL];/*scans the word from the file*/
	char label[MAX_LABEL];/*scans labels*/
	char line[MAX_LINE];
	char* str;
	char* comma = ",";/*for strtok*/
	char* bracket1 = "(";/*for strtok*/
	char* bracket2 = ")";/*for strtok*/
	char* operands[MAX_OPER] = {0};
	char* params[MAX_PARAM] = {0};
	char help_var[MAX_OPER][MAX_LABEL] = {0};/*auxiliary variables for solving bugs*/
	node* n1, *copy;
	entry* e1;
	FILE* file;
	enum{zero, external, inner};
	e1 = NULL;
	n1 = NULL;
	file = fopen(file_name, "r");
	while(fscanf(file, "%s", word) != EOF)
	{
		for(i = 0; instruc[i].opcode != -1; i++)
		{
			if(strcmp(word, instruc[i].name) == 0)/*if instruction has scanned*/
			{
				is_instruct = 1;
			}
		}
		if(is_instruct)/*if instruction has scanned*/
		{
			if(label_scanned)/*if label has scanned*/
			{
				if(check_entry(label, n1))/*if the label is .entry*/
				{
					change_address(ic, label, n1);/*change the address of the label because it was defined as 0*/
					/*n1 = add_label(label, ic, inner, is_instruct, line, n1);*/
				}
				else
				{
					if(first_label)/*if the first label that has scanned*/
					{
						n1 = add_label(label, ic, inner, is_instruct, "", NULL);/*add the label to the linked list*/
						first_label = 0;
					}
					else
					{
						n1 = add_label(label, ic, inner, is_instruct, "", n1);/*add the label to the linked list*/
					}
					label_scanned = 0;
				}
			}
			fgets(line, MAX_LINE, file);/*gets the rest of the line*/
			ic++;/*add a word for the instruction line*/
			operands[0] = strtok(line, comma);
			if(operands[0][strlen(operands[0])-1] == '\n')/*if there is \n in the end, it means that there is one operand or no operands*/
			{
				remove_blanks(operands[0]);
				if(operands[0][0] != 0)/*if there is one operand*/
				{
					ic++;/*add word for the operand*/
				}
			}
			else/*if there is a second operand or brackets*/
			{
				if(strchr(operands[0], *bracket1))/*if there are brackets*/
				{
					str = (char*)calloc(strlen(operands[0]), sizeof(char));
					strcpy(str, operands[0]);/*saves the first part of the sentence*/
					operands[0] = strtok(NULL, comma);/*getting the second part of the sentence*/
					params[0] = strtok(str, bracket1);
					params[0] = strtok(NULL, bracket1);/*getting the first param*/
					params[1] = strtok(operands[0], bracket2);/*getting the second param*/
					strcpy(help_var[0], params[0]);/*saving in the auxilary variables to prevent bugs*/
					strcpy(help_var[1], params[1]);/*saving in the auxilary variables to prevent bugs*/
					ic++;/*add word for the label*/
					free(str);
				}
				else/*if there is a second operand*/
				{
					operands[1] = strtok(NULL, comma);/*getting the second parameter*/
					remove_blanks(operands[0]);
					remove_blanks(operands[1]);
					strcpy(help_var[0], operands[0]);/*saving in the auxilary variables to prevent bugs*/
					strcpy(help_var[1], operands[1]);/*saving in the auxilary variables to prevent bugs*/
				}
				if(type_operand(help_var[0]) == 2 && type_operand(help_var[1]) == 2)/*if both params or both operands are registers*/
				{
					ic ++;/*add word word for the registers*/
				}
				else
				{
					ic += 2;/*add 2 words for the operands or parameters*/
				}
			}
			is_instruct = 0;
		}
		else if(strcmp(word, ".extern") == 0 || strcmp(word, ".entry") == 0)
		{/*if .extern or .entry has scanned*/
			fscanf(file, "%s", label);/*scan the label*/
			if(strcmp(word, ".entry") == 0)/*if its .entry*/
			{
				type = zero;/*make the type of the label entry*/
				if(is_exist(label, n1))/*if the label exist already*/
				{
					copy = n1;
					while(copy != NULL)/*go through all the scanned labels*/
					{
						if(strcmp(copy->name, label) == 0)
						{
							copy->type = 2;/*change the type of the label to 2 for printing the ARE as 10 (2 in binary)*/
						}
						copy = copy->next;
					}
				}
				if(first_entry)/*if its the first .entry label*/
				{
					e1 = add_entry(label, NULL);/*add an entry label to the linked list*/
					first_entry = 0;
				}
				else
				{
					e1 = add_entry(label, e1);/*add an entry label to the linked list*/
				}
			}
			else/*if its .extern*/
			{
				type = external;/*make the type of the label external*/
				if(first_label)/*if it sthe first label that has scanned*/
				{
					n1 = add_label(label, 0, type, is_instruct, "", NULL);/*add the external label with address of 0*/
					first_label = 0;
				}
				else
				{
					n1 = add_label(label, 0, type, is_instruct, "", n1);/*add the external label with address of 0*/
				}
			}
			
		}
		else if(strcmp(word, ".data") == 0)
		{/*if the scanned word is .data*/
			fgets(line, MAX_LINE, file);
			remove_blanks(line);
			if(check_entry(label, n1))/*check if the label is .entry*/
			{
				change_address(ic, label, n1);/*change the entry address*/
			}
			else
			{
				if(first_label)/*same as the previous adding of labels...*/
				{
					n1 = add_label(label, ic, inner, is_instruct, line, NULL);
					first_label = 0;
				}
				else
				{
					n1 = add_label(label, ic, inner, is_instruct, line, n1);
				}
			}
			str = strtok(line, comma);/*getting the first number*/
			while(str != NULL)/*scanning every number*/
			{
				str = strtok(NULL, comma);
				ic++;/*adds a word for the numbers, in my program the ic variable is the number of lines and not the instruct lines*/
				dc++;/*adding a word for the data counter*/
			}
			
		}
		else if(strcmp(word, ".string") == 0)
		{/*if the scanned word is .string*/
			fgets(line, MAX_LINE, file);
			remove_blanks(line);/*check if the label is .entry*/
			if(check_entry(label, n1))/*check if the label is .entry*/
			{
				change_address(ic, label, n1);/*change the entry address*/
			}
			else
			{
				if(first_label)/*same as the previous adding of labels...*/
				{
					n1 = add_label(label, ic, inner, is_instruct, line, NULL);
					first_label = 0;
				}
				else
				{
					n1 = add_label(label, ic, inner, is_instruct, line, n1);
				}
			}
			dc += (strlen(line)-1);/*adding to the data counter the length of the string without the quotation marks and plus a null(the length of the string - 1)*/
			ic += (strlen(line)-1);/*same to the lines counter*/
			
			
		}
		else if(word[strlen(word)-1] == colon)
		{/*if the word is a label - XXXXXXXX: */
			word[strlen(word)-1] = 0;
			if(check_name(word, n1) && !first_label)/*if label is defined twice*/
			{/*if the label already exist*/
				fprintf(stderr, "ERROR in line %d: label defined twice!\n", ic-FIRST_LINE);/*print an error*/
				is_error = 1;
			}
			else
			{
				label_scanned = 1;
				strcpy(label, word);
			}
		}  
	}
	copy = n1;
    fclose(file);
    if(!is_error)/*if there is no errors activate the second passage*/
    {
    	second_passage(file_name, n1, e1, ic , dc, is_error);
    }
}

