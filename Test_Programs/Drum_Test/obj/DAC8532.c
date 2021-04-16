/*****************************************************************************
* | File      	:   DAC8532.C
* | Author      :   Waveshare team
* | Function    :   DAC8532 driver
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-03-25
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "DAC8532.h"

/******************************************************************************
function:	Write a data to the DAC channel
parameter:
    Channel: Channel number  channel_A and channel_B
    Data   : Written data  0-65535
Info:
******************************************************************************/
static void Write_DAC8532(UBYTE Channel, UWORD Data)
{
    DEV_Digital_Write(DEV_DACCS_PIN, 1);
    DEV_Digital_Write(DEV_DACCS_PIN, 0);
    DEV_SPI_WriteByte(Channel);
    DEV_SPI_WriteByte((Data>>8));
    DEV_SPI_WriteByte((Data&0xff));  
    DEV_Digital_Write(DEV_DACCS_PIN, 1);
}

/******************************************************************************
function:	Specify a channel output voltage value
parameter:
    Channel: Channel number,  channel_A and channel_B
    Voltage: Output voltage value, 0-DAC_VREF
Info:
******************************************************************************/
void DAC8532_Out_Voltage(UBYTE Channel, float Voltage)
{
    UWORD temp = 0;
    if((Voltage <= DAC_VREF) && (Voltage >= 0)){
        temp = (UWORD)(Voltage * DAC_Value_MAX / DAC_VREF);
        Write_DAC8532(Channel, temp);
    }
}



