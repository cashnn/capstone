/*****************************************************************************
* | File        :   DAC_test.c
* | Author      :   Cash Niederhuber
* | Function    :   electric drum operation
* | Date        :   2021 / 03 / 04
* | Info        :   Program to test DAC operation
*****************************************************************************/

#include "adrum_play.h"

//Global Variables

//Function Prototypes
void DAC_Play(FILE *fptr, int num_samp);

//Interrupt Handling Subroutine
void Handler(int signo)
{
	//system Exit
	//fclose(fptr);
	DEV_ModuleExit();
	exit(0);
}


int main(void)
{
	int samp_num;
	int pos;
	DEV_DACModuleInit();
	signal(SIGINT, Handler);
	char filename[30] = "TEST_A.wav";
	FILE *fptr;
	fptr = fopen(filename, "rb");
	if(fptr == NULL)
	{
		fprintf(stderr, "Error Opening File\n");
		exit(0);
	}
	else
	{
		printf("File Opened\n");
	}
	
	//read data from file header
	pos = scan_wav(fptr, &samp_num);
	DAC_Play(fptr, samp_num);	

	fclose(fptr);
	DEV_ModuleExit();
	return(0);
}


void DAC_Play(FILE *fptr, int samp_num)
{
	int i, x;
	double amp;
	struct timespec tp;
	clock_id clk_id;
	clk_id = CLOCK_MONOTONIC;
	int start_t, end_t, diff_t;
	end_t = 0;
	printf("Starting Playback");
	for(i=0;i<samp_num;i++)
	{
		clock_gettime(clk_id, &tp);
		start_t = tp.tv_nsec;
		get_wav_samp(fptr);
		if(start_t > UP_LIM)
		{
			end_t = start_t;
			start_t = start_t%PERIOD;
			while(end_t > UP_LIM)
			{
				clock_gettime(clk_id, &tp)
				end_t = tp.tv_nsec;
			}
		}
		while(end_t <= (start_t + PERIOD))
		{
			clock_gettime(clk_id, &tp);
			end_t = tp.tv_nsec;
		}
		DAC8532_Out_Voltage(channel_A, amp);
		end_t = 0;
	}
}
