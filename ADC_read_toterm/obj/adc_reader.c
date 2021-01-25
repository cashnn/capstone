#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <time.h>
#include "ADS1256.h"	//adc header
#include "stdio.h"
#include <time.h>
#include <string.h>
#include <sys/timeb.h>
#include <bcm2835.h>	//spi library

double THRESHOLD = 0.1;
clock_t begin;

double take_samp(void);
double local_peak(int);

void  Handler(int signo)
{
    //System Exit
    clock_t end = clock();	//end time of scan
    double time_spent = (double)(end - begin)/CLOCKS_PER_SEC;	//computing elapsed adc run time
    printf("\nRun Time: %lf\n", time_spent);	//output run time for manual sample freq computations
    printf("\r\nEND                  \r\n");
    DEV_ModuleExit();

    exit(0);
}

int main(void)
{
    DEV_ModuleInit();
    //reader variable declarations
    double hit_peak = 0;
    int samp_clock = 0;         //sample counter
    double sample;              //temp variable for current sample value
    double peak;                //temp variable for storing local peaks
    double last_peak = 0;   //comparison to last peak
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

    begin = clock();	//start of total scan time

    //continuous run loop
    while(1)
    {
        sample = take_samp();
        //interpretting read results
        samp_clock++;
        //if threshold is passed, hit is possible and peak is found
        last_peak = peak;
        if(sample > THRESHOLD && period_flag == 0)
        {
            period_flag = 1;    //setting flag to wait for next signal period
            peak = local_peak(samp_clock);
            //printf("%lf, ", peak);
            if(stroke_flag > 0){stroke_flag--;}
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
        if(peak > last_peak && stroke_flag == 0)
        {
            printf("hit! at %d\n", samp_clock);
            hit_peak = peak;
            stroke_flag = 3;
        }
        //fprintf(fptr, "%f\n", temp);	

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