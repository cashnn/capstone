/*****************************************************************************
* | File        :   DAC_test.c
* | Author      :   Cash Niederhuber
* | Function    :   electric drum operation
* | Date        :   2021 / 03 / 04
* | Info        :   Program to test DAC operation
*****************************************************************************/

#include "adrum_play.h"

//Global Variables
int PERIOD = 44000;
int UP_LIM = 999956000;

//Function Prototypes
void DAC_Play(FILE *fptr, int num_samp);

//Interrupt Handling Subroutine
void Handler(int signo)
{
	//system Exit
	DEV_DACModuleExit();
	exit(0);
}


int main(void)
{
	int samp_num;
	int pos;
	DEV_DACModuleInit();
	DAC8532_Out_Voltage(channel_A, 0);
	DAC8532_Out_Voltage(channel_B, 0);
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
	printf("position: %d\n", pos);
	printf("samp_num %d\n", samp_num);
	DAC_Play(fptr, samp_num);	

	fclose(fptr);
	DEV_DACModuleExit();
	return(0);
}


void DAC_Play(FILE *fptr, int samp_num)
{
	int i;
	double amp;
	struct timespec tp;
	clockid_t clk_id;
	clk_id = CLOCK_MONOTONIC;
	int start_t, end_t;
	end_t = 0;
	printf("Starting Playback\n");
	for(i=0;i<(samp_num/12);i++)
	{
		fflush(stdin);
		clock_gettime(clk_id, &tp);
		start_t = tp.tv_nsec;
		amp = get_wav_samp(fptr);
		if(start_t > UP_LIM)
		{
			end_t = start_t;
			start_t = start_t%PERIOD;
			while(end_t > UP_LIM)
			{
				clock_gettime(clk_id, &tp);
				end_t = tp.tv_nsec;
			}
		}
		while(end_t <= (start_t + PERIOD))
		{
			clock_gettime(clk_id, &tp);
			end_t = tp.tv_nsec;
		}
		DAC8532_Out_Voltage(channel_A, amp);
		DAC8532_Out_Voltage(channel_B, 0);
		end_t = 0;
	}

}
