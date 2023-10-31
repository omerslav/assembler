#include "assembler.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*
adding a label to the linked list of labels
returns the pointer to the last label node
*/
node* add_label(char name[], int address, int type, int is_instruct, char* value, node* head)
{
	/*copying each parameter into the node's members*/
	node* n1 = (node*)malloc(sizeof(node));
	n1->value = (char*)calloc(strlen(value), sizeof(char));
	strcpy(n1->name,name);
	n1->address = address;
	n1->type = type;
	n1->is_instruct = is_instruct;
	strcpy(n1->value,value);
	n1->next = head;
	return n1;
}

/*
adding an entry label into the entry labels linked list
returns the pointer to the last entry label node
*/
entry* add_entry(char name[], entry* head)
{
	/*copying each parameter into the node's members*/
	entry* n1 = (entry*)malloc(sizeof(entry));
	strcpy(n1->name,name);
	n1->next = head;
	return n1;
}

/*
check if a given name is a label in the labels linked list
if it does - returns 1, otherwise returns 0.
*/
int is_exist(char name[], node* n1)
{
	/*iterates through the linked list*/
	while(n1 != NULL)
	{
		if(strcmp(n1->name, name) == 0)/*if the names are the same return 1*/
			return 1;
		n1 = n1->next;
	}
	return 0;
}

/*
change the address of a label in the labels linked list
*/
void change_address(int address, char* name, node* n1)
{
	while(n1 != NULL)/*iterates through the linked list*/
	{
		if(strcmp(n1->name, name) == 0)/*if the names are the same change the address*/
		{
			n1->address = address;
		}
		n1 = n1->next;
	}
}


/*
checking if the given name is an extern label in the labels linked list.
if it does - returns 1, otherwise returns 0.
*/
int check_extern(char name[], node* head)
{
	node* n1 = head;
	while (n1 != NULL)/*iterates through the linked list*/
	{
		if (strcmp(n1->name,name) == 0)/*if the names are the same*/
		{
			return n1->type == 1;/*return if its external label*/
		}
		n1 = n1->next;
	}
	return 0;
}

/*
checking if he given name is an entry label in the labels linked list
if its entry returns 1, otherwise returns 0.
*/
int check_entry(char name[], node* head)
{
	node* n1 = head;
	while (n1 != NULL)/*iterates through the linked list*/
	{
		if (strcmp(n1->name,name) == 0)/*if the names are the same*/
		{
			return n1->type == 0;/*return if its external label*/
		}
		n1 = n1->next;
	}
	return 0;
}


/*
checking if the label exist in the labels linked list.
if yes it returns the address of him, otherwise returns 0.
*/
int check_name(char name[], node* head)
{
	node* n1 = head;
	while (n1 != NULL)
	{
		if (strcmp(n1->name,name) == 0)
		{
			return n1->address;
		}
		n1 = n1->next;
	}
	return 0;
}

/*
coding parts of the binary lines in object file.
for the given part of sentence it will edit the proper indexes as the given bits
*/
void kidud_instruc(char line[], char* bits, char* part)
{
	if(strcmp(part, "ARE") == 0)
	{
		line[13] = bits[1];
		line[12] = bits[0];
	}
	else if(strcmp(part, "miun2") == 0)
	{
		line[11] = bits[1];
		line[10] = bits[0];
	}
	else if(strcmp(part, "miun1") == 0)
	{
		line[9] = bits[1];
		line[8] = bits[0];
	}
	else if(strcmp(part, "opcode") == 0)
	{
		line[7] = bits[3];
		line[6] = bits[2];
		line[5] = bits[1];
		line[4] = bits[0];
	}
	else if(strcmp(part, "param2") == 0)
	{
		line[3] = bits[1];
		line[2] = bits[0];
	}
	else if(strcmp(part, "param1") == 0)
	{
		line[1] = bits[1];
		line[0] = bits[0];
	}
}


/*
Coding the addressing methods of parameters. 
*/
void kidud_param_oper(char bits_line[], char* str, char* part)
{
	if(type_operand(str) == 0)/*if the operand is a number*/
	{
		kidud_instruc(bits_line, "00", part);
	}
	else if(type_operand(str) == 1)/*if the operand is a label*/
	{
		kidud_instruc(bits_line, "01", part);
	}
	else if(type_operand(str) == 2)/*if the operand is register*/
	{
		kidud_instruc(bits_line, "11", part);
	}
}

/*
checks the given operands type.
if number returns 0
if label returns 1
if register returns 2
*/
int type_operand(char* operand)  
{
	int i,j;/**/
	char* regs[NUM_REGS] = {"r0","r1","r2","r3","r4","r5","r6","r7"};
	if(operand == NULL)
	{
		return -1;
	}
	for(i = 0; i < MAX_OPER; i++)/*iterate through every possibler register and compare it with the operand*/
	{
		for(j = 0; j < NUM_REGS; j++)
		{
			if(strcmp(operand, regs[j]) == 0)/*if the operand is a register return 2*/
			{
					return 2;		
			}
		}
	}
	if(operand[0] == '#')
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void remove_blanks(char line[])
{/*a function for removing legal white spaces from a string for read_set*/
	int i;
	int j = 0;
	char *copy;
	copy = (char*)calloc(strlen(line), sizeof(char));/*allocate memory for copy variable*/
	for(i = 0; i < strlen(line); i++){/*for every char in the given string*/
		if (isspace(line[i]))/*if the char is white space*/
			continue;
		else/*if the char isnt a white space*/
			copy[j] = line[i];/*copy the char*/
			j++;
	}
	strcpy(line,copy);
	free(copy);
}


