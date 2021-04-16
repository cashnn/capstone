#include <wiringPi.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h> //Header file for sleep(). man 3 sleep for details. 
//#include <pthread.h>
#include <string.h>
#include "drumSounds.h" // THIS IS THE DRUM SOUND HEADER FILE CONTAINING THE HEX VALUES OF EACH SOUND. THIS IS COMMON TO THE TRANSMIT CODE AND RECEIVE CODE.

#define  Led1Pin		4
#define  Led2Pin		3
#define  Button1Pin		6
#define  Button2Pin		5

_Bool pressed1 = FALSE;
_Bool pressed2 = FALSE;

int count1 = 0;
int count2 = 0;


int main(void)
{
        if(wiringPiSetup() == -1){ /*when initialize wiring failed,print message to screen*/
	    printf("setup wiringPi failed !");
	    return 1;
	}
        
	pinMode(Button1Pin, INPUT);
	pinMode(Button2Pin, INPUT);
        
        int             fd[2];
        pid_t           childpid;
        unsigned char  	selectSound;
        
        pipe(fd);
        
        if((childpid = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }

        if(childpid == 0)
        {
                // Child process closes up output side of pipe
                close(fd[1]);
                
                int currentLed = 3;
                
                unsigned char  	receiveSound;
                
                int count = 0;
                
                pinMode(Led1Pin, OUTPUT);
                pinMode(Led2Pin, OUTPUT);
                
                digitalWrite(Led1Pin, LOW);
                digitalWrite(Led2Pin, LOW);
                
                while(1) {
                        
                        // READ CODE GOES HERE
                        
                        read(fd[0], &receiveSound, sizeof(receiveSound));
                        
                        if (receiveSound == SOUND1) {
                                digitalWrite(currentLed, LOW);
                                if (currentLed != Led1Pin) {
                                        count++;
                                }
                                currentLed = Led1Pin;
                                digitalWrite(currentLed, HIGH);
                        } else if (receiveSound == SOUND2) {
                                digitalWrite(currentLed, LOW);
                                if (currentLed != Led2Pin) {
                                        count++;
                                }
                                currentLed = Led2Pin;
                                digitalWrite(currentLed, HIGH);
                        } else if (receiveSound == SOUND0) {
                                digitalWrite(Led1Pin, LOW);
                                digitalWrite(Led2Pin, LOW);
                                break;
                        }
                }
                
                printf("EXITING CHILD PROCESS\n");
                close(fd[0]);
                exit(0);
        }
        else
        {
                // Parent process closes up input side of pipe 
                close(fd[0]);
                
                while(1) {
                        
                        // WRITE CODE GOES HERE
                        
                        if (!digitalRead(Button1Pin)) {
                                pressed1 = TRUE;
                                delay(50);
                        } else if (!digitalRead(Button2Pin)) {
                                pressed2 = TRUE;
                                delay(50);
                        }
                        if ((digitalRead(Button1Pin)) && pressed1) {
                                selectSound = SOUND1;
                                printf("%X\n",selectSound);
                                write(fd[1], &selectSound, sizeof(selectSound));
                                pressed1 = FALSE;
                                count1++;
        
                                if (count1 == 10) {
                                        selectSound = SOUND0;
                                        write(fd[1], &selectSound, sizeof(selectSound));
                                        close(fd[1]);
                                        break;
                                }
                        } else if ((digitalRead(Button2Pin)) && pressed2) {
                                selectSound = SOUND2;
                                printf("%X\n",selectSound);
                                write(fd[1], &selectSound, sizeof(selectSound));
                                pressed2 = FALSE;
                                count2++;
        
                                if (count2 == 10) {
                                        selectSound = SOUND0;
                                        write(fd[1], &selectSound, sizeof(selectSound));
                                        close(fd[1]);
                                        break;
                                }
                        }
                }
        }
        
        
        return(0);
}
