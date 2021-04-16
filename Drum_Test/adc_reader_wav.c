/*****************************************************************************
* | File        :   adc_reader_wav.c
* | Author      :   Cash Niederhuber
* | Function    :   electric drum operation
* | Date        :   2021 / 03 / 04
* | Info        :   main program function
*****************************************************************************/
#include "adc_reader_wav.h"

//Global Variables
double peak1_amp = 0;
int peak1_time;
int peak2_time = 0;
clock_t begin;
int mark;
int wav_size;


//Interrput Handling Sburoutine
void Handler(int signo)
{
    //System Exit
    clock_t end = clock();	//end time of scan
    double time_spent = (double)(end - begin)/CLOCKS_PER_SEC;	//computing elapsed adc run time
    printf("\nRun Time: %lf\n", time_spent);	//output run time for manual sample freq computations
    DEV_ModuleExit();
    exit(0);
}

int main(void)
{
    //DEV_ModuleInit();
    //reader variable declarations
    int samp_clock = 0;         //sample counter
    double sample;              //temp variable for current sample value
    double peak;                //temp variable for storing local peaks
    int stroke_flag = 0;    //flag for delaying next hit register
    int period_flag = 0;    //flag for delaying a period before new local max
    int output = 0;

    char filename[30] = "TEST_A.wav\n";
    mark = 0;

    // Exception handling:ctrl + c
    signal(SIGINT, Handler);
    //initialization of the adc board
//    if(ADS1256_init() == 1){
//        printf("\r\nEND                  \r\n");
//        DEV_ModuleExit();
//        exit(0);
//    }
    //setting up thread
    //pthread_t thread_id;
    //pthread_create(&thread_id, NULL, outputthread, NULL); <open new thread
    //pthread_join(thread_id, NULL);    <wait for thread to finish

    begin = clock();	//start of total scan time

    //continuous run loop
    while(1)
    {
        sample = take_samp(output);
        //interpretting read results
        samp_clock++;
        peak1_amp = peak;
        //if threshold is passed, hit is possible and peak is found
        if(sample > THRESHOLD && period_flag == 0)
        {

            period_flag = 1;    //setting flag to wait for next signal period
            peak = local_peak(samp_clock, output);
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
            mark = scan_wav(filename, &wav_size);
            //beginning output thread
            printf("Wave file parsed\n");
	        output = 1;
           
            fprintf(stdout, "Hit! of %lf at %d next peaks: ", peak, samp_clock);
		//printf("Hit\n");
	        fflush(stdout);    //forcing output to terminal

            stroke_flag = 2;
	    period_flag = 1;
        }

        //if period flag is active, wait until signal hits zero before finding next peak
        if(period_flag == 1)
        {
            while(sample > 0)
            {
		printf("herex\n");
                sample = take_samp(output);
                samp_clock++;
            }
        period_flag = 0;    //resetting period after signal dips below 0
	//printf("here\n");
        }
        
    }
    return 0;
}

/******************************************************************************
function:   take_samp()
arguments:  none
return:     sample - value of sample read from ADC
Info:function reads single sample value from ADC to be returned to main
******************************************************************************/
double take_samp(int output)
{
     DEV_ModuleInit();
     if(ADS1256_init() == 1){
        printf("\r\nEND                  \r\n");
        DEV_ModuleExit();
        exit(0);
    }
    char filename[30];
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
    if(output == 1)
    {
	DEV_ModuleExit();
	printf("here\n");
        mark = stream_wav_samp(filename, mark);
    }
    if(mark >= wav_size)
    {
        output = 0;
        mark = 0;
    }
	for(i=0;i<1000;i++){;}
    return(sample);
}

/******************************************************************************
function:   local_peak()
arguments:  samp_clock - current sample ticker for runtime
return:     max - value of peak of current oscillation period
Info:function calls for sequential samples in order to determine the moment
the upwards trend changes. Upon decrease in sample size the function returns
the localized peak for the given oscillation period
******************************************************************************/
double local_peak(int samp_clock, int output)
{
    peak1_time = peak2_time;
    double max;     //return value of local maximum
    double temp;    //temp variable for finding max
    int peak_flag = 0;  //flag defining when max has been found
    max = take_samp(output);
    samp_clock++;
    //continue searching for local peak until 2 sequential decreases are found
    while(peak_flag != 2)
    {
        temp = take_samp(output);    //retrieve next data point
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

