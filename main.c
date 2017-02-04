#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 512

//List of built in commands in the shell
//If the command is not here, it is either an external program or an error
char *commands;
//Pointers to those built in commands
void (*functions[]) (int argc, char **argv);

//Returns the index of
int getCommandIndex();

//Returns all arguments in an array of strings
//Last pointer of the array in set to null
//If there are no arguments, only the last null pointer is returned
char **tokenize(char* string);
void execute();




int main(int argc, char **argv)
{
    char buffer[BUFF_SIZE];
    char **tokens;
    int functionIndex;
    int i;

    while(1){
        printf(">");
        gets(buffer);
        tokens = tokenize(buffer);
        i = 0;
        while (tokens[i] != null){
        	i++;
        }
        if (i > 0){
        	commandIndex = getCommandIndex(tokens[0]);
        	if (commandIndex >= 0){
        		(functions[commandIndex]) (i - 1, tokens+sizeof (tokens));
        	}
        }
    }
    return 0;
}
