#include "assembler.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


void print_binary(char bits_line[], char* oper1, char* oper2, node* n1, FILE* file);
void binary(char bits_line[], int first, int last, int num);


/*
creates the object file, the externs file, and the entries file.
*/
void second_passage(char* file_macro_name, node* n1, entry* e1, int ic, int dc, int is_error)
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
	int miun2 = 0;/*boolean for miun2*/
	int empty;
	int empty_file = 1;/**/
	int is_instruct = 0;/*boolean*/
	int num;
	char c;
	char word[MAX_LABEL] = {0};
	char line[MAX_LINE];
	char bits_line[MAX_BITS+2] = "00000000000000\n";/*the length is plus two for new line and null*/
	char com[MAX_COM+1] = {0};/*the command in each line, adding one char for NULL*/
	char* str;
	char* file_name = NULL;
	char* new_file_name;
	char * file_ob_name;
	char* comma = ",";/*for strtok*/
	char* bracket1 = "(";/*for strtok*/
	char* bracket2 = ")";/*for strtok*/
	char* operands[MAX_OPER] = {0};
	char* params[MAX_PARAM] = {0};
	char help_var[MAX_OPER][MAX_LABEL] = {0};/*auxiliary variables for solving bugs*/
	FILE* file;/*the given file*/
	FILE* ob_file;/*the object file*/
	FILE* data_file;/*data lines file, at the end of the program the data lines will be copied to the end of object file*/
	enum{inner, external, entry};
	
	
	operands[0] = (char*)calloc(MAX_LABEL, sizeof(char));
	operands[1] = (char*)calloc(MAX_LABEL, sizeof(char));
	params[0] = (char*)calloc(MAX_LABEL, sizeof(char));
	params[1] = (char*)calloc(MAX_LABEL, sizeof(char));
	
	
	file_name = (char*)calloc(strlen(file_macro_name)-3, sizeof(char));/*allocating memory for the  file name, reducing three because of the ".am" chars of the given file*/
	file_ob_name = (char*)calloc(strlen(file_macro_name), sizeof(char));/*allocating memory for the new file name*/
	
	file = fopen(file_macro_name, "r");
	create_file_ent(file_macro_name, n1, e1);/*create the entry file*/
	
	str = strtok(file_macro_name, ".");/*getting the file name without the ending*/
	strcpy(file_name, str);
	strcpy(file_ob_name, file_name);
	strcat(file_ob_name, ".ob");/*creating the ".ob" file name*/
	
	ob_file = fopen(file_ob_name, "w");
	data_file = fopen("data", "w");
	while(fscanf(file, "%s", word) != EOF)
	{
		for(i = 0; instruc[i].opcode != -1; i++)
		{
			if(strcmp(word, instruc[i].name) == 0)/*if instruction has scanned*/
			{
				kidud_instruc(bits_line, instruc[i].opcode_bin, "opcode");/*coding opcode. for instruction bits_line the ARE bits are 00. because the bits_line is initialized the ARE bits are automatically 00*/
				is_instruct = 1;
				strcpy(com, word);/*copy the command*/
			}
		}
		if(is_instruct)/*if instruction has scanned*/
		{
			
			fgets(line, MAX_LINE, file);
			operands[0] = strtok(line, comma);
			if(operands[0][strlen(operands[0])-1] == '\n')/*if there is \n in the end, it means that there is one operand or no operands*/
			{
				remove_blanks(operands[0]);
				if(operands[0][0] != 0)/*if there is one operand*/
				{
					/*coding the rest of the instruction line*/
					kidud_instruc(bits_line, "00", "param1");
					kidud_instruc(bits_line, "00", "param2");
					kidud_instruc(bits_line, "00", "miun1");
					kidud_param_oper(bits_line, operands[0], "miun2");
				}
			}
			else/*if there is a second operand or brackets*/
			{
				if(strchr(operands[0], *bracket1))/*if there are brackets*/
				{
					miun2 = 1;
					str = (char*)calloc(strlen(operands[0]), sizeof(char));
					strcpy(str, operands[0]);/*first part of the sentence*/
					operands[0] = strtok(NULL, comma);/*second part of the sentence*/
					params[1] = strtok(operands[0], bracket2);/*getting the second param*/
					strcpy(help_var[1], params[1]);/*saving in the auxilary variables to prevent bugs*/
					params[0] = strtok(str, bracket1);
					strcpy(operands[0], params[0]);/*getting the first operand*/
					params[0] = strtok(NULL, bracket1);/*getting the first param*/
					strcpy(help_var[0], params[0]);/*saving in the auxilary variables to prevent bugs*/
					kidud_instruc(bits_line, "10", "miun2");/*coding the rest of the instruction line*/
					kidud_instruc(bits_line, "00", "miun1");
					kidud_param_oper(bits_line, help_var[0], "param1");
					kidud_param_oper(bits_line, help_var
					[1], "param2");
					free(str);
				}
				else /*if there is a second operand*/
				{
					kidud_instruc(bits_line, "00", "param1");/*coding the rest of the instruction line*/
					kidud_instruc(bits_line, "00", "param2");
					operands[1] = strtok(NULL, comma);/*getting the secong operand*/
					remove_blanks(operands[0]);
					remove_blanks(operands[1]);
					kidud_param_oper(bits_line, operands[0], "miun1");/*coding the rest of the instruction line*/
					kidud_param_oper(bits_line, operands[1], "miun2");
				}
			}
			fprintf(ob_file, "%s", bits_line);/*print to the file the bits line*/
			strcpy(bits_line, "00000000000000\n");/*reset the string*/
			
			
			i = 0;
			empty = 1;
			if (operands[0] != NULL)
			{
				while (operands[0][i] != '\0')/*checks if the first operand is empty*/
				{
					if(!isspace(operands[0][i]))/*if there is a non space char*/
					{
						empty = 0;
					}
					i++;
				}
			}
			if(empty)/*if first operands is empty make it null*/
			{
				operands[0] = NULL;
			}
			i = 0;
			empty = 1;
			if (operands[1] != NULL)
			{
				while (operands[1][i] != '\0')/*checks if the first operand is empty*/
				{
					if(!isspace(operands[1][i]))/*if there is a non space char*/
					{
						empty = 0;
					}
					i++;
				}
			}
			if(empty)/*if first operands is empty make it null*/
			{
				operands[1] = NULL;
				empty = 1;
			}
			i = 0;
			
			
			if(operands[1] != NULL &&
				type_operand(operands[1]) == 0 && 
				(strcmp(com, "mov") == 0 ||
				strcmp(com, "add") == 0 ||
				strcmp(com, "sub") == 0
				))/*checking for illegal addresing method in a second operand*/
			{
				fprintf(stderr, "ERROR in line %d: incorrect addressing method!\n", ic-FIRST_LINE);
				is_error = 1;
			}
			if(operands[1] == NULL || help_var[0][0] != 0)
			{
				if(operands[1] == NULL &&
				type_operand(operands[0]) == 0 && 
				(strcmp(com, "not") == 0 ||
				strcmp(com, "clr") == 0 ||
				strcmp(com, "lea") == 0 ||
				strcmp(com, "inc") == 0 ||
				strcmp(com, "dec") == 0 ||
				strcmp(com, "jmp") == 0 ||
				strcmp(com, "bne") == 0 ||
				strcmp(com, "red") == 0 ||
				strcmp(com, "jsr") == 0
				))/*checking for illegal addresing method in a singal operand*/
				{
					fprintf(stderr, "ERROR in line %d: incorrect addressing method!\n", ic-FIRST_LINE);
					is_error = 1;
				}
				print_binary(bits_line, NULL, operands[0], n1, ob_file);/*code binary lines for the operands*/
				
				if(miun2)
				{
					print_binary(bits_line, help_var[0], help_var[1], n1, ob_file);/*code binary lines for the operands*/
				}
			}
			else
			{
				if(strcmp(com, "lea") == 0 &&
				(type_operand(operands[0]) == 0 ||
				type_operand(operands[0]) == 2 ||
				type_operand(operands[1]) == 0)
				)/*check for illegal addressing methods in "lea" instruction*/
				{
					fprintf(stderr, "ERROR in line %d: incorrect addressing method!\n", ic-FIRST_LINE);
					is_error = 1;
				}
				print_binary(bits_line, operands[0], operands[1], n1, ob_file);/*code binary lines for the operands*/
			}
			
			/*resetting variables*/
			is_instruct = 0;
			operands[0] = NULL;
			operands[1] = NULL;
			help_var[0][0] = 0;
			empty = 1;
			miun2 = 0;
		}
		else if(strcmp(word, ".data") == 0)
		{/*if the scanned word is .data*/
			fgets(line, MAX_LINE, file);
			remove_blanks(line);
			str = strtok(line, comma);
			while(str != NULL)/*scanning every number*/
			{
				sscanf(str, "%d", &num);/*scans the number*/
				binary(bits_line, 0, 13, num);/*coding the number in dinary*/
				fprintf(data_file, "%s", bits_line);/*printing into the data file*/
				str = strtok(NULL, comma);/*scans the next number*/
			}
		}
		else if(strcmp(word, ".string") == 0)
		{/*if the scanned word is .string*/
			fgets(line, MAX_LINE, file);
			remove_blanks(line);
			for(i = 1; i < strlen(line)-1; i++)/*loop for coding the string*/
			{
				num = line[i];/*getting the ascii code of the chars*/
				binary(bits_line, 0, 13, num);/*coding the ascii code*/
				fprintf(data_file, "%s", bits_line);/*printing into the data file*/
			}
			num = 0;/*ascii code of NULL*/
			binary(bits_line, 0, 13, num);
			fprintf(data_file, "%s", bits_line);/*coding a null in the end of the string*/
		}
		strcpy(bits_line, "00000000000000\n");/*reset the string*/
	}
	fclose(file);
	fclose(ob_file);
	fclose(data_file);
	
	
	new_file_name = (char*)calloc(strlen(file_name)+4, sizeof(char));/*allocating memory for the new file name, adding three for the ".ent" or ".ext" or ".ob" chars*/
	strcpy(new_file_name, file_name);
	strcat(new_file_name, ".ob");
	
	data_file = fopen("data", "r");
	ob_file = fopen(new_file_name, "a");
	
	c = fgetc(data_file);
    while (c != EOF)/*copy the data file lines into the end of the object file*/
    {
        fputc(c, ob_file);
        c = fgetc(data_file);
    }
    
	fclose(ob_file);
	fclose(data_file);
	remove("data");
	
	create_file_ext(file_name);/*create the externals file*/
	uniqe_binary(file_name, ic, dc);/*change the object file to the unique binary format*/
	remove("copy");/*removing an auxilary file that created in uniqe_binary()*/
	
	
	strcpy(new_file_name, file_name);
	strcat(new_file_name, ".ent");
	file = fopen(new_file_name, "r");
	c = fgetc(file);
	while(c != EOF)/*checks if the .ent file is empty*/
	{
		if(!isspace(c))
		{
			empty_file = 0;
			break;
		}
		c = fgetc(file);	
	}
	fclose(file);
	if(empty_file)/*if the .ent file is empty delete it*/
	{
		remove(new_file_name);
	}
	
	empty_file = 1;
	
	strcpy(new_file_name, file_name);
	strcat(new_file_name, ".ext");
	file = fopen(new_file_name, "r");
	c = fgetc(file);
	while(c != EOF)/*checks if the .ext file is empty*/
	{
		if(!isspace(c))
		{
			empty_file = 0;
			break;
		}
		c = fgetc(file);	
	}
	fclose(file);
	if(empty_file)/*if the .ext file is empty delete it*/
	{
		remove(new_file_name);
	}
	if(is_error)/*if there is errors*/
	{
		remove(new_file_name);/*remove .ext file*/
		strcpy(new_file_name, file_name);
		strcat(new_file_name, ".ob");
		remove(new_file_name);/*remove .ob file*/
		strcpy(new_file_name, file_name);
		strcat(new_file_name, ".ent");/*remove .ent file*/
		remove(new_file_name);
	}
}





