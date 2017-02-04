#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 512

void help(int argc, char **argv);
void print(int argc, char **argv);
void exitProgram(int argc, char **argv);

//List of built in commands in the shell
//If the command is not here, it is either an external program or an error
char *commands[] = {
    "print",
	"help",
	"exit"
};
//Pointers to those built in commands
void (*functions[]) (int argc, char **argv) = {
    print,
    help,
    exitProgram
};

//Returns the index of
int getCommandIndex(char* command);

//Returns all arguments in an array of strings
//Last pointer of the array in set to null
//If there are no arguments, only one element wide array with the last NULL pointer is returned
char **tokenize(char* string);




int main(int argc, char **argv)
{
    char buffer[BUFF_SIZE];
    char **tokens;
    int functionIndex;
    int i;
    int commandIndex;

    while(1){
        printf(">");
        gets(buffer);
        tokens = tokenize(buffer);
        i = 0;
        while (tokens[i] != NULL){
        	i++;
        }
        if (i > 0){
            commandIndex = getCommandIndex(tokens[0]);
            if (commandIndex >= 0){
                (functions[commandIndex]) (i - 1, ++tokens);
            }
        }
    }
    return 0;
}


int getCommandIndex(char *command) {
	int i = 0;
	while (commands[i] != NULL) {
		if (strcmp(command, commands[i]) == 0) {
			return i;
		}
		i++;
	}
	return -1;
}


char **tokenize(char *string) {
	//printf("debugTokenize");
	int index = 0;
	int i;
	int size = 8;
	char **tokens = malloc(size * sizeof(char*));
	char **tokensTemp;
	char *token = strtok(string, " ");
	while (token != NULL) {
		tokens[index] = token;
		token = strtok(NULL, " ");
		index++;
		if (index == size - 1) {
			tokensTemp = tokens;
			tokens = malloc(2 * size * sizeof(char*));
			for (i = 0; i < size; i++) {
				tokens[i] = tokensTemp[i];
			}
			free(tokensTemp);
			size = size * 2;
		}
	}
	tokens[index] = NULL;
	return tokens;
}

void help(int argc, char **argv) {
	printf("HELP! I need somebody. HELP! Not just anybody.\r\n");
}

void print(int argc, char **argv) {
	if (argc > 0) {
		printf(argv[0]);
	}
	else {
		printf("I don't know what to print...\r\n");
	}
}

void exitProgram(int argc, char **argv) {
	exit(0);
}
