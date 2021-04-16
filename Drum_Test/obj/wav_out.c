/*****************************************************************************
* | File      	:   wav_out.c
* | Author      :   Cash Niederhuber
* | Function    :   wav file parsing and output
* | Date        :   2021 / 03 / 04
* | Info        :
*****************************************************************************/

#include "wav_out.h"

/******************************************************************************
function:	get_wav_samp()
arguments:	fptr - file pointer
return:
Info: function used to retrieve the most subsequent sample value from any given
file, the retieved value is then converted to a voltage measure and returned

todo:
find a better way to skip over the channel B samples that does not involve 
reading them, ie. *move pointer 16bits ahead in file*
******************************************************************************/

int get_wav_samp(FILE *fptr)
{
	int amp;
	int16_t sign_byte;
	unsigned char buffer[2];
	//reading data of channel A
	fread(buffer, sizeof(buffer), 1, fptr);
	fread(buffer, sizeof(buffer), 1, fptr);
	sign_byte = buffer[0]|(buffer[1]<<8);
	amp = ((double)sign_byte/WAV_16b_MAX)*3.3;
	//channel B sample is ignored
	fread(buffer, sizeof(buffer), 1, fptr);
	fread(buffer, sizeof(buffer), 1, fptr);
	return(amp);
}


/******************************************************************************
function:	scan_wav()
arguments:	fptr - pointer to wav file to be parsed
		num_samp - pointer to address of sample number storage
return:		pos - file pointer location of sample data
Info:function used to read the header information of a wav file returning a 0
if file format is incorrect. After function finds the ending of the wav header 
it returns the file pointer location where the sample data is stored

todo:
write a function that can retrive a 4 BYTE word and return the value read in 
order to slim dowm the size of program(read RIFF, WAVE etc.)
write a functions that can read little endian numbers based on file pointer 
and a passed return size specifying the number of bits to be read
******************************************************************************/
int scan_wav(FILE *fptr, int *num_samp)
{
	int i;
	unsigned int byte_le;
	char byte1[4];
	char byte2[4];
	char byte3[4];
	char byte4[4];
	unsigned char buffer4[4];
	unsigned char buffer2[2];
	unsigned char buffer[1];

	//Begin at start of File
	fseek(fptr, 0, SEEK_SET);
	
	//RIFF identifier
	if(fread(byte1,sizeof(byte1),1,fptr) != 1)
		return(0);
	if(strcmp(byte1, "RIFF") != 0)
		return(0);

	//Total File Size
	fseek(fptr, 4, SEEK_SET);
	fread(buffer4, sizeof(buffer4), 1, fptr);
	byte_le = buffer4[0] |(buffer4[1]<<8) |(buffer4[2]<<16) |(buffer4[3]<<24);
	//printf("Total File Size \t%" PRIu32 "\n", byte_le);

	//Wave identifier
	fseek(fptr, 8, SEEK_SET);
	if(fread(byte2,sizeof(byte2),1,fptr) != 1)
		return(0);
	if(strcmp(byte2, "WAVE") != 0)
		return(0);

	int pos = 0;
	fread(buffer,sizeof(buffer),1,fptr);
	while(buffer[0] != 'f')
		{
			fread(buffer,sizeof(buffer),1,fptr);
			pos++;
		}
	fseek(fptr,pos+12,SEEK_SET);
	if(fread(byte3,sizeof(byte3),1,fptr) != 1)	//read fmt
		return(0);
	if(strcmp(byte3, "fmt ") != 0)
		return(0);

	//format data length
	fread(buffer4, sizeof(buffer4), 1, fptr);
	byte_le = buffer4[0] |(buffer4[1]<<8) |(buffer4[2]<<16) |(buffer4[3]<<24);
	//printf("Format data length \t%" PRIu32 "\n", byte_le);

	//format type
	fread(buffer2, sizeof(buffer2), 1, fptr);
	byte_le = buffer2[0] |(buffer2[1]<<8); // |(buffer4[2]<<16) |(buffer4[3]<<24);
	//printf("Format type (PCM = 1) \t%" PRIu16 "\n", byte_le);

	//number of channels
	fread(buffer2, sizeof(buffer2), 1, fptr);
	byte_le = buffer2[0] |(buffer2[1]<<8); // |(buffer4[2]<<16) |(buffer4[3]<<24);
	//printf("Number of channels \t%" PRIu16 "\n", byte_le);

	//sample rate
	fread(buffer4, sizeof(buffer4), 1, fptr);
	byte_le = buffer4[0] |(buffer4[1]<<8) |(buffer4[2]<<16) |(buffer4[3]<<24);
	//printf("Sample Rate \t\t%" PRIu32 "\n", byte_le);

	//(Sample Rate * BitsPerSample * Channels) / 8.
	fread(buffer4, sizeof(buffer4), 1, fptr);
	byte_le = buffer4[0] |(buffer4[1]<<8) |(buffer4[2]<<16) |(buffer4[3]<<24);
	//printf("SR*Bps*C/8 \t\t%" PRIu32 "\n", byte_le);

	//(BitsPerSample * Channels) / 8.1 – 8 bit mono2 – 8 bit stereo/16 bit mono4 – 16 bit stereo
	fread(buffer2, sizeof(buffer2), 1, fptr);
	byte_le = buffer2[0] |(buffer2[1]<<8); // |(buffer4[2]<<16) |(buffer4[3]<<24);
	//printf("bps*C/8 \t\t%" PRIu16 "\n", byte_le);
	int channels = byte_le;

	//Bits per sample
	fread(buffer2, sizeof(buffer2), 1, fptr);
	byte_le = buffer2[0] |(buffer2[1]<<8); // |(buffer4[2]<<16) |(buffer4[3]<<24);
	//printf("bps  \t\t\t%" PRIu16 "\n", byte_le);
	int BPS = byte_le;

	pos = ftell(fptr);

	//data begining
	fseek(fptr, pos, SEEK_SET);
	if(fread(byte4,sizeof(byte4),1,fptr) != 1)
		return(0);

	//size of data section
	fread(buffer4, sizeof(buffer4), 1, fptr);
	byte_le = buffer4[0] |(buffer4[1]<<8) |(buffer4[2]<<16) |(buffer4[3]<<24);
	//printf("Raw data length \t%" PRIu32 "\n", byte_le);
	int data = byte_le;
	*num_samp = data;
	pos = ftell(fptr);
	fclose(fptr);
	return(pos);
}