/*
printing the additional binary lines for an original instrucion line.
oper1 and oper2 are representing parameters or operands
*/
void print_binary(char bits_line[], char* oper1, char* oper2, node* n1, FILE* file)
{
	int i;
	int num;
	char* oper_cpy = {0};
	node* head;
	head = n1;
	strcpy(bits_line, "00000000000000");/*reset the string*/
	if(oper1 == NULL && oper2 == NULL)/*if both oper1 and oper2 are NULL exit the function*/
	{
		return;
	}
	if(oper1 != NULL)
	{
		remove_blanks(oper1);
		oper_cpy = (char*)calloc(strlen(oper1), sizeof(char));
		if(type_operand(oper1) == 2 && type_operand(oper2) == 2)/*if both operands are registers*/
		{
			num = oper1[1] - FIRST_CHAR;/*getting the num of the register by the diffrence between the ascii values of the chars*/
			binary(bits_line, 0, 5, num);/*coding the makor reg*/
			num = oper2[1] - FIRST_CHAR;/*getting the num of the register by the diffrence between the ascii values of the chars*/
			binary(bits_line, 6, 11, num);/*coding the yaad reg*/
			fprintf(file, "%s\n", bits_line);/*printing the coding into the object file*/
			return;
		}
		else
		{
			if(type_operand(oper1) == 0)/*if  oper1 is a number*/
			{
				strcpy(oper_cpy, oper1);
				for(i = 0; i < strlen(oper_cpy); i++)/*loop for moving the string one spot back and erasing the #*/
				{
					if(i == strlen(oper_cpy)-1)
					{
						oper_cpy[i] = 0;
					}
					else
					{
						oper_cpy[i] = oper_cpy[i+1];
					}
				}
				sscanf(oper_cpy, "%d", &num);/*getting the num*/
				binary(bits_line, 0, strlen(bits_line) - 3, num);/*coding the num*/
				/*minus three because the first two bits are ARE*/
			}
			else if(type_operand(oper1) == 1)/*if oper1 is label*/
			{
				while(head != NULL)/*loop for finding the node of the label*/
				{
					if(strcmp(oper1, head->name) == 0)
					{
						break;
					}	
					head = head->next;
				}
				binary(bits_line, strlen(bits_line)-2, strlen(bits_line)-1, head->type);/*coding the ARE bits*/
				
				num = check_name(oper1, n1);/*get the address of the label*/
				if(check_extern(oper1, n1))/*if the label is extern*/
				{
					num = 0;/*change his address to 0*/
					fprintf(file, "*%s*", head->name);/*print the name into the object file, this is for creating the external file*/
				}
				binary(bits_line, 0, strlen(bits_line) - 3, num);/*code the  address*/
				
			}
			else if(type_operand(oper1) == 2)/*if oper1 is register*/
			{ 
				num = oper1[1] - FIRST_CHAR;/*getting the num of the register by the diffrence between the ascii values of the chars*/
				binary(bits_line, 0, 5, num);/*coding the register*/
			}
		}
		fprintf(file, "%s\n", bits_line);/*printing the coding into the file*/
	}
	head = n1;
	strcpy(bits_line, "00000000000000");/*reset the string*/
	if(oper2 != NULL)
	{
		remove_blanks(oper2);
		if(type_operand(oper2) == 0)/*if  oper2 is a number - same as oper1*/
		{
			oper_cpy = (char*)calloc(strlen(oper2), sizeof(char));
			strcpy(oper_cpy, oper2);
			for(i = 0; i < strlen(oper_cpy); i++)
			{
				if(i == strlen(oper_cpy)-1)
				{
				
					oper_cpy[i] = 0;
				}
				else
				{
					oper_cpy[i] = oper_cpy[i+1];
				}
			}
			sscanf(oper_cpy, "%d", &num);
			binary(bits_line, 0, strlen(bits_line) - 3, num);
			/*minus three because the first two bits are ARE*/
		}
		else if(type_operand(oper2) == 1)/*if  oper2 is a label - same as oper1*/
		{
			remove_blanks(oper2);
			while(head != NULL)
			{
				if(strcmp(oper2, head->name) == 0)
				{
					break;
				}	
				head = head->next;
			}
			binary(bits_line, strlen(bits_line)-2, strlen(bits_line)-1, head->type);/*coding the ARE in the string*/
			num = check_name(oper2, n1);/*check the address of the label*/
			if(check_extern(oper2, n1))
			{
				num = 0;
				fprintf(file, "*%s*", head->name);
			}
			binary(bits_line, 0, strlen(bits_line) - 3, num);/*coding the binary address in the string*/
		}
		else if(type_operand(oper2) == 2)/*if  oper2 is a register - same as oper1*/
		{ 
			num = oper2[1] - 48;
			binary(bits_line, 6, 11, num);
		}
		fprintf(file, "%s\n", bits_line);
	}
}
	
	
	
