/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master 
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2019-03-12
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
#include "DEV_Config.h"

//function prototype Definition
static void DEV_DACGPIOConfig(void);

/******************************************************************************
function:	ADC Initialization pin
parameter:
Info:
******************************************************************************/
static void DEV_GPIOConfig(void)
{
     //output
    bcm2835_gpio_fsel(DEV_RST_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(DEV_CS_PIN, BCM2835_GPIO_FSEL_OUTP);

    //intput
    bcm2835_gpio_fsel(DEV_DRDY_PIN,BCM2835_GPIO_FSEL_INPT);
	
}

/******************************************************************************
function:   DAC Initialization pin
parameter:
Info:
******************************************************************************/
static void DEV_DACGPIOConfig(void)
{
    //output
    bcm2835_gpio_fsel(DEV_DACCS_PIN, BCM2835_GPIO_FSEL_OUTP);
    
}

/******************************************************************************
function:	Module Initialize, the BCM2835 library and initialize the pins, SPI protocol for ADC
parameter:
Info:		Initialization of SPI0 Comms Channel
******************************************************************************/
UBYTE DEV_ModuleInit(void)
{
    if(!bcm2835_init()) {
        printf("bcm2835 init failed  !!! \r\n");
        return 1;
    } else {
        //printf("bcm2835 init success !!! \r\n");
    }
	
	DEV_GPIOConfig();

    bcm2835_spi_begin();                                         //Start spi interface, set spi pin for the reuse function
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);     //High first transmission
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);                  //spi mode 0
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);  //Frequency	Alteration #1(from 64 to 256)
    return 0;
}

/******************************************************************************
function:   Module Initialize, the BCM2835 library and initialize the pins, SPI protocol for DAC
parameter:
Info:	    Initializaton of SPI0 Comms channel for DAC
******************************************************************************/
UBYTE DEV_DACModuleInit(void)
{
    if(!bcm2835_init()) {
        printf("bcm2835 init failed  !!! \r\n");
        return 1;
    } else {
        //printf("bcm2835 init success !!! \r\n");
    }
    
    DEV_DACGPIOConfig();

    bcm2835_spi_begin();                                         //Start spi interface, set spi pin for the reuse function
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);     //High first transmission
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);                  //spi mode 0
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);  //Frequency
    
    DEV_Digital_Write(DEV_DACCS_PIN, 1);
    return 0;
}

/******************************************************************************
function:   Module Initialize, the BCM2835 library and initialize the pins, SPI1 protocol for DAC
parameter:
Info:	    Initializaton of SPI1 Comms channel for DAC
******************************************************************************/
UBYTE DEV_AUX_DACModuleInit(void)
{
    if(!bcm2835_init()) {
        printf("bcm2835 init failed  !!! \r\n");
        return 1;
    } else {
        //printf("bcm2835 init success !!! \r\n");
    }
    
    DEV_DACGPIOConfig();

    bcm2835_aux_spi_begin();                                         //Start spi interface, set spi pin for the reuse function
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);     //High first transmission
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);                  //spi mode 0
    bcm2835_aux_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);  //Frequency
    
    DEV_Digital_Write(DEV_DACCS_PIN, 1);
    return 0;
}



/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_ModuleExit(void)
{
    bcm2835_spi_end();		//close SPI0
    bcm2835_close();
}

/******************************************************************************
function:	Module exits, closes SPI library
parameter:
Info:
******************************************************************************/
void DEV_DACModuleExit(void)
{
	bcm2835_spi_end();	//close SPI1
}

/******************************************************************************
function:	Module exits, closes aux SPI library
parameter:
Info:
******************************************************************************/
void DEV_AUX_DACModuleExit(void)
{
	bcm2835_aux_spi_end();	//close SPI1
}
