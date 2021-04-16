// #include <wiringPi.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h> //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h>
#include <string.h>
#include "drumSounds.h"


int main()
{
		unsigned char receiveSound;

		// sleep(1);
		// printf("Done sleeping in receive code.\n");

		/* Read in a byte from the pipe */
		read(STDIN_FILENO, &receiveSound, sizeof(receiveSound));
        
        printf("Received drum sound (in receive prog.): %X\n", receiveSound);

        // if (receiveSound == 0x05) {
        // 	printf("Play sound #%d\n", SOUND5);
        // } else if (receiveSound == 0x0E) {
        // 	printf("Play sound #%d\n", SOUND14);
        // }

        return(0);
}
        