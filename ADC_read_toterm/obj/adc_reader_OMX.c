#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <time.h>
#include "ADS1256.h"	//adc header
#include "stdio.h"
#include <string.h>
#include <sys/timeb.h>
#include <sys/types.h> 
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.
#include <bcm2835.h>	//spi library
#include <pthread.h>    //threading library
//#include <wiringPi.h>

//Global Variables
double THRESHOLD = 0.2;
double peak1_amp = 0;
int peak1_time;
int peak2_time = 0;
clock_t begin;

double take_samp(void);
double local_peak(int);

//seperate thread for output
void *outputthread(void *vargp)
{
    printf("\nHIT! ");
    fflush(stdout);
    return NULL;
}

//Interrput Handling Sburoutine
void Handler(int signo)
{
    //System Exit
    clock_t end = clock();	//end time of scan
    double time_spent = (double)(end - begin)/CLOCKS_PER_SEC;	//computing elapsed adc run time
    printf("\nRun Time: %lf\n", time_spent);	//output run time for manual sample freq computations
    //printf("\r\nEND                  \r\n");
    DEV_ModuleExit();

    exit(0);
}

int main(void)
{
    DEV_ModuleInit();
    //reader variable declarations
    int samp_clock = 0;         //sample counter
    double sample;              //temp variable for current sample value
    double peak;                //temp variable for storing local peaks
    int stroke_flag = 0;    //flag for delaying next hit register
    int period_flag = 0;    //flag for delaying a period before new local max
    
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);
    //initialization of the adc board
    if(ADS1256_init() == 1){
        printf("\r\nEND                  \r\n");
        DEV_ModuleExit();
        exit(0);
    }
    //setting up thread
    pthread_t thread_id;
    //pthread_create(&thread_id, NULL, outputthread, NULL); <open new thread
    //pthread_join(thread_id, NULL);    <wait for thread to finish

    begin = clock();	//start of total scan time

    //continuous run loop
    while(1)
    {
        sample = take_samp();
        //interpretting read results
        samp_clock++;
        peak1_amp = peak;
        //if threshold is passed, hit is possible and peak is found
        if(sample > THRESHOLD && period_flag == 0)
        {
            period_flag = 1;    //setting flag to wait for next signal period
            peak = local_peak(samp_clock);
            //printf("%lf, ", peak);
            if(stroke_flag > 0)
            {
                stroke_flag--;
                printf("%d, ", peak2_time);
                fflush(stdout);
            }
        }

        if(peak > (peak1_amp+0.12) && stroke_flag == 0)
        {
            //beginning output thread
            int child1;
            //int childPID = -1;
            child1 = fork();
            if(child1 == 0) {
                execlp("/usr/bin/omxplayer", "omxplayer", "-d", "-o", "local", "/home/pi/Github/testbranch2/ADC_read_toterm/TEST_A.wav",NULL); //Output through headphones
                _exit(0);
            } 
            else 
            {
                printf("Done\n");
            }
            
            pthread_create(&thread_id, NULL, outputthread, NULL);
            pthread_join(thread_id, NULL);  //wait for output thread to finish
            fprintf(stdout, "of %lf at %d next peaks: ", peak, samp_clock);
	        fflush(stdout);    //forcing output to terminal
            stroke_flag = 2;
        }

        //if period flag is active, wait until signal hits zero before finding next peak
        if(period_flag == 1)
        {
            while(sample > 0)
            {
                sample = take_samp();
                samp_clock++;
            }
        period_flag = 0;    //resetting period after signal dips below 0
        }
        
    }
    return 0;
}


double take_samp()
{
    UDOUBLE read = 0;
    double sample;    //voltage sample variable
    UBYTE buf[3];   //register storage location
    UDOUBLE i;
    bcm2835_gpio_write(22 ,0);      //cs pin set to write
    bcm2835_spi_transfer(0x51);
    bcm2835_spi_transfer(0x00);
    bcm2835_spi_transfer(0x38);
    bcm2835_spi_transfer(0xFC);
    bcm2835_spi_transfer(0x00);
    bcm2835_gpio_write(22, 1);
    buf[0] = 0;     //clear register storage location
    buf[1] = 0;     //clear register storage location
    //wait for comminication from chip
    i = 0;
        for(i=0;i<4000000;i++){
            //if(DEV_Digital_Read(DEV_DRDY_PIN) == 0)
            if(bcm2835_gpio_lev(17) == 0)
                break;
        }
        if(i >= 4000000){
            printf("Time Out ...\r\n");
        }

    bcm2835_delay(0.04);        //delay
    bcm2835_gpio_write(22, 0);  //cs pin to write
    bcm2835_spi_transfer(0x01); 
    bcm2835_delay(0.04);        //delay
    buf[0] = bcm2835_spi_transfer(0xff);    //transfer register over spi
    buf[1] = bcm2835_spi_transfer(0xff);    //transfer register over spi
    bcm2835_gpio_write(22, 1);  //unenable cs pin
    //decoding retrieved register values
    read = ((UDOUBLE)buf[0] << 16) & 0x00FF0000;    
    read |= ((UDOUBLE)buf[1] << 8);
    if (read & 0x800000){read &= 0xFF000000;}
    sample = read*5.0/0x7fffff;
    return(sample);
}


double local_peak(int samp_clock)
{
    peak1_time = peak2_time;
    double max;     //return value of local maximum
    double temp;    //temp variable for finding max
    int peak_flag = 0;  //flag defining when max has been found
    max = take_samp();
    samp_clock++;
    //continue searching for local peak until 2 sequential decreases are found
    while(peak_flag != 2)
    {
        temp = take_samp();    //retrieve next data point
        samp_clock++;
        //check if value still increasing
        if(temp > max)
        {
            max = temp;
            peak2_time = samp_clock; 
            peak_flag = 0;
        }
        else
        {
            //increment flag if sequential samples decrease
            peak_flag++;
        }
    }
    return(max);
}
