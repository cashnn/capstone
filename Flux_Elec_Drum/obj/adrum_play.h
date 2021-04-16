/*****************************************************************************
* | File      	:   adc_reader_wav.h
* | Author      :   Cash Niederhuber
* | Function    :   electric drum operation
* | Date        :   2021 / 03 / 04
* | Info        :

todo
add in a indexing structure that can be referenced when determining what type
of stroke has been detected
adjust the value of FILE_END to the correct value required
*****************************************************************************/
#ifndef _adrum_play_h_
#define _adrum_play_h_

//Library Declarations
#include <stdio.h>
#include <stdlib.h>     //exit()
#include <string.h>
#include <signal.h>     //signal()
#include <bcm2835.h>	//spi lib
#include <pthread.h>	//threading lib
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h> 
#include <unistd.h>
//Header Declarations
#include "ADS1256.h"	//adc header
#include "DAC8532.h"	//dac header
#include "wav_out.h"	//wav parser header


//Global Constants
double THRESHOLD = 0.2;
int MAX_FILE_NUM = 10;
double WAIT_TIME = 0.04;
int PERIOD = 44000;
int UP_LIM = 999956000;

//wav file index
char stroke[][10] = 
{
	"01A",
	"02A",
	"03A",
	"04A",
	"05A",
	"06A",
	"07A",
	"08A",
	"09A",
	"10A"
};


//Function Prototypes
double take_samp();
double local_peak(int);
void DAC_Hand(int hit_id);

#endif
