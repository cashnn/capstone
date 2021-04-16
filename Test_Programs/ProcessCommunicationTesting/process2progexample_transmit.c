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
        unsigned char  	transmitSound = SOUND13;

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
                
                /* Close stdin, duplicate the input side of pipe to stdin */
                dup2(fd[0], STDIN_FILENO);

                // close(fd[0]);

                execl("./p2pe_r", "./p2pe_r", NULL);

                exit(0);
        }
        else
        {
                int i = 0;

                /* Parent process closes up input side of pipe */
                close(fd[0]);

                /* Send "string" through the output side of pipe */
                write(fd[1], &transmitSound, sizeof(transmitSound));

                printf("Transmit drum sound (in transmit prog.): %X\n", transmitSound);
                
                close(fd[1]);

                // sleep(5);
                // printf("Done sleeping in transmit code.\n");
        }
        
        return(0);
}