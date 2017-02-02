

//List of built in commands in the shell
//If the command is not here, it is either an external program or an error
char *commands;
//Pointers to those built in commands
int (*functions[]) (int argc, char **argv);

void promptUser();
char *readInput();
char **tokenizeInput(char* input);
void execute();


int main(int argc, char **argv){
  int status = 0;
  char *input;
  char **arguments;
  while (!status){
    promptUser();
    input = readInput();
    arguments = tokenizeInput(input);
    status = execute(arguments);
  }

  return status;
}
