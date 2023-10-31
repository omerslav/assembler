#include "pre_ass.h"
#include "assembler.h"
#include "error.h"
#include <stdio.h>
#include <string.h>

/*
wrote by Omer Slavkin and Yarden Carmi
Omer Slavkin ID: 215975434
Yarden Carmi ID: 328154596
*/
int main(int argc, char *argv[])
{	
	
	int i = 1;
	char* file_name;
	char* file_as_name;
	char* file_am_name;
	char* file_ob_name;
	FILE* file;
	if(argc < 2)/*if there are no specified files in the arguments*/
	{
		fprintf(stderr, "ERROR: there are no specified files!\n");
		return 1;
	}
	for (i = 1; i < argc; i++)
	{
		file_name = (char*)calloc(strlen(argv[i]), sizeof(char));/*allocating memory as the length of the argument*/
		strcpy(file_name, argv[i]);
		file_as_name = (char*)calloc(strlen(argv[i]+3), sizeof(char));/*allocating memory for the new file name, adding three for the ".as" chars*/
		strcpy(file_as_name, file_name);
		strcat(file_as_name, ".as");
		file = fopen(file_as_name, "r");
		printf("file \"%s\":\n", file_name);
		if(file == NULL)/*if the file cant open*/
		{
			fprintf(stderr, "ERROR: cant open file \"%s\"\n", argv[i]);
			printf("-------------\n");
			continue;
		}
		else
		{
			/*errors for macro*/
			fclose(file);
			pre_proccecor(file_name, file_as_name);/*activate the macros function*/
			
			file_am_name = (char*)calloc(strlen(file_name)+3, sizeof(char));/*allocating memory for the new file name, adding three for the ".am" chars*/
			strcpy(file_am_name, file_name);
			strcat(file_am_name, ".am");
			file = fopen(file_am_name, "r");/*open the after macro file ".am"*/
			
			if(!check_error(file))/*activate the finding errors function, if there are errors first and second passages will not activate*/
			{
				fclose(file);
				first_passage(file_am_name, 0);/*starting the two passages, first_passage() will activate the second_passage()*/
			}
			else
			{
				fclose(file);
				first_passage(file_am_name, 1);
				file_ob_name = (char*)calloc(strlen(file_name)+3, sizeof(char));/*allocating memory for the new file name, adding three for the ".am" chars*/
				strcpy(file_ob_name, file_name);
				strcat(file_ob_name, ".ob");
				remove(file_ob_name);/*delete object file if there are errrors*/
				
			}
			free(file_am_name);	
		}
		printf("-------------\n");
		free(file_name);
		free(file_as_name);
	}
	return 0;
}


