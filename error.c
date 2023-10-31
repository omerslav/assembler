#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "error.h"
#define MAX_LABEL 30 /* max length of a label */
#define MAX_LINE 80 /* max length of assembly file line */

int check_error_in_line(char* line, int line_num);
int are_strings_equal(char* str1, int len1, char* str2);





/*
 * Checks the contents of the input assembly file for errors and prints any 
 * errors found to sdterr. 
 * If errors are found, returns 1. Otherwise, returns 0.
 */
int check_error(FILE* fp)
{
	int is_error = 0;
	int line_num = 0;
	char line[MAX_LINE];
	
	while (fgets(line, MAX_LINE, fp))
	{       
		line_num++;
	
		/* Check the current line for errors */
		if (check_error_in_line(line, line_num))
		{
			is_error = 1;
		} 
	}

	return is_error;	
}

/*
 * Reads all space characters starting from input index and returns the index
 * immediately following them or the input index if there are none.
 */
int read_spaces(char* line, int i)
{
    while (i < MAX_LINE && line[i] != '\n' && isspace(line[i]))
    {
    	i++;
    }
    
    return i;
}

/*
 * Reads all nonspace characters starting from input index and returns the index
 * immediately following them or the input index if there are none.
 */
int read_nonspaces(char* line, int i)
{
    while (i < MAX_LINE && !isspace(line[i]) && line[i] != '\n')
    {
    	i++;
    }
    
    return i;
}

/*
 * Reads all number characters (+/- symbol followed by digits) starting from
 * the input index and returns the index immediately following them or the
 * input index if there is no valid number to read.
 */
int read_number(char* line, int i)
{
	if (i >= MAX_LINE)
	{
		return i;
	}
	
	if (line[i] == '-' || line[i] == '+')
	{
	 	if (i + 1 < MAX_LINE && !isdigit(line[i + 1]))
	 	{
	 	    /* The number must immediately follow the sign */
	 		return i;
	 	}
	 	
	 	i++;
 	}
	
	while (i < MAX_LINE && isdigit(line[i]))
	{
	    i++;
	}
	
	return i;
}

/*
 * Reads all the characters of a string from the starting double quote to
 * the last one in the line starting from input index and returns the index
 * immediately following the last double quote or the input index if there
 * is no valid string to read.
 */
int read_string(char* line, int i)
{
	int j;
	if (i >= MAX_LINE)
	{
		return i;
	}
	
	if (line[i] != '"')
	{
		/* String must start with a double quote */
	 	return i;
 	}

    /* Find the last double quote in the line starting from the newline */	
	j = i + 1;
	while (j < MAX_LINE && line[j] != '\n')
	{
	    j++;
	}

	while (j > i)
	{
		j--;
		if (line[j] == '"')
		{
			break;
		}
	}	
	
	if (j == i)
	{
        /* There was no closing double quote */
		return i;
	}
	
	return j + 1;
}

/*
 * Reads all identifier characters (a letter followed by letters and digts)
 * starting from the input index and returns the index
 * immediately following them or the input index if there is no valid 
 * identifier.
 */
int read_identifier(char* line, int i)
{
	if (i >= MAX_LINE)
	{
		return i;
	}

	if (!isalpha(line[i]))
	{
		return i;
	}
	
	i++;
	while (i < MAX_LINE && (isalpha(line[i]) || isdigit(line[i])))
	{
		i++;
	}
	
	return i;
}

/*
 * Reads all the characters of a non-parmeterized operand (e.g., r1, #12, etc.)
 * starting from the input index and returns the index immediately following
 * them or the input index if there is no valid operand to read.
 */
int read_non_param_operand(char* line, int i)
{
	int j;
	int failed = i;
	
	if (i >= MAX_LINE)
	{
		return i;
	}
	
	if (line[i] == '#')
	{
		i++;
		j = read_number(line, i);
		
		if (i == j)
		{
			return failed;
		}
	}
	else
	{
		j = read_identifier(line, i);
		
		if (i == j)
		{
			return failed;
		}
	}
	
	return j;
}

