#include "pre_ass.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*
writing the defined macros in the .as file at specified spots over the file and creating .am file
*/
void pre_proccecor(char* file_name, char* file_as_name)
{
	FILE* file;/*pointer to the original file*/
	FILE* file_as;/*pointer to the new .am file*/
	mac* n1;/*node for the linked list*/
	int first_mcr = 1;/*boolean*/
	char c;
	char word[MAX_LABEL];
	char key[MAX_LABEL];
	char line[MAX_LINE] = {0};
	char* file_am_name;/*.am file name*/
	char* content;/*the content of the macro*/
	char* value = NULL;
	file = fopen(file_as_name, "r");
	while (fscanf(file, "%s", word) != EOF)/*iterating thruogh every word in the original file*/
	{
		if(strcmp(word,"mcr") == 0)/*if the scanned word is a beginning of a macro*/
		{
			fscanf(file, "%s", key);/*scans the name of the macro*/
			fscanf(file, "%s", word);/*scans the first word of the content*/
			content = (char*)calloc(strlen(word),sizeof(char));/*allocating memory for the word*/
			strcat(content, word);/*adding the word into the content of the macro*/
			fgets(line, MAX_LINE, file);/*scan a new line*/
			while(!strstr(line, "endmcr"))/*iterating through the content of the macro till a "mcr" is scanned*/
			{
				content = (char*)realloc(content, (strlen(content)+strlen(line)) * sizeof(char));/*allocating new memory for the scanned line*/
				strcat(content, line);/*adding the line into the content of the macro*/
				fgets(line, MAX_LINE, file);/*scan a new line*/
			}
			if(first_mcr)/*if its the first macro*/
			{
				n1 = add_node(key, content, NULL);/*add a node that pointing to null*/
				first_mcr = 0;/*resetting the boolan to false*/
			}
			else
			{
				n1 = add_node(key, content, n1);/*add a node that pointing to the previous node*/
			}
			free(content);
		}
	}
	file_am_name = (char*)calloc(strlen(file_name)+3, sizeof(char));/*allocating memory for the new file name, adding three for the ".am" chars*/
	strcpy(file_am_name, file_name);
	strcat(file_am_name, ".am");
	file_as = fopen(file_am_name, "w");/*create the .am file*/
	rewind(file);/*reset the pointer file to the head of the file*/
	while (fscanf(file, "%s", word) != EOF)/*iterating thruogh every word in the original file*/
	{
		if(strcmp(word, "mcr") == 0)/*if the scanned word is a start of a macro*/
		{
			while(strcmp(word, "endmcr") != 0)/*a loop that skips on the macro and not copying it to the new file*/
			{
				fscanf(file, "%s", word);/*skips the words*/
			}
			c = fgetc(file);/*moving the pointer one step further to skip the \n char*/
		}
		else if((value = check_key(word, n1)))/*if the word is a name of a macro*/
		{
			fwrite(value, 1, strlen(value), file_as);/*copy the content of the macro to the new file*/
			c = fgetc(file);/*moving the pointer one step further to skip the \n char*/
		}
		else/*if the word is not a name of macro or a beginning of a macro*/
		{
			fwrite(word, 1, strlen(word), file_as);/*copy the word to the new file*/
			
		}
		c = fgetc(file);/*because I used the fscanf function there are white spaces chars after word that need to be copied*/
			while(isspace(c))/*a loop for copying all the whitespaces between the word and the next word*/
			{
				fputc(c, file_as);/*copying the white space to the new file*/
				c = fgetc(file);/*scanning a new white space*/
			}
			fseek(file, -1, SEEK_CUR);/*the file pointer is pointing to a char so it need to be returned by 1 to copy the whole next word*/
		
	}
	free(file_am_name);
	fclose(file_as);
	fclose(file);
}


/*
adding a macro to the linked list of macros
returns the pointer to the last macro node
*/
mac* add_node(char key[], char* value, mac* head)
{
	/*copying each parameter into the node's members*/
	mac* n1 = (mac*)malloc(sizeof(mac));
	n1->value = (char*)malloc(strlen(value));
	strcpy(n1->key,key);
	strcpy(n1->value,value);
	n1->next = head;
	return n1;
}


/*
checking if key macro is exisiting in the macros linked list.
returnes the value of the macro if it exists. otherwise returnes NULL
*/
char* check_key(char key[], mac* head)
{
	mac* n1 = head;
	while (n1 != NULL)
	{
		if (strcmp(n1->key,key) == 0)
		{
			return n1->value;
		}
		n1 = n1->next;
	}
	return NULL;
}
	
	

