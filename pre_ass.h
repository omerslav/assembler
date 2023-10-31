#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINE 81/*the maximum length of a line*/
#define MAX_LABEL 30/*the maximum length of a label*/

struct mac/*struct for a macro node  in a linked list*/
{
	char key[MAX_LABEL];
	char* value;
	struct mac* next;/*pointer to the next node*/
};
typedef struct mac mac;

mac* add_node(char key[], char* value, mac* last_mac);
char* check_key(char key[], mac* head);
void pre_proccecor(char* file_name, char* file_as_name);