/*
 * Reads all the characters of an operand (e.g., r1, #12, L1(r1,#12), etc.)
 * starting from the input index and returns the index immediately following
 * them or the input index if there is no valid operand to read.
 */
int read_operand(char* line, int i)
{
	int j;
	int failed = i;
	
	if (i >= MAX_LINE)
	{
		return i;
	}
	
	j = read_non_param_operand(line, i);
	if (i == j)
	{
		/* Could not find operand */
		return failed;
	}
	
	i = j;
	
    /* Read the operand parameters if there are any */
	if (i < MAX_LINE && line[i] == '(')
	{
		i++;
		j = read_non_param_operand(line, i);
		if (i == j)
		{
			/* Could not read the first operand */
			return failed;
		}
	
		i = j;
		if (i >= MAX_LINE - 2)
		{
			/* No place for the ',' and second operand */
			return failed;

		}

		if (line[i] != ',')
		{
			return failed;	
		}
				
		i++;
		j = read_non_param_operand(line, i);
		if (i == j)
		{
			/* Could not read the second operand */
			return failed;
		}

		i = j;
		if (i >= MAX_LINE || line[i] != ')')
		{
			/* Missing closing paren */
			return failed;
		}
		
		i++;
	}
	
	return i;
}

/*
 * Returns 1 if the string of the input length pointed to by str1 is equal to
 * the string str2. Otherwise, returns 0.
 */
int are_strings_equal(char* str1, int len1, char* str2)
{
	int i;
    int len2 = strlen(str2);
    
	if (len1 != len2)
	{
		return 0;
	}
	
	for (i = 0; i < len1; ++i)
	{	
		if (str1[i] != str2[i])
		{
			return 0;
		}
	}
	
	return 1;
}

/*
 * Checks the contents of the input assembly line for errors and prints any 
 * errors found to sdterr. 
 * If errors are found, returns 1. Otherwise, returns 0.
 */
