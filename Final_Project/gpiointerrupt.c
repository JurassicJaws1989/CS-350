/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== gpiointerrupt.c ========
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>

/* Driver configuration */
#include "ti_drivers_config.h"
#include <ti/drivers/Timer.h>
#include <ti/drivers/UART2.h>
#include <ti/drivers/I2C.h>

/* Button Conditions*/
int leftButton = 0;
int rightButton = 0;

//Left button callback
void gpioButtonFxn0(uint_least8_t index)
{
    leftButton = 1;
}
//Right button callback
void gpioButtonFxn1(uint_least8_t index)
{
    rightButton = 1;
}

// Define the display function and declare bytes written for UART2 Write function
size_t bytesWritten;
#define DISPLAY(x) UART2_write(uart, &output, x, &bytesWritten);
// UART2 Global Variables
char output[64];
int bytesToSend;
//Driver Handles - Global Variables
UART2_Handle uart;
void initUART2(void)
{
    UART2_Params uartParams;
    //Init the driver
    //UART2_init();
    //Configure driver
    UART2_Params_init(&uartParams);
    //uartParams.writeMode = UART2_Mode_BLOCKING;
   // uartParams.readMode = UART2_Mode_BLOCKING;
   // uartParams.readReturnMode = UART2_RETURN_FULL;
    uartParams.baudRate = 115200;
    //open driver
    uart = UART2_open(CONFIG_UART2_0, &uartParams);
    if (uart == NULL)
    {
        /* UART_open() failed*/
        while(1);
    }

}

// I2C Global Variables
static const struct
{
    uint8_t address;
    uint8_t resultReg;
    char *id;
}
sensors[3] =
{
         { 0x48, 0x0000, "11X" },
         { 0x49, 0x0000, "116" },
         { 0x41, 0x0001, "006" }
};
uint8_t txBuffer[1];
uint8_t rxBuffer[2];
I2C_Transaction i2cTransaction;

// Driver Handles - Global Variables
I2C_Handle i2c;
char output[64];

//Make sure to call initUART2() before calling this function!
void initI2C(void)
{
    int8_t i, found;
    I2C_Params i2cParams;
    DISPLAY(printf(output, 64, "Initializing I2C Driver - "));

    // Init the driver
    I2C_init();

    //Configure driver
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    //Open driver
    i2c = I2C_open(CONFIG_I2C_0, &i2cParams);
    if (i2c == NULL)
    {
        DISPLAY(snprintf(output, 64, "Failed\n\r"));
        while (1);
    }
    DISPLAY(snprintf(output, 32, "Passed\n\r"));

    //Boards were shipped with different sensors.
    //Welcome to the world of embedded systems.
    //Try to determine which sensor we have.
    //Scan through the possible sensor addresses

    /* Common I2C Transaction Setup*/
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 0;
    found = false;
    for (i = 0; i < 3; ++i)
    {
        i2cTransaction.targetAddress = sensors[i].address;
        txBuffer[0] = sensors[i].resultReg;
        DISPLAY(snprintf(output, 64, "Is this %s? ", sensors[i].id));
        if (I2C_transfer(i2c, &i2cTransaction))
        {
            DISPLAY(snprintf(output, 64, "Found\n\r"));
            found = true;
            break;
        }
        DISPLAY(snprintf(output, 64, "No\n\r"));

    };
    if (found)
    {
        DISPLAY(snprintf(output, 64, "Detected TMP%s I2C address: %x\n\r", sensors[i].id, i2cTransaction.targetAddress));
    }
    else
    {
        DISPLAY(snprintf(output, 64, "Temperature sensor not found, contact professor!\n\r"));
    }
}
int16_t readTemp(void)
{
    int j;
    int16_t temperature = 0;
    i2cTransaction.readCount = 2;
    if (I2C_transfer(i2c, &i2cTransaction))
    {
        /*
         * Extract degrees C from the received data;
         * see TMP sensor datasheet
         */
        temperature = (rxBuffer[0] << 8) | (rxBuffer[1]);
        temperature *= 0.0078125;
        /*
         * If the MSB is set '1', then we have a 2's complement
         * negative value which needs to be sign extended
         */
        if (rxBuffer[0] & 0x80)
        {
            temperature |= 0xF000;
        }
    }
    else
    {
        DISPLAY(snprintf(output, 64, "Error reading temperature sensor (%d)\n\r", i2cTransaction.status));
        DISPLAY(snprintf(output, 64, "Please power cycle your board by unplugging USB and plugging it back in.\n\r"));
    }
    return temperature;
}


//Timer Driver Handles - Global Variables
Timer_Handle timer0;
volatile unsigned char TimerFlag = 0;


/* Timer code integration
 */
void timerCallback(Timer_Handle myHandle, int_fast16_t status)
{
    TimerFlag = 1;

}

void initTimer(void)
{
    //Timer_Handle timer0;
    Timer_Params params;
    //init the driver
    Timer_init();
    Timer_Params_init(&params);
    params.period = 1000000;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;
    //open driver
    timer0 = Timer_open(CONFIG_TIMER_0, &params);

    if (timer0 == NULL) {
        /* Failed to initialize timer */
        while (1) {}
    }

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        /* Failed to start timer */
        while (1) {}
    }
}


/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* Call driver init functions */
    GPIO_init();
    //configure red LED
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    //configure button 0 pin
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    //turn on user LED
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    //button callback
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonFxn0);
    //interrupts
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);
    //handle interrupts on potential second button
    if (CONFIG_GPIO_BUTTON_0 != CONFIG_GPIO_BUTTON_1)
    {
        //button 1 configuration
        GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
        //button1 callback
        GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);
        //button1 interrupt
        GPIO_enableInt(CONFIG_GPIO_BUTTON_1);
    }
    initUART2();
    initI2C();
    initTimer();
    //Check Time Variables
    unsigned long buttonTime = 0;
    unsigned long tempTime = 0;
    unsigned long displayTime = 0;
    //Period Variables
    const unsigned long buttonPeriod = 200;
    const unsigned long tempPeriod = 500;
    const unsigned long displayPeriod = 1000;
    const unsigned long timerPeriod = 100;
    //Thermostat Variables
    int setPoint = 25;
    int heat = 0;
    int seconds = 0;
    int temp = 0;

    //Execution loop
    while(1)
    {
        readTemp();
        //detect button presses every 200ms - Increase setpoint temp with right button, decrease with left in increments of 1c
        if (buttonTime >= buttonPeriod)
        {
            if (rightButton == 1)
            {
                setPoint += 1;
                rightButton = 0;
            }
            if (leftButton == 1)
            {
                setPoint -= 1;
                leftButton = 0;
            }
        }
        //update temperature reading every 500ms, disable heat if condition is met, enable if not
        if (tempTime >= tempPeriod)
        {
            temp = readTemp();
            if (temp < setPoint)
            {
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
                heat = 1;
            }
            else
            {
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
                heat = 0;
            }
        }
        //Output temperature to UART every 1000ms
        if (displayTime >= displayPeriod)
        {
            DISPLAY(snprintf(output, 64, "<%02d,%02d,%d,%04d>\n\r", temp, setPoint, heat, seconds));
            ++seconds;
        }
        //wait for the timer's period
        while(!TimerFlag){}
        //Lower flag
        TimerFlag = 0;
        //Increase check times by timer period
        buttonTime += timerPeriod;
        tempTime += timerPeriod;
        displayTime += timerPeriod;
    }

}
