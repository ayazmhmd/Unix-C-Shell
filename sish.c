#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>
#define BUFFER_LEN 512
#define MAX_HIS_SIZE 100 //maximum history count
int last = 0;
char* history[MAX_HIS_SIZE]; //history 
void newCmd(char *cmd) { //adding command to the list of history
     history[last] = malloc(strlen(cmd)+1);
    strcpy(history[last],cmd);
     last=(last+1)%MAX_HIS_SIZE;

 }

void list_all_History(){  //showing all the history (command-> history)

     for(int i=0;i<=last-1;i++)
     printf("%i %s\n",i,history[i]);
}
void list_n_History(int value){ //for n no of history value (command-> history 5)
     if(last<value){ //if max history is less the the argument enter by user print all the history
  	list_all_History(); 
     }
     int j=0;
     for(int i=last-value;i<=last-1;i++)
     printf("%d %s\n",i,history[i]);
     j=j+1;
}
void pipecommand(char * file_path, char * arg[100],int argc){ 
//get command of the system path for the first command, all the pipe command, no of count of pipe command
   	char *Token;
        char* argv1[20];
        char* argv2[20];
        int argc1=0;                  
        Token = strtok(arg[0]," ");
        if(Token != NULL){
        while(Token!=NULL){ 
        //get all the command of the pipe first command and save in argv1
            argv1[argc1]=Token;      
            Token = strtok(NULL," ");
            argc1++;
        }}
      argv1[argc1]=NULL;
        Token = strtok(arg[1]," ");
        argc1=0;
        if(Token != NULL){
        while(Token!=NULL){ 
        //get all the command of the pipe second command and save in argv2
            argv2[argc1]=Token;      
            Token = strtok(NULL," ");
            argc1++;
        }}
      argv2[argc1]=NULL;
const int READ = 0, WRITE = 1;
int pipefd[2]; //defining pipe file descripter
if (pipe(pipefd) == -1) { //creating pipe 1
printf("Error creating pipe\n");
}
else{
int pid2 = -1, pid1 = -1;
pid1 = fork(); //creating a child process
if (pid1 == 0) {
close (pipefd[READ]); //close the read descriptor
dup2(pipefd[WRITE], WRITE); 
if (execvp(file_path,argv1) == -1) { //running the command using exec system call
perror("Child proccess end"); 
}
exit(EXIT_FAILURE);
}
else {
pid2 = fork(); //creating another child process
if (pid2 == 0) {
close(pipefd[WRITE]); //close the write descriptor
dup2(pipefd[READ], READ); 
if (execvp("wc",argv2) == -1) { //running another command using exec for the second child
perror("Child proccess end"); 
}
exit(EXIT_FAILURE);
}
}
waitpid(pid1,0, 0); //parent wait for child 1 to terminate

close(pipefd[WRITE]); //closing the write descriptor
waitpid(pid2,0, 0); //waiting for the second child process to terminate
close(pipefd[READ]); //closing the write descriptor
}}

int main(){

    char user_input[BUFFER_LEN];  //get command line
    char* argv[120]; //user command
    char* path= "/bin/";  //set path at bin  
    char command[BUFFER_LEN];
    char file_path[50];//full file path

    while(1){  //the loop will continuously run for my shell
        
        printf("sish> ");  
        // Greeting shell during startup               
 
        if(!fgets(user_input,BUFFER_LEN, stdin)){
            break;  
            //break if the command length exceed the defined BUFFER_LEN
        }
        size_t length = strlen(user_input);
        if(length == 0){ 
        //if the user Just enter so no command input it will break
            break;
        }

        if (user_input[length - 1] == '\n'){ 
            user_input[length - 1] = '\0'; 
            // replace last char by '\0' if it is new line char
        }
           strcpy(command,user_input);
        //split command using spaces
        char *token;                  
        token = strtok(user_input," ");
        int argc=0; //argument count
   
        if(token == NULL){
            continue;
        }
        while(token!=NULL){ 
        //get all the command the user enter and save in argv
            argv[argc]=token;      
            token = strtok(NULL," ");
            argc++;
        }
        
        argv[argc]=NULL; //last value of the argv should be null
        
        strcpy(file_path, path);  //Assign path to file_path 
        strcat(file_path, argv[0]); //conctanate command and file path        
        newCmd(command); //adding command to history
        if (access(file_path,F_OK)==0){  
        //check the command is available in /bin
	pid_t pid;
	int status;  
        char *pipetoken;    //token for command of pipe               
        pipetoken = strtok(command,"|"); //delimeter is | 
        int argp=0; //argument count for pipe 
        char* argvp[120]; //commands for pipe
  	while(pipetoken!=NULL){ 
  		//get all the command the user enter of delimeter | (pipe) and save in argvp 
  	     argvp[argp]=pipetoken;     
  	     pipetoken = strtok(NULL,"|");
  	     argp++;
  	}
  	if (argp>1){ // if the user enter the command of pipe like ls | wc
  	pipecommand(file_path,argvp,argp); //function call
  	}
  	else{ //if simple single command like pwd, ls
	pid = fork(); //creating a child process
        	if (pid == 0) { //child process 
        	    
                	if (execvp(file_path,argv) == -1) { //running the command using exec system call
                  		perror("Child proccess end"); 
                	}
                	exit(EXIT_FAILURE);
            } 
            else if (pid > 0) { // parent process
            //parent process will wait for their child process
                waitpid(pid, &status, WUNTRACED); 
                while (!WIFEXITED(status) && !WIFSIGNALED(status)){
                    waitpid(pid, &status, WUNTRACED); 
                }
            } 
            else {
                perror("Fork Failed"); //process id can not be null 
            }

        }}
        else {  //for the user defined command, Command is not available in the bin

        	char str[50];
        	if (!strcmp(argv[0], "cd")){ //if cd command enter
        	 char* gdir = getcwd(str, sizeof(str)); //get current working directory
  		 char* dir = strcat(gdir, "/");
  		 char* to = strcat(dir, argv[1]);
  		 if(chdir(to)==-1){ //if no file or directory exist
  		  printf("bash: %s: %s: No such file or directory\n",argv[0],argv[1]); 
  		 }
  		 else{ //move to the change directory
  		 to=getcwd(str, sizeof(str));

        	}}
        	 else if (!strcmp(argv[0], "history")){ //if user enter history command
      		if(argc!=1){ //if other argument enter with history
      		  if(!strcmp(argv[1], "-c")){  //for history -c
      		  last=0;
      		  memset(history, 0,  sizeof(history)); //free the history memory
      		  }
      		  else{ //for history -n
      		  list_n_History(atoi(argv[1])); //function to print n previous history
      		  }
      		  
        	 }
        	 else{ //if no other argument enter with history
   			list_all_History(); //print all the history of command
        	}
    		}
    		else if (!strcmp(argv[0], "exit")){
    		             printf("Bye\n");
    		          memset(history, 0,  sizeof(history)); //free the history memory
                             exit(0);
    		}
    	     
} }}