/*
changing the given indexes of the given string to the binary value of the given num
*/	
void binary(char bits_line[], int first, int last, int num)
{
	int i;
	i = last;
	while(i != (first-1))/*iterates thrugh the indexes of the string*/
	{
		if (num & 1)/*if the first bit is 1*/
			bits_line[i] = '1';
		else/*if the first bit is 0*/
			bits_line[i] = '0';
		num >>= 1;
		i--;
	}
}


/*
creates the .ent file
*/
void create_file_ent(char* file_name, node* n1, entry* e1)
{
	char* new_file_name;
	char* str;
	FILE* file;
	node* n2;
	n2 = n1;
	new_file_name = (char*)calloc(strlen(file_name)+4, sizeof(char));/*allocating memory for the new file name, adding three for the ".ent" chars*/
	
	str = strtok(file_name, ".");
	strcpy(new_file_name, str);
	strcat(new_file_name, ".ent");
	file = fopen(new_file_name, "w");/*creating the .ent file*/
	
	while(e1 != NULL)/*for every entry label*/
	{
		while(n1 != NULL)/*for every label*/
		{
			if(strcmp(e1->name,n1->name) == 0)/*find the entry label in the labels*/
			{
				fprintf(file, "%s\t\t%d\n", n1->name, n1->address);/*print into the file the name and address*/
				break;
			}
			n1 = n1->next;
		}
		n1 = n2;/*reset the lables node*/
		e1 = e1->next;
		
	}
	fclose(file);
}


