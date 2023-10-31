#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINE 81/*the maximum length of a line*//*check why its 81*/
#define MAX_LABEL 30/*the maximum length of a label*/
#define FIRST_LINE 100
#define MAX_OPER 2/*maximum number of operands*/
#define MAX_PARAM 2/*maximum number of parameters*/
#define NUM_REGS 8/*maximum number of registers*/
#define MAX_BITS 14/*maximum length of a bits_line*/
#define MAX_COM 4/*maximum length of a command*/
#define FIRST_CHAR 48/*the first alphabetic char in ascii code- 'a'*/

struct node/*struct for a node in a linked list*/
{
	char name[MAX_LABEL];
	int address;
	int type;/*boolean*/
	int is_instruct;/*boolean*/
	char* value;
	struct node* next;/*pointer to the next node*/
};
typedef struct node node;


struct entry/*struct for a node in a linked list*/
{
	char name[MAX_LABEL];
	struct entry* next;/*pointer to the next node*/
};
typedef struct entry entry;

node* add_label(char name[], int address, int type, int is_instruct, char* value, node* head);
int check_name(char name[], node* head);
void kidud_instruc(char line[], char* bits, char* part);
void remove_blanks(char line[]);
int type_operand(char* operand);
void kidud_param_oper(char bits_line[], char* str, char* part);
void second_passage(char* file_name, node* n1, entry* e1, int ic, int dc, int is_error);
void first_passage(char* file_name, int is_error);
void change_address(int address, char* name, node* n1);
int check_extern(char name[], node* head);
int check_entry(char name[], node* head);
void create_file_ent(char* file_name, node* n1, entry* e1);
void create_file_ext(char* file_name);
int is_exist(char name[], node* n1);
entry* add_entry(char name[], entry* head);
void uniqe_binary(char* file_name, int ic, int dc);

