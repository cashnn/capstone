/*****************************************************************************
* | File      	:   wav_out.h
* | Author      :   Cash Niederhuber
* | Function    :   wav file parsing and output
* | Date        :   2021 / 03 / 04
* | Info        :
*****************************************************************************/
#ifndef _WAV_OUT_H_
#define _WAV_OUT_H_

//library Declarations
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdint.h>
 #include <inttypes.h>
 #include <byteswap.h>
 #include <time.h>
 #include <sys/timeb.h>
//Header Declarations
 #include "DEV_Config.h"
 #include "DAC8532.h"

//global constants
 #define WAV_16b_MAX 65535

//function prototypes
double get_wav_samp(FILE *fptr);
int scan_wav(FILE *fptr, int *num_samp);
#endif
