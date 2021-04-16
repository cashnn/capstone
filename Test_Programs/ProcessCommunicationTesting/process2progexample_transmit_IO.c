#include <wiringPi.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h> //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h>
#include <string.h>
#include "drumSounds.h" // THIS IS THE DRUM SOUND HEADER FILE CONTAINING THE HEX VALUES OF EACH SOUND. THIS IS COMMON TO THE TRANSMIT CODE AND RECEIVE CODE.

#define  Button1Pin		6
#define  Button2Pin		5

_Bool pressed1 = FALSE;
_Bool pressed2 = FALSE;

int count1 = 0;
int count2 = 0;

int main(void)
{
        int             fd[2];
        pid_t           childpid;
        unsigned char  	transmitSound;

        if(wiringPiSetup() == -1){ /*when initialize wiring failed,print message to screen*/
	    printf("setup wiringPi failed !");
	    return 1;
	}
        
        pinMode(Button1Pin, INPUT);
	pinMode(Button2Pin, INPUT);

        pipe(fd);

        if((childpid = fork()) == -1) // THIS CREATES THE CHILD AND CHECKS FOR AN ERROR.
        {
                perror("fork");
                exit(1);
        }
        
        if(childpid == 0) // INSIDE THE CHILD PROCESS
        {
                // Child process closes up output side of pipe
                close(fd[1]);
                
                // Close stdin, duplicate the input side of pipe to stdin
                dup2(fd[0], STDIN_FILENO);

                // close(fd[0]);

                execl("./p2pe_r_IO", "./p2pe_r_IO", NULL); // THIS RUNS THE RECEIVE PROGRAM IN THE CHILD PROCESS.

                exit(0);
        }
        else // INSIDE THE PARENT PROCESS
        {
                //Parent process closes up input side of pipe
                close(fd[0]);
                
                while(1) {
                        
                        // DRUM STRIKE DETECTION CODE GOES HERE
                        
                        if (!digitalRead(Button1Pin)) {
                                pressed1 = TRUE;
                                delay(50);
                        } else if (!digitalRead(Button2Pin)) {
                                pressed2 = TRUE;
                                delay(50);
                        }
                        if ((digitalRead(Button1Pin)) && pressed1) {
                                transmitSound = SOUND1;
                                printf("%X\n",transmitSound);
                                write(fd[1], &transmitSound, sizeof(transmitSound)); // THIS TRANSMITS THE CORRESPONDING SOUND'S HEX VALUE TO THE RECEIVE PROGRAM (RUNNING IN THE CHILD PROCESS).
                                pressed1 = FALSE;
                                count1++;
        
                                if (count1 == 10) {
                                        transmitSound = SOUND0;
                                        write(fd[1], &transmitSound, sizeof(transmitSound));
                                        close(fd[1]);
                                        break;
                                }
                        } else if ((digitalRead(Button2Pin)) && pressed2) {
                                transmitSound = SOUND2;
                                printf("%X\n",transmitSound);
                                write(fd[1], &transmitSound, sizeof(transmitSound));
                                pressed2 = FALSE;
                                count2++;
        
                                if (count2 == 10) {
                                        transmitSound = SOUND0;
                                        write(fd[1], &transmitSound, sizeof(transmitSound));
                                        close(fd[1]);
                                        break;
                                }
                        }
                }
        }
        
        return(0);
}
