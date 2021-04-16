/*****************************************************************************
* | File        :   adrum_play.c
* | Author      :   Cash Niederhuber
* | Function    :   electric drum operation
* | Date        :   2021 / 03 / 04
* | Info        :   main program function
*
todo
Define any flag limits in the header file
*****************************************************************************/

#include "adrum_play.h"

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
	int samp_clock = 0;         //sample counter
   	double sample;              //temp variable for current sample value
    	double peak;                //temp variable for storing local peaks
    	int stroke_flag = 0;    //flag for delaying next hit register
    	int period_flag = 0;    //flag for delaying a period before new local max
    	int output = 0;

	signal(SIGINT, Handler);
	char filename[30] = "TEST_A.wav\n";
	begin = clock();

	while(1)
	{
		sample = SPI_hand(output, filename);
		samp_clock++;
		peak1_amp = peak;
		//if threshold is passed then peak must be found
		if(sample > THRESHOLD && period_flag == 0)
		{
			period_flag = 1;
			peak = local_peak(samp_clock, output, filename);
			if(stroke_flag > 0)
			{
				stroke_flag--;
			}
		}

		if(peak > (peak1_amp+0.12) && stroke_flag == 0)
		{
			mark = scan_wav(filename, &wav_size);
			printf("Number of bytes: %d\n", wav_size);
			output = 1;

			stroke_flag = 2;
	        period_flag = 1;
		}

		if(period_flag == 1)
        {
        	while(sample > 0)
            {
		      printf("herex\n");
                sample = SPI_hand(output, filename);
                samp_clock++;
            }
        	period_flag = 0;
        }

	}


}

/******************************************************************************
function:   DAC_Hand()
arguments:  address of hit marker location
return:     none
Info:function 
ISSUE 1: Notifying function that a new hit is registered
ISSUE 2: How to start DAC_Hand() from the main, fork?
ISSUE 3: How to ID the appropriate wav file based on main program

solutions:
DAC_Hand(int *hit_id) - where hit_id is set back to zero by DAC_Hand each time
a new hit is recognized, main problem is how to update hit_id in the main in 
such a way as to not cause conflicts between DAC_Hand() and the main

todo:
Add clock based synchronization for DAC output
determine the correct end file condition
Add wav file definition index to header
adjust fopen call to open the required wav files based on file definition index
******************************************************************************/
void DAC_Hand(int *hit_id);
{
	//variable Declarations
	int wav_size;		//variable needed for scan_wav(), not used elsewhere
	int i;			//general index counter
	FILE *fptr[MAX_FILE_NUM];//Array or indexed file pointers
	int file_tracker[MAX_FILE_NUM];	//Array for marking end of each file
	//continuously outputing loop
	while(1)
	{
		//open files if applicable
		if(hit_id != 0)
		{
			for(i=0;i<MAX_FILE_NUM;i++)
			{
				if(fptr[i] == NULL)
				{
					//open new file pointed for latest stroke
					fptr[i] = fopen("TEST_A.wav", "rb");
					//begin file tracking
					file_tracker[i] = scan_wav(fptr[num_files], wav_size);
				}
			}
		}	
		//retrieve current sample data
		for(i=0;i<=num_files;i++)
		{
			//check if file is finished
			if(file_tracker[i] >= FILE_END)
			{
				num_files--;
				fclose(fptr[i]);
				file_tracker[i] = 0;
			}	
			//sum individual file samples
			if(fptr[i] != NULL)
			{
				sum = sum + get_wav_samp(fptr[i]);
				file_tracker[i]++;
			}
		}
		//Wait for output window
		//Clock based synchronizer	
		//DAC output execution
		DAC_Out_Voltage(channel_A, sum);	
	}
}



/******************************************************************************
function:   take_samp()
arguments:  none
return:     sample - value of sample read from ADC
Info:function reads single sample value from ADC to be returned to main

todo
Look for inefficiencies
******************************************************************************/
double take_samp()
{
    UDOUBLE read = 0;
    double sample;    //voltage sample variable
    UBYTE buf[3];   //register storage location
    UDOUBLE i;
    //initializing SPI comms
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

    bcm2835_delay(WAIT_TIME);        //delay
    bcm2835_gpio_write(22, 0);  //cs pin to write
    bcm2835_spi_transfer(0x01); 
    bcm2835_delay(WAIT_TIME);        //delay
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


/******************************************************************************
function:   local_peak()
arguments:  samp_clock - current sample ticker for runtime
return:     max - value of peak of current oscillation period
Info:function calls for sequential samples in order to determine the moment
the upwards trend changes. Upon decrease in sample size the function returns
the localized peak for the given oscillation period

todo
Look for inefficiencies
******************************************************************************/
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