/*
creates the externals file
till this function every external label was written into the object file (in line 401) with asterisks, the function editing the object file to only zeros and ones and scanning the externals from the object file.
*/
void create_file_ext(char* file_name)
{
	int i = 0;
	char c;
	char* ast = "*";/*for strtok*/
	char* word;
	char* new_file_name;
	char line[MAX_LINE] = {0};
	FILE* file;
	FILE* file_copy;/*auxiliary file*/
	FILE* file_ext;
	new_file_name = (char*)calloc(strlen(file_name)+4, sizeof(char));/*allocating memory for the new file name, adding three for the ".ext" chars*/
	strcpy(new_file_name, file_name);
	strcat(new_file_name, ".ext");
	file_ext = fopen(new_file_name, "w");/*creating the externals file*/
	
	strcpy(new_file_name, file_name);
	strcat(new_file_name, ".ob");
	file = fopen(new_file_name, "r");/*opening the object file*/
	
	file_copy = fopen("copy", "w");/*creating the auxiliary file*/
	while(fscanf(file, "%s", line) != EOF)/*for every line in the object file*/
	{
		if(line[0] == '*')/*if the line starts with asterisk it has an external label name*/
		{
			word = strtok(line, ast);/*scans the name of the label*/
			fprintf(file_ext, "%s\t\t%d\n", word, FIRST_LINE+i);/*printing the name to the external file with the address*/
			word = strtok(NULL, ast);/*scans the rest of the line - the binary coding*/
			fprintf(file_copy, "%s\n", word);/*copy the binary coding into the auxiliary file*/
		}
		else
		{
			fprintf(file_copy, "%s\n", line);
		}
		i++;
	}
	fclose(file_ext);
	fclose(file);
	fclose(file_copy);
	
	file_copy = fopen("copy", "r");/*open the auxiliary file*/
	file = fopen(new_file_name, "w");/*open the object file*/
	
    c = fgetc(file_copy);
    while (c != EOF)/*copy the auxiliary file into the object file*/
    {
        fputc(c, file);
        c = fgetc(file_copy);
    }
	fclose(file);
	fclose(file_copy);
    
}