int check_error_in_line(char* line, int line_num)
{
	const char* ERROR_FMT = "ERROR in line %d: %s\n";
	int i = 0;
	int j = 0;
	
    /* Skip spaces at the beginning of the line */
	i = read_spaces(line, i);
	j = i;
	
	if (line[j] == '\n' || j >= MAX_LINE)
	{
		/* it is an empty line, ignore and continue to the next line */
		return 0;
	}
	
	if (line[j] == ';')
	{
		/* It is a note, ignore and continue to the next line */
		return 0;
	}
	
	if (isalpha(line[j]))
	{
		/* This is either a label or an operation.
           We search for the ':' to deteremine which one it is */
		while (j < MAX_LINE && !isspace(line[j]))
		{
	 		j++;
		}
		
		if (i == j)
		{
			fprintf(stderr, ERROR_FMT, line_num, "Missing command or label\n");
			return 1;
		}
		
		if (line[j - 1] == ':')
		{
			/* This is a label. Make sure it is not too long */
			if (j - i - 1 > MAX_LABEL)
			{
				fprintf(stderr, ERROR_FMT, line_num, "Label is too long!");
				return 1;
			}	
			
			/* Make sure label consists of numbers and letters */
			while (i < j - 1)
			{
				if (!isalpha(line[i]) && !isdigit(line[i]))
				{
				    fprintf(stderr, ERROR_FMT, line_num, "Illegal one or more characters in label!");
					return 1;
				}
				
				i++;
			}
			
			i = j;	
		}
		else
		{
			/* This is not a label, we should recheck as an operation */
			j = i;
		}
	}

	i = read_spaces(line, i);
	
	/* We expect an instruction or an operation */
	if (i > MAX_LINE)
	{
		fprintf(stderr, ERROR_FMT, line_num, "Missing instruction or operation!");
		return 1;
	}

	else if (line[i] == '.')
	{
		/* This is an instruction. Find it's name */
		j = read_nonspaces(line, i);
		
		/* Make sure this is a valid instruction */
		if (are_strings_equal(line + i, j - i, ".data"))
		{
		    /* We expect a list of numbers */
		  	i = j;
		  	i = read_spaces(line, i);
		  	j = read_number(line, i);
		  	if (i == j)
		  	{
		  		fprintf(stderr, ERROR_FMT, line_num, "Missing or inavlid number!");
		  		return 1;
		  	}
		  	
		  	i = read_spaces(line, j);
		  	while (i < MAX_LINE && line[i] == ',')
		  	{
		  		i++;
		  		i = read_spaces(line, i);
			  	j = read_number(line, i);
			  	if (i == j)
			  	{
			  		fprintf(stderr, ERROR_FMT, line_num, "Missing or inavlid number!");
			  		return 1;
			  	}

		  		i = read_spaces(line, j);
		  	}
		}
		else if (are_strings_equal(line + i, j - i, ".string"))
		{
		    /* We expect a string */
		  	i = j;
		  	i = read_spaces(line, i);
		  	j = read_string(line, i);
		    if (i == j)
		    {
		    	fprintf(stderr, ERROR_FMT, line_num, "Missing or inavlid string!");
				return 1;
		    }
		}
		else if (are_strings_equal(line + i, j - i, ".entry") ||
			are_strings_equal(line + i, j - i, ".extern"))
		{
		    /* We expect an identifier */
		    i = j;
		    i = read_spaces(line, i);
		    j = read_identifier(line, i);
		    if (i == j)
		    {
		    	fprintf(stderr, ERROR_FMT, line_num, "Missing or inavlid label name!");
				return 1;
		    }
		}
		else
		{
			fprintf(stderr, ERROR_FMT, line_num, "Invalid instruction!");
			return 1;
		}
	}
	else
	{
		/* This is an operation. Find it's name */
		j = read_nonspaces(line, i);
		if (i == j)
		{
			fprintf(stderr, ERROR_FMT, line_num, "Missing operation!");
			return 1;
		}
		
		if (are_strings_equal(line + i, j - i, "stop") ||
			are_strings_equal(line + i, j - i, "rts"))
		{
			/* No operands expected */
		}
		else if (are_strings_equal(line + i, j - i, "not") ||
			are_strings_equal(line + i, j - i, "clr") ||
			are_strings_equal(line + i, j - i, "inc") ||
			are_strings_equal(line + i, j - i, "dec") ||
			are_strings_equal(line + i, j - i, "jmp") ||
			are_strings_equal(line + i, j - i, "bne") ||
			are_strings_equal(line + i, j - i, "red") ||
			are_strings_equal(line + i, j - i, "prn") ||
			are_strings_equal(line + i, j - i, "jsr"))
		{
			/* Single operand expected */
			i = j;
			i = read_spaces(line, i);
			j = read_operand(line, i);
			if (i == j)
			{
				fprintf(stderr, ERROR_FMT, line_num, "Missing or invalid operand!");
				return 1;
			}
		}
		else if (are_strings_equal(line + i, j - i, "mov") ||
			are_strings_equal(line + i, j - i, "cmp") ||
			are_strings_equal(line + i, j - i, "add") ||
			are_strings_equal(line + i, j - i, "sub") ||
			are_strings_equal(line + i, j - i, "lea"))
		{
			/* Two operands expected */
			i = j;
			i = read_spaces(line, i);
			j = read_operand(line, i);
			if (i == j)
			{
				fprintf(stderr, ERROR_FMT, line_num, "Missing or invalid first operand!");
				return 1;
			}
			
			i = j;
			i = read_spaces(line, i);
			
			if (i >= MAX_LINE || line[i] != ',')
			{
				fprintf(stderr, ERROR_FMT, line_num, "Missing second operand!");
				return 1;
			}
			
			i++;
			i = read_spaces(line, i);
			j = read_operand(line, i);

			if (i == j)
			{
				fprintf(stderr, ERROR_FMT, line_num, "Missing or inavlid second operand!");
				return 1;
			}
		}
		else
		{
			fprintf(stderr, ERROR_FMT, line_num, "Missing or unknown operation!");
			return 1;
		}
	}
	
	/* Make sure the remainder of the line is empty */
	i = j;
	j = read_spaces(line, i);
	if (j < MAX_LINE && line[j] != '\n')
	{
		fprintf(stderr, ERROR_FMT, line_num, "Invalid charcters at end of line!");
		return 1;
	}
	
	/* No errors found */
	return 0;
}

