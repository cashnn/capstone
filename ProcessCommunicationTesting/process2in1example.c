// #include <wiringPi.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h> //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h>
#include <string.h>
#include "drumSounds.h"


int main(void)
{
        int             fd[2], nbytes;
        pid_t           childpid;
        unsigned char  	selectSound = SOUND11;

        pipe(fd);
        
        if((childpid = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }

        if(childpid == 0)
        {
                /* Child process closes up output side of pipe */
                close(fd[1]);

                /* Read in a string from the pipe */
                nbytes = read(fd[0], &selectSound, sizeof(selectSound));
                printf("Received drum sound (in Child): %X\n", selectSound);
                
                exit(0);
        }
        else
        {
                /* Parent process closes up input side of pipe */
                close(fd[0]);

                /* Send "string" through the output side of pipe */
                write(fd[1], &selectSound, sizeof(selectSound));
        }
        
        return(0);
}