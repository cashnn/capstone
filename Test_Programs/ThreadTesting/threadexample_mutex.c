#include <wiringPi.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h> //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h>

#define  Led1Pin		3
#define  Led2Pin		4
#define  Button1Pin		5
#define  Button2Pin		6

void *myThreadFun1();
void *myThreadFun2();
// void *myThreadFun3();

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int counter = 0;

_Bool pressed1 = FALSE;
int count1 = 0;
_Bool pressed2 = FALSE;
int count2 = 0;

int currentLed = Led1Pin;

// A normal C function that is executed as a thread 
// when its name is specified in pthread_create()
void *myThreadFun3()
{ 
	while(1) {
		digitalWrite(currentLed, HIGH);  /*led on*/
	    //printf("led on...\n");
	    delay(50);
	    digitalWrite(currentLed, LOW);  /*led off*/
	    //printf("...led off\n");
	    delay(50);
	    
	    if (count1 + count2 == 40) {
			break;
		}
	}
	//system("killall omxplayer.bin");
	//if(pid==0) {
		//system("killall omxplayer.bin");
		//exit(0);
	//}
	return NULL; 
} 

void *musicThread() {
	//execlp("/usr/bin/omxplayer", " ", "/home/pi/Documents/ELG4913/ThreadTesting/Wed_Nov_9_2016_0030.m4a", NULL);	//Execute file: file, arguments (1 or more strings followed by NULL)
	//_exit(0);
	//int pid;
	//pid=fork();
	//if(pid==0) {
		//printf("I am the child\n");
		//execlp("/usr/bin/omxplayer", " ", "/home/pi/Documents/ELG4913/ThreadTesting/Wed_Nov_9_2016_0030.m4a", NULL);	//Execute file: file, arguments (1 or more strings followed by NULL)
		//_exit(0);
	//} else {
		//printf("I am the parent\n");
	//}
	
	return NULL; 
}

void *myThreadFun1()
{ 
	while(1) {
		if (!digitalRead(Button1Pin)) {
			pressed1 = TRUE;
			delay(50);
		}
		if ((digitalRead(Button1Pin)) && pressed1) {
			pthread_mutex_lock(&mutex1);
			digitalWrite(currentLed, LOW);
			currentLed = Led1Pin;
			pthread_mutex_unlock(&mutex1);
		    pressed1 = FALSE;
		    count1++;

		    if (count1 == 20) {
		    	break;
		    }
		}
	}
	
	return NULL; 
}

void *myThreadFun2() 
{ 
	while(1) {
		if (!digitalRead(Button2Pin)) {
			pressed2 = TRUE;
			delay(50);
		}
		if ((digitalRead(Button2Pin)) && pressed2) {
			pthread_mutex_lock(&mutex1);
			digitalWrite(currentLed, LOW);
			currentLed = Led2Pin;
			pthread_mutex_unlock(&mutex1);
		    pressed2 = FALSE;
		    count2++;

		    if (count2 == 20) {
		    	break;
		    }
		}
	}
 	
	return NULL; 
}

int main() 
{ 
	if(wiringPiSetup() == -1){ /*when initialize wiring failed,print messageto screen*/
	    printf("setup wiringPi failed !");
	    return 1;
	}
	printf("linker Led1Pin : GPIO %d(wiringPi pin)\n",Led1Pin); /*when initialize wiring successfully,print message to screen*/
	printf("linker Led2Pin : GPIO %d(wiringPi pin)\n",Led2Pin); /*when initialize wiring successfully,print message to screen*/
	printf("linker Button1Pin : GPIO %d(wiringPi pin)\n",Button1Pin); /*when initialize wiring successfully,print message to screen*/
	printf("linker Button2Pin : GPIO %d(wiringPi pin)\n",Button2Pin); /*when initialize wiring successfully,print message to screen*/

	pinMode(Led1Pin, OUTPUT);
	pinMode(Led2Pin, OUTPUT);
	pinMode(Button1Pin, INPUT);
	pinMode(Button2Pin, INPUT);

	int rc1, rc2;
	int rc3;
	int rc4;
	pthread_t thread_id1, thread_id2;
	pthread_t thread_id3;
	pthread_t thread_id4; 
	printf("Before Thread\n"); 
	if ((rc1=pthread_create(&thread_id1, NULL, &myThreadFun1, NULL))) {
		printf("Thread creation failed: %d\n", rc1);
	}
	if ((rc2=pthread_create(&thread_id2, NULL, &myThreadFun2, NULL))) {
		printf("Thread creation failed: %d\n", rc2);
	}
	if ((rc3=pthread_create(&thread_id3, NULL, &myThreadFun3, NULL))) {
		printf("Thread creation failed: %d\n", rc3);
	}
	if ((rc4=pthread_create(&thread_id4, NULL, &musicThread, NULL))) {
		printf("Thread creation failed: %d\n", rc4);
	}
	
	int pid;
	//int childPID = -1;
	pid=fork();
	if(pid==0) {
		printf("I am the child\n");
		//childPID = (int)getpid();
		execlp("/usr/bin/omxplayer", " ", "/home/pi/Documents/ELG4913/ThreadTesting/Sound.wav", NULL);	//Execute file: file, arguments (1 or more strings followed by NULL)
		_exit(0);
	} else {
		printf("I am the parent\n");
		pthread_join(thread_id1, NULL); 
		pthread_join(thread_id2, NULL); 
		pthread_join(thread_id3, NULL); 
		pthread_join(thread_id4, NULL);
		
		//system("kill -9 omxplayer.bin");
	}
	//pthread_join(thread_id1, NULL); 
	//pthread_join(thread_id2, NULL); 
	//pthread_join(thread_id3, NULL); 
	//pthread_join(thread_id4, NULL);
	printf("After Thread\n"); 
	//system("killall omxplayer.bin");
	
	//char buf[13];
	//snprintf(buf, 13, "kill -9 %d", childPID); // puts string into buffer
	//printf("%s\n", buf); // outputs so you can see it
	//system(buf);
	//printf("After Kill\n"); 
	
	exit(0); 
}
