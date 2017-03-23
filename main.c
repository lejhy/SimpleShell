#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define BUFF_SIZE 512
#define historySize 20
#define size_of_aliases 10

char path[256];
char directory[128];
char previousDirectory[128];
// make these variables global for now
int count;
char **files;
int alias_count = 0;


typedef struct{
    char * alias;
    char * command;
} aliasElement;

aliasElement aliases[size_of_aliases];


void cd(int argc, char **argv);
void getPath(int argc, char **argv);
void setPath(int argc, char **argv);
void history(int argc, char **argv);
void alias(int argc, char **argv);
void unalias(int argc, char **argv);
void help(int argc, char **argv);
void print(int argc, char **argv);
void exitProgram(int argc, char **argv);
void add_alias();
void print_alias();
void update_alias(int index, char **argv);
int alias_exists(char *target_alias);



// aux function
void printdir(int argc, char **argv);

//List of built in commands in the shell
//If the command is not here, it is either an external program or an error
char *commands[] = {
	"cd",
	"getpath",
	"setpath",
	"history",
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

char printContents(const char *directory, char ***ls);
void saveHistoryToFile();
void loadHistoryFromFile();
void updateHistory();

char *historyArray[historySize];
int historyCounter = 0;
int historyArrayCounter = 0;

int main(int argc, char **argv)
{
    char buffer[BUFF_SIZE];
    char **tokens;
    int functionIndex;
    int argumentsIndex;
    int commandIndex;
    pid_t processID;
		//int alias_size;

		// dynamic memory allocation for structs
	//	*aliases = malloc(alias_size * sizeof(aliasElement));

		//get the paths of the current working directory and home directory
		strcpy(path, getenv("PATH"));
		strcpy(directory, getenv("HOME"));
		chdir(directory);

    loadHistoryFromFile();

		//Main loop
	  while(1){
	    printf("%s>", directory);

	    //check for ctrl-D (EOF)
	  	if (fgets(buffer, 512, stdin) == NULL) {
				printf("\n");
	  		exitProgram(0,0);
	  	}



			//check fo history
			if (buffer[0] == '!') {
				if (strcmp(buffer, "!!\n") == 0) {
					if (historyCounter == 0) {
						printf("Error: Empty history!\n");
						continue;
					} else if (historyArrayCounter == 0) {
						// pointer at the beginning, last one at the end
						int lastCommand = historySize - 1;
						strcpy (buffer, historyArray[lastCommand]);
					} else {
						int lastCommand = historyArrayCounter - 1;
						strcpy (buffer, historyArray[lastCommand]);
					}
				} else {
					// shift the buffer
					int i = 1;
					while (buffer[i] != '\0') {
						buffer[i-1] = buffer[i];
						i++;
					}
					buffer[i-1] = '\0';
					// convert to number
					int command = atoi(buffer);
					if (command < 0) {
						command = historyCounter + command - 1;
					}
					if (command >= 0 && command < historyCounter && command >= historyCounter-historySize) {
						command = command%historySize;
						strcpy (buffer, historyArray[command]);
					} else {
						printf("Error: Bad command number!\n");
						continue;
					}
				}
			} else if (buffer[0] != '\n') {
	      historyArray[historyArrayCounter] = malloc(BUFF_SIZE*sizeof(char));
	      strcpy(historyArray[historyArrayCounter], buffer);
	      historyCounter++;
	      historyArrayCounter = historyCounter%historySize;
	    }

	      tokens = tokenize(buffer);

        // check for aliases
        int aliasIndex = alias_exists(tokens[0]);
        if (aliasIndex >= 0){
          strcpy(buffer, aliases[aliasIndex].command);
            tokens = tokenize(buffer);
        }

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
	                  wait(NULL);
	                } else {
	                  //child
	                  if (execvp(tokens[0], tokens) < 0){
											perror(tokens[0]);
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
	  printf("Here is a list of commands available.\r\n");
      int i =0;
      for(i=0;i<10;i++){
          char *pos = commands[i];
          while(*pos != '\0'){
            printf("- ");

          while(*pos != '\0'){
            printf("%c", *(pos++));
              }
            }
                     printf("\n");
                   }
}
      else {
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

		saveHistoryToFile();

		if (setenv("PATH", path, 1) != -1){
			printf("Seting path to: %s\n", path);
		};
	  exit(0);
  } else {
    printf("Invalid arguments");
  }
}


//helper function to return the contents of current working directory
char printContents(const char *directory, char ***ls) {
    size_t count = 0;
    size_t length = 0;
    DIR *dp = NULL;
    struct dirent *ep = NULL;

    dp = opendir(directory);
    if(NULL == dp) {
        fprintf(stderr, "no such directory: '%s'", directory);
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


//PLS DON'T OVERWITE THIS, WOJTEK
//REALLY
// or what?, Fraser

void cd(int argc, char **argv){
	char newDirectory[400];
	int isSuccess;

	//executes chdir(args) command depending on the type
	//of arguments used, stores result in isSuccess
	if (argc == 0){
		isSuccess = chdir(getenv("HOME"));
		//if no problems with chdir
		if(isSuccess == 0){
			strcpy(previousDirectory, directory); //save old dir
			getcwd(newDirectory,400);							//get cur dir
			strcpy(directory,newDirectory);				//save cur dir
		} else {
			perror("Error");
		}
	} else if(argc == 1) {
		if(strcmp(*argv,"~") == 0 ) {
			isSuccess = chdir(getenv("HOME"));
		} else if(strcmp(*argv, "-") == 0) {
			isSuccess = chdir(previousDirectory);
		} else {
			isSuccess = chdir(*argv);
		}
		//if no problems with chdir
		if(isSuccess == 0){
			strcpy(previousDirectory, directory); //save old dir
			getcwd(newDirectory,400);							//get cur dir
			strcpy(directory,newDirectory);				//save cur dir
		} else {
			perror("Error");
		}
	} else {
		printf("Error : Too many arguments!\n");
	}
}

// print the path
void getPath(int argc, char **argv){
	if (argc == 0) {
		printf("%s\n", getenv("PATH"));
	} else {
		printf("Error: Invalid number of arguments!\n");
	}
}

// set the path
void setPath(int argc, char **argv){
	if (argc == 1) {
		if (setenv("PATH", *argv, 1) == -1){
			perror("Error");
		}
	} else {
		printf("Error: Invalid number of arguments!\n");
	}

}

void history(int argc, char **argv){
  if (argc == 0){
    int commandNumber = 0;
    if (historyCounter >= historySize){
      commandNumber = historyCounter - historySize;
      for(int i = historyArrayCounter; i < historySize; i++){
        printf("%d: %s", commandNumber, historyArray[i]);
        commandNumber++;
      }
    }
	  for(int i = 0; i < historyArrayCounter; i++){
      printf("%d: %s", commandNumber, historyArray[i]);
      commandNumber++;
    }
  } else {
    printf("Error: Invalid arguments\n");
  }
}

void loadHistoryFromFile() {
	FILE *fp;
	char c[BUFF_SIZE+1];

	if( (fp = fopen(".hist_list", "r")) == NULL){
		perror("failed to open stream");

	} else {
    if (fgets(c, BUFF_SIZE, fp) != NULL)
      historyCounter = atoi(c);
    if (fgets(c, BUFF_SIZE, fp) != NULL)
      historyArrayCounter = atoi(c);

    int i = 0;
		while(fgets(c, BUFF_SIZE, fp) != NULL) {
      historyArray[i] = malloc(BUFF_SIZE*sizeof(char));
      strcpy(historyArray[i], c);
      i++;
	 	}
	 	fclose(fp);
	}
}


void saveHistoryToFile() {
	FILE *fp;
	int commandNumber = 1;
	// history save to .hist_list in home directory
	fp = fopen(".hist_list", "w+");

	if (fp == NULL) {
		perror("failed to open stream");

	} else {
		printf("FILE OPENED FOR READING\n");
    fprintf(fp, "%d\n%d\n", historyCounter, historyArrayCounter);
    if (historyCounter > historySize) {
      for (int i = 0; i<historySize; i++) {
        fprintf(fp, "%s", historyArray[i]);
      }
  		fclose(fp);
    } else {
      for (int i = 0; i < historyCounter; i++) {
        fprintf(fp, "%s", historyArray[i]);
      }
  		fclose(fp);
    }
	}
}

int alias_exists(char *target_alias){

  for(int i = 0; i< alias_count; i++){
    if(strcmp(aliases[i].alias, target_alias) == 0)
    return i;
  }

  return -1;
}

void print_alias(){

    int i = 0;


    if(alias_count == 0){
        printf("Error: There are no aliases available to use!\n");
    }

    while(i<alias_count){
        printf("%s -> %s\n", aliases[i].alias, aliases[i].command);
        i++;
    }
}

 void alias(int argc, char **argv){

// may need to add another argument to the alias function but unsure just now

    if (argc == 0){
        print_alias();
    }
    else{
    add_alias(argc, argv);
     }

 }


 void add_alias(int argc, char **argv){

   //creating a pointer to the location of the alias in the structure
     int pointer = alias_exists(argv[0]);


if (argv[1] == 0){
  printf("Error: Invalid alias. Second argument missing.\n");
  return;
}
else if(strcmp(argv[0], argv[1]) == 0){
  printf("Error: both commands are the same.\n");
  return;
}

else if(pointer>=0){
    printf("Error: You cannot add an alias to an existing alias \n");
  }

    else{

       if(alias_count>=10){
         printf("Error: Alias list is already full\n");
         return;
       }

      update_alias(alias_count, argv);
      alias_count++;

      }
  }



 void update_alias(int index, char **argv)
 {
   aliases[index].alias = strdup(argv[0]);
   int i=1;
   aliases[index].command = malloc(sizeof(char)*BUFF_SIZE);
   strcpy(aliases[index].command, "");

   while(argv[i] != 0){
     strcat(aliases[index].command, argv[i]);
     strcat(aliases[index].command, "");
     i++;
   }

}


void unalias(int argc, char **argv){
if(argc == 0){
  printf("Error: No alias selected.\n");
}
else{
  //if alias exists point = i, else = -1

        int pointer = alias_exists(argv[0]);

//remove alias from array
        if(pointer >= 0){
          for(int i = pointer+1; i<alias_count; i++){
            strcpy(aliases[i-1].alias, aliases[i].alias);
            strcpy(aliases[i-1].command, aliases[i].command);
          }

//decrement alias count
          alias_count--;
        }

      else{
        printf("Error: Alias does not exist.");
      }
    }

}

void printdir(int argc, char **argv){
	int i;
	if (argc == 0){
		count = printContents(directory, &files);
		for (i = 0; i < count; i++) {
			printf("%s\n", files[i]);
		}
	} else {
		printf("Erroer: No arguments required for this command to run!\n");
	}
}
