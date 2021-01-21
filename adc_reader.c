#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <time.h>
#include "ADS1256.h"	//adc header
#include "stdio.h"
#include <time.h>
#include <string.h>
#include <sys/timeb.h>
#include <bcm2835.h>	//spi library

clock_t begin;

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
    UDOUBLE read = 0;
    double temp;	//voltage sample variable
    UBYTE buf[3];	//register storage location
    UDOUBLE i;
    printf("demo\r\n");
    DEV_ModuleInit();
    
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);
    //initialization of the adc board
    if(ADS1256_init() == 1){
        printf("\r\nEND                  \r\n");
        DEV_ModuleExit();
        exit(0);
    }

    FILE *fptr;
    fptr = fopen("filt.csv", "w");	//log file open
    fprintf(fptr, "Time,   Value\n");
    begin = clock();	//start of total scan time

    while(1){
         
    bcm2835_gpio_write(22 ,0);		//cs pin set to write
    bcm2835_spi_transfer(0x51);
    bcm2835_spi_transfer(0x00);
    bcm2835_spi_transfer(0x38);
    bcm2835_spi_transfer(0xFC);
    bcm2835_spi_transfer(0x00);
    bcm2835_gpio_write(22, 1);
    buf[0] = 0;		//clear register storage location
    buf[1] = 0;		//clear register storage location
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

    bcm2835_delay(0.04);		//delay
    bcm2835_gpio_write(22, 0);	//cs pin to write
    bcm2835_spi_transfer(0x01);	
    bcm2835_delay(0.04);		//delay
    buf[0] = bcm2835_spi_transfer(0xff);	//transfer register over spi
    buf[1] = bcm2835_spi_transfer(0xff);	//transfer register over spi
    bcm2835_gpio_write(22, 1);	//unenable cs pin
    //decoding retrieved register values
    read = ((UDOUBLE)buf[0] << 16) & 0x00FF0000;	
    read |= ((UDOUBLE)buf[1] << 8);
    if (read & 0x800000){read &= 0xFF000000;}
        
    temp = read*5.0/0x7fffff;	//converting to voltage level signal
    //fprintf(fptr, "%d     %1.10f\n", x, temp);
    fprintf(fptr, "%f\n", temp);	//store sequential values

    }
    return 0;
}