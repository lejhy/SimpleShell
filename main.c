#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define BUFF_SIZE 512

char path[128];
// make these variables global for now
int count;
char **files;

void cd(int argc, char **argv);
void getPath(int argc, char **argv);
void setPath(int argc, char **argv);
void history(int argc, char **argv);
void alias(int argc, char **argv);
void unalias(int argc, char **argv);
void help(int argc, char **argv);
void print(int argc, char **argv);
void exitProgram(int argc, char **argv);
// aux function
void printdir(int argc, char **argv);

//List of built in commands in the shell
//If the command is not here, it is either an external program or an error
char *commands[] = {
  "cd",
  "getpath",
  "setpath",
  "history",
  // "!!",
  // "!<no>",int chdir(const char *path); 
  // "!-<no>",
  "alias",
  "unalias",
  "print",
	"help",
	"exit",
	"printdir",
	NULL
};

//Pointers to those built in commands
void (*functions[]) (int argc, char **argv) = {
  cd,
  getPath,
  setPath,
  history,
  alias,
  unalias,
  print,
  help,
  exitProgram,
  printdir

};

//Returns the index of
int getCommandIndex(char* command);

//Returns all arguments in an array of strings
//Last pointer of the array in set to null
//If there are no arguments, only one element wide array with the last NULL pointer is returned
char **tokenize(char* string);

char printContents(const char *path, char ***ls);

int main(int argc, char **argv)
{
    char buffer[BUFF_SIZE];
    char **tokens;
    int functionIndex;
    int argumentsIndex;
    int commandIndex;
    pid_t processID;

	//Main loop
    while(1){
      getcwd(path, 128);
      printf("%s>", path);
  		if (fgets(buffer, 512, stdin) == NULL) {
  			exit(0);
  		}

      tokens = tokenize(buffer);
  		argumentsIndex = 0;
  		while (tokens[argumentsIndex] != NULL) {
  			argumentsIndex++;
  		}
          if (argumentsIndex > 0){
              commandIndex = getCommandIndex(tokens[0]);
              if (commandIndex >= 0){
                (functions[commandIndex]) (argumentsIndex - 1, ++tokens);
              } else {
                processID = getpid();
                fork();
                if(processID == getpid()){
                  //parent
                  wait();
                } else {
                  //child
                  //This part needs fixing
                  if (execv(path, tokens) == -1){
                    int j =0;
                    while (tokens[j] != NULL){
                      printf("%s\n", tokens[j]);
                      j++;
                    }
                    exit(0);
                  }
                }
              }
          }
    }
    return 0;
}

//Checks whether the given string is a built in command
//Returns the command index
//Returns negative value if no match was found
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

//Splits the input into an array of strings and returns it as a pointer
char **tokenize(char *string) {
	int index = 0;
	int i;
	int size = 8;
	char **tokens = malloc(size * sizeof(char*));
	char **tokensTemp;
	//Get the first token
	char *token = strtok(string, "[\n<>;|& \t]");
	//Get all the remaining tokens
	while (token != NULL) {
		tokens[index] = token;
		token = strtok(NULL, "[\n<>;|& \t]");
		index++;
		//Checks whether more memory is needed and then allocates it
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
	//Last pointer is set to NULL and the pointer to the array is returned
	tokens[index] = NULL;
	return tokens;
}

//All the buid in commands should be declared here
void help(int argc, char **argv) {
  if (argc == 0){
	  printf("HELP! I need somebody. HELP! Not just anybody.\r\n");
  } else {
    printf("Invalid arguments");
  }
}

void print(int argc, char **argv) {
  int i = 0;
	for (i; i < argc;i++) {
    printf("'");
		printf("%s", argv[i]);
    printf("'");
    printf("\r\n");
	}
}

void exitProgram(int argc, char **argv) {
  if (argc == 0){
	  exit(0);
  } else {
    printf("Invalid arguments");
  }
}
 

//helper function to return the contents of current working directory
char printContents(const char *path, char ***ls) {
    size_t count = 0;
    size_t length = 0;
    DIR *dp = NULL;
    struct dirent *ep = NULL;

    dp = opendir(path);
    if(NULL == dp) {
        fprintf(stderr, "no such directory: '%s'", path);
        return 0;
    }

    *ls = NULL;
    ep = readdir(dp);
    while(NULL != ep){
        count++;
        ep = readdir(dp);
    }

    rewinddir(dp);
    *ls = calloc(count, sizeof(char *));

    count = 0;
    ep = readdir(dp);
    while(NULL != ep){
        (*ls)[count++] = strdup(ep->d_name);
        ep = readdir(dp);
    }

    closedir(dp);
    return count;
}

void cd(int argc, char **argv){
	if (argc == 0){
		printf("No argument enetered. You need to enter the name of the working directory\n");
		return;
	}

	for(int i = 0; i < count; i++) {
		if(strcmp(argv[0], files[i]) == 0) {
			chdir(files[i]);
		}

	}
}

void getPath(int argc, char **argv){
	printf("%s\n", path);
}
void setPath(int argc, char **argv){

}
void history(int argc, char **argv){

}
void alias(int argc, char **argv){

}
void unalias(int argc, char **argv){

}

void printdir(int argc, char **argv){
	int i;
	if (argc == 0){
		count = printContents(path, &files);
		for (i = 0; i < count; i++) {
			printf("%s\n", files[i]);
		}
	} else {
		printf("No arguments required for this command to run\n");
	}
}
