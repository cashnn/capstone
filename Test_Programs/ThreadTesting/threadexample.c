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

// A normal C function that is executed as a thread 
// when its name is specified in pthread_create()
void *myThreadFun1()
{ 
	while(1) {
		if (!digitalRead(Button1Pin)) {
			pressed1 = TRUE;
			delay(50);
		}
		if ((digitalRead(Button1Pin)) && pressed1) {
			digitalWrite(Led1Pin, HIGH);  /*led on*/
		    printf("led on...\n");
		    delay(50);
		    digitalWrite(Led1Pin, LOW);  /*led off*/
		    printf("...led off\n");
		    delay(50);
		    pressed1 = FALSE;
		    count1++;

		    if (count1 == 20) {
		    	break;
		    }
		}
	}
	// sleep(1); 
	// // printf("Printing from Thread 1\n"); 
	// printf("1.1\n"); 
	// printf("1.2\n"); 
	// sleep(1); 
	// printf("1.3\n"); 
	// printf("1.4\n"); 
	// printf("1.5\n"); 
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
			digitalWrite(Led2Pin, HIGH);  /*led on*/
		    printf("led on...\n");
		    delay(50);
		    digitalWrite(Led2Pin, LOW);  /*led off*/
		    printf("...led off\n");
		    delay(50);
		    pressed2 = FALSE;
		    count2++;

		    if (count2 == 20) {
		    	break;
		    }
		}
	}
	// sleep(1); 
	// // printf("Printing from Thread 2\n"); 
	// printf("2.1\n"); 
	// printf("2.2\n"); 
	// printf("2.3\n"); 
	// printf("2.4\n"); 
	// sleep(1); 
	// printf("2.5\n");
	return NULL; 
}

// void *myThreadFun3() 
// { 
// 	sleep(1); 
// 	// printf("Printing from Thread 3\n"); 
// 	printf("2.1\n"); 
// 	printf("2.2\n"); 
// 	printf("2.3\n"); 
// 	printf("2.4\n"); 
// 	sleep(1); 
// 	printf("2.5\n");
// 	return NULL; 
// }

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
	// int rc3;
	pthread_t thread_id1, thread_id2;
	// pthread_t thread_id3; 
	printf("Before Thread\n"); 
	if ((rc1=pthread_create(&thread_id1, NULL, &myThreadFun1, NULL))) {
		printf("Thread creation failed: %d\n", rc1);
	}
	if ((rc2=pthread_create(&thread_id2, NULL, &myThreadFun2, NULL))) {
		printf("Thread creation failed: %d\n", rc2);
	}
	// if ((rc3=pthread_create(&thread_id3, NULL, &myThreadFun3, NULL))) {
	// 	printf("Thread creation failed: %d\n", rc3);
	// }
	pthread_join(thread_id1, NULL); 
	pthread_join(thread_id2, NULL); 
	// pthread_join(thread_id3, NULL); 
	printf("After Thread\n"); 
	exit(0); 
}
