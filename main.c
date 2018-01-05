#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#define  max_word 10
#define  max_char 100

int input_redirection_flag;
int output_redirection_flag;
int piping_flag;
int background_flag;
char* input_file=NULL;
char* output_file=NULL;

int parse_line(char* temp[],char line[]){
int i=0;
temp[i]=strtok(line," ");
if(temp[i]== NULL){
return 1;
}
while(temp[i]!= NULL){
i++;
temp[i]=strtok(NULL," ");
}
//if( strcmp(temp[0],"exit")==0 ){
 //exit(0);
 }

//return 1;


void remove_endof_line(char line[]){
int i=0;
while(line[i] != '\n'){
 i++;
}
line[i]='\0';
}

void read_line(char line[]){
 printf("SISH>");
 char* ret=fgets(line,max_char,stdin);
 remove_endof_line(line);

}
int pipe_redirection_check(char* temp[]){
int i=0;

while(temp[i]!= NULL){
if(strcmp(temp[i],">")==0){
output_redirection_flag=1;
output_file=temp[i+1];
return i;
}
if(strcmp(temp[i],"<")==0){
input_redirection_flag=1;
input_file=temp[i+1];
return i;
}
if(strcmp(temp[i],"|")==0){
piping_flag=1;
return i;
}
if(strcmp(temp[i],"&")==0){
background_flag=1;

return i;
}
i++;
}
return i;
}
void check_line(char* temp[]){
int i=0;
int pipe_count=0;
int output_redirection_count=0;
int input_redirection_count=0;
if(temp[i]== NULL){
printf("no command \n" );
return 1;
}
while(temp[i]!= NULL){
if(strcmp(temp[i],">")==0){
output_redirection_count++;
}
if(strcmp(temp[i],"<")==0){
input_redirection_count++;
}
if(strcmp(temp[i],"|")==0){
pipe_count++;
}
i++;
}
if(output_redirection_count+input_redirection_count+pipe_count >1){
printf("ERROR");
temp[0]==NULL;
}
}

int read_parse_line(char* arg[],char line[],char* pipe_arg[]){
  char* temp[max_word];
  int pos;
  int i=0;
  read_line(line);
  parse_line(temp,line);
  check_line(temp);
  if( strcmp(temp[0],"exit")==0 ){
   exit(0);
 }
  pos=pipe_redirection_check(temp);
  while(i<pos){
  arg[i]=temp[i];
  i++;
  }
  arg[i]=NULL;
  i++;
  if(piping_flag==1){
  int j=0;
  while(temp[i]!=NULL){
  pipe_arg[j]=temp[i];
  i++;
  j++;
  }

  }
  return 1;

}
void pipe_handle(char* arg[],char* pipe_arg[],int pipefd[]){
int  pid;
int i;
pid=fork();
if(pid==0){
  dup2(pipefd[1],1);
  close(pipefd[0]);

  execvp(arg[0],arg);
  perror(arg[0]);
}
else{
  dup2(pipefd[0],0);
  close(pipefd[1]);
  execvp(pipe_arg[0],pipe_arg);
  perror(pipe_arg[0]);
}
}

int main()
{   char* arg[max_word];
    char line[max_char];
    char* pipe_arg[max_word];

    int pipefd[2];
    pipe(pipefd);


   while(read_parse_line(arg,line,pipe_arg)){


    int status;
    pid_t pid=fork();


    if(pid ==0){
      if(input_redirection_flag==1 && input_file != NULL){
        dup2(open(input_file,O_RDWR|O_CREAT,0777),0);
      }
      if(output_redirection_flag==1 && output_file != NULL){
        dup2(open(output_file,O_RDWR|O_CREAT, 0777),1);
      }
      if(piping_flag==1){
      pipe_handle(arg,pipe_arg,pipefd);
      exit(0);

      }
      if(background_flag ==1){

         execvp(arg[0],arg);


         exit(0);
      }

      execvp(arg[0],arg);
      input_redirection_flag=0;
      output_redirection_flag=0;
      piping_flag=0;
      background_flag=0;
      input_file=NULL;
      output_file=NULL;
      }
else{



     if(background_flag==1){
     printf("%d\n ",pid);
     }
     if(!background_flag){
     waitpid( pid ,&status, WUNTRACED | WCONTINUED);

     }


      input_redirection_flag=0;
      output_redirection_flag=0;
      piping_flag=0;
      background_flag=0;
      input_file=NULL;

      output_file=NULL;

}
}
 return 0;
}