/*
changing the regulart object file into the unique binary format object
*/
void uniqe_binary(char* file_name, int ic, int dc)
{
	int i= FIRST_LINE;
	char c;
	char line[MAX_LINE];
	char* new_file_name;
	FILE* file;
	FILE* file_copy;/*auxiliary file*/
	new_file_name = (char*)calloc(strlen(file_name)+3, sizeof(char));/*allocating memory for the new file name, adding three for the ".ob" chars*/
	strcpy(new_file_name, file_name);
	strcat(new_file_name, ".ob");
	file = fopen(new_file_name, "r");/*opens the object file*/
	file_copy = fopen("copy", "w");/*creates the auxiliary file*/
    c = fgetc(file);
	while (c != EOF)/*for every char in the object file*/
    {
    	if(c == '0')
    		fputc('.', file_copy);
    	else if(c == '1')
    		fputc('/', file_copy);
    	else
        	fputc(c, file_copy);
        c = fgetc(file);
    }
    fclose(file);
    fclose(file_copy);
    
    /*copying the auxiliary file into the object file and transformint into the format*/
	file = fopen(new_file_name, "w");
	file_copy = fopen("copy", "r");
	fprintf(file, "\t%d %d\n", ic-FIRST_LINE-dc, dc);/*print the ic and dc in the head of the object file*/
    while(fscanf(file_copy, "%s", line) != EOF)/*scan line from the auxiliary file*/
	{
		fprintf(file, "0%d\t%s\n", i, line);/*print it into the object file*/
		i++;
	}
    fclose(file);
    fclose(file_copy);
}


