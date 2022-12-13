#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[256] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;
char dollarsign[1] = "$";
int childpid;

void alarmHandler(int signum) {
  kill(childpid, SIGKILL);
}

void sigintHandler(int signum) {
  signal(SIGINT, sigintHandler);
  fflush(stdout);
}
void strip_first_letter(char *str) {
  int i;
  if(str == NULL || str[0] == '\0') {
    return;
  }
  for (i = 0; str[i] != '\0'; i++) {
    str[i] = str[i+1];
  }
}
bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}
int main() {
    bool isbg = true;
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
  
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];
    
    // signal(SIGINT, sigintHandler);
    signal(SIGALRM, alarmHandler);

    while (true) {
      
        do{ 
            int bufsize = 256;
            getcwd(prompt, bufsize);
            
            // Print the shell prompt.
            printf("%s>", prompt);
            fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
        
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }
 
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        // TODO:
        // 
        
			  // 0. Modify the prompt to print the current working directory
  
			
        // 1. Tokenize the command line input (split it on whitespace)
        arguments[0] = strtok(command_line, delimiters);
    
        int i = 0;
        while (arguments[i] != NULL) {
          i++;
          arguments[i] = strtok(NULL, delimiters);
        }

        if(strcmp(arguments[i-1], "&")== 0){
          isbg = true;
          arguments[i-1] = '\0';
        }
      
        // 2. Implement Built-In Commands
        if (strcmp(arguments[0], "exit") == 0) {
          exit(1);
        } else if (strcmp(arguments[0], "pwd") == 0) {
          printf("%s\n", getenv("PWD"));
        } else if (strcmp(arguments[0], "cd") == 0) {
          if (chdir(arguments[1]) != 0) {
            printf("change directory to %s failed", arguments[1]);
          } else {
            if (arguments[1] == NULL) {
              chdir(getenv("HOME"));
            } else {
              chdir(arguments[1]);
            }
          }
        } else if (strcmp(arguments[0], "env") == 0) {
          for (; *environ; environ++) {
            printf("%s\n", *environ);
          }
        } else if (strcmp(arguments[0], "setenv") == 0) {
          if (arguments[1] != NULL && arguments[2] != NULL) {
            setenv(arguments[1], arguments[2], 1);
          }
          else{
            printf("SETENV Error: no parameters specified\n");
          }
        }else if (strcmp(arguments[0], "echo") == 0) {
          int i = 1;
          while (arguments[i] != NULL) {
            char text[100];
            memcpy(text, arguments[i], sizeof(arguments[i]));
            if(prefix(dollarsign, text)){
              strip_first_letter(text);
              printf("%s\n", getenv(text));
            }
            else{
              printf("%s ", arguments[i]);
            }
            i++;
          }
          printf("\n");
        } else if (prefix(dollarsign, arguments[0])) {
          char envvar[100];
          memcpy(envvar, arguments[0], sizeof(arguments[0]));
          strip_first_letter(envvar);
          printf("%s\n", getenv(envvar));
        }
        // 3. Create a child process which will execute the command line input
        else {
          pid_t pid; 
          int status; 
          pid = fork();
          if(pid < 0)
          {
            printf("Error: Could not execute the requested process\n");
          }
          else if(pid==0)
          {
            if (execvp(arguments[0], arguments) == -1){
              perror("execvp() fail: ");
              printf("An error occured\n"); 
            }
            exit(0);
          }
          else
          {
            childpid = pid; 
            alarm(10);
            if(!isbg){
              wait(NULL);
            }
          }
        }
        // 4. The parent process should wait for the child to complete unless its a background process
      
      
        // Hints (put these into Google):
        // man fork
        // man execvp
        // man wait
        // man strtok
        // man environ
        // man signals
        
        // Extra Credit
        // man dup2
        // man open
        // man pipes
    }
    // This should never be reached.
    return -1;
}