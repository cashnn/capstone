#include <wiringPi.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h> //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h>
#include <string.h>
#include "drumSounds.h" // THIS IS THE DRUM SOUND HEADER FILE CONTAINING THE HEX VALUES OF EACH SOUND. THIS IS COMMON TO THE TRANSMIT CODE AND RECEIVE CODE.

#define  Led1Pin		4
#define  Led2Pin		3

int main()
{
	int currentLed = Led1Pin;
                
	unsigned char  	receiveSound;
	
	int count = 0;
	
	if(wiringPiSetup() == -1){ /*when initialize wiring failed,print message to screen*/
	    printf("setup wiringPi failed !");
	    return 1;
	}
	
	pinMode(Led1Pin, OUTPUT);
	pinMode(Led2Pin, OUTPUT);
	
	digitalWrite(Led1Pin, LOW);
	digitalWrite(Led2Pin, LOW);
	
	while(1) {
		
		// RECEIVE CODE GOES HERE
		
		// (add a busy wait here)
		
		read(STDIN_FILENO, &receiveSound, sizeof(receiveSound)); // THIS RECEIVES THE DRUM SOUND'S HEX VALUE TRANSMITTED BY THE TRANSMIT CODE (RUNNING IN THE PARENT PROCESS).
		
		// AUDIO OUTPUT CODE GOES HERE 
		
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

        return(0);
}
        
