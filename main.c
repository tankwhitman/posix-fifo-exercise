/*
Joshua Whitman
JSW625
CLANG
Exerise 8
implimentation of FIFOs
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define BUF_SIZE 1024
//standard my print
void myPrint(const char *str)
{
    if (write(STDOUT_FILENO, str, strlen(str)) == -1)
    {
        perror("write");
        exit(EXIT_FAILURE);
    }
}
int myPrintInt(const int val)
{
  char buf[BUF_SIZE];
  sprintf(buf, "%d", val);
  if((write(STDOUT_FILENO, buf, strlen(buf))) == -1)
  {
    return 0;
  }
  return 1;
}
//dup string to turn array of chars into char*
char * dupString(const char *val)
{
    int len = strlen(val);
    char * outStr = (char*) calloc(len+1, sizeof(char));
    if(outStr == NULL)
    {
        perror("Calloc");
        exit(EXIT_FAILURE);
    }
    strcpy(outStr, val);
    return outStr;
}

void signalHandlerC(int sig)
{   
    if(sig == SIGCONT)
    {
        return;
        
    }
}





int main(int argc, char *argv[])
{
    //create FIFO
    mkfifo("ourFIFO", S_IRWXU);

    struct sigaction saC;
    int fd;
   
    char buf[BUF_SIZE];

    //fork
    pid_t status = fork();
    switch(status)
    {
        //do in chld
        //will read fd[0]
        case 0:
            //signal handler stuff
            saC.sa_handler = signalHandlerC;
            sigemptyset(&saC.sa_mask);
            saC.sa_flags = 0;
            //open fd
            fd = open("ourFIFO", O_RDONLY);
          
            if(sigaction(SIGCONT, &saC, NULL )== -1)
            {
                perror("sigaction");
                exit(EXIT_FAILURE);
            }
            //wait
            pause();    
                
                
            int numRead = 1;
            char childbuf;
            myPrint("Child: ");
            //read one byte and print at a time
            while(read(fd, &childbuf, 1) >0)
            {
                write(STDOUT_FILENO, &childbuf, 1);
            }
                
            
            close(fd);
            exit(EXIT_SUCCESS);


        //do in parent
        //will write to fd[1]
        default:
            fd = open("ourFIFO", O_WRONLY);

            
            myPrint("Please enter text at the parent: \n");
            //read in the input
            read(STDIN_FILENO, &buf, BUF_SIZE);
            char * msg = dupString(buf);
            //write into fifo
            write(fd, msg, strlen(msg));
            //tell child to read that stuff
            kill(status, SIGCONT);
                
           
            close(fd);
            wait(&status);
            exit(EXIT_SUCCESS);

    }

    



    return 0;
}