#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

//global variables
double THRESHOLD = 0.2;
int SAMP_NUM = 15000;
double peak1_amp = 0;
int peak1_time;
int peak2_time = 0;
//double data_set[SAMP_NUM];

//function prototypes
double local_peak(int, double data_set[]);

void main(void)
{
	double data_set[SAMP_NUM];	//array storing csv values
	int samp_clock = 0;			//sample counter
	double sample;				//temp variable for current sample value
	double peak;				//temp variable for storing local peaks
	int stroke_flag = 0;	//flag for delaying next hit register
	int period_flag = 0;	//flag for delaying a period before new local max
	//getting desired csv file path from user
	char filename[30];
	printf("Please specify file to analyse\n");
	fscanf(stdin, "%30[^\n]", filename);
	//open desired file for reading
	FILE *fptr;
	fptr = fopen(filename, "r");
	fseek(fptr, 0, SEEK_SET);
	//skip first line of csv file
	char skip[15];
	fscanf(fptr, "%s", skip);
	fscanf(fptr, "%s", skip);
	//read csv file results to an array
	while(samp_clock < SAMP_NUM)
	{
		fscanf(fptr, "%lf", &sample);
		data_set[samp_clock] = sample;
		samp_clock++;
	}
	fclose(fptr);
	printf("scan from csv complete\n");
	samp_clock = 0;		//resetting sample counter

	//interpretting array results
	while(samp_clock < SAMP_NUM)
	{
		sample = data_set[samp_clock];
		samp_clock++;
		peak1_amp = peak;

		//if threshold is passed, hit is possible and peak is found
		if(sample > THRESHOLD && period_flag == 0)
		{
			period_flag = 1;	//setting flag to wait for next signal period
			peak = local_peak(samp_clock, data_set);	
			//printf("%lf @ %d \n", peak, samp_clock);							
			//printf("%lf, ", peak);
			if(stroke_flag > 0){stroke_flag--;printf("%d, ", peak2_time);}//decrement peak counter
		}

		//output in the event a stroke is registered
		if(peak > (peak1_amp+0.12) && stroke_flag == 0)
		{
			fprintf(stdout, "\a");	//output computer beep sound
			printf("\nhit! of %lf at %d next peaks: ", peak, samp_clock);
			stroke_flag = 3;
		}

		//check if roll is present after first peak
		//if((peak - peak1_amp) < 0.1 && (peak2_time-peak1_time) < 30 && stroke_flag>0 && period_flag == 1)
		//{
		//	printf("roll!");
		//}


		//if period flag is active, wait until signal hits zero before finding next peak
		if(period_flag == 1)
		{
			while(sample > 0)
			{
				sample = data_set[samp_clock];
				samp_clock++;
			}
		period_flag = 0;	//resetting period after signal dips below 0
		}

	}


}

double local_peak(int samp_clock, double data_set[])
{
	peak1_time = peak2_time;
	double max;		//return value of local maximum
	double temp;	//temp variable for finding max
	int peak_flag = 0;	//flag defining when max has been found
	max = data_set[samp_clock];
	samp_clock++;
	//continue searching for local peak until 2 sequential decreases are found
	while(peak_flag != 2)
	{
		temp = data_set[samp_clock];	//retrieve next data point
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