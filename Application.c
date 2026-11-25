/**********************************************************************************************************************
 * \file Application.c
 * @naga kumar - This file generated just for learning purposes and experiments
 **********************************************************************************************************************/

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxWtu.h"
#include "IfxStm.h"
#include "IfxPort.h"
#include "Bsp.h"
#include "Application.h"



/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define LED_PORT_D107    &MODULE_P13,0                                           /* LED PORT D107: Port, Pin definition       */
#define LED_PORT_D108    &MODULE_P14,0                                           /* LED PORT D108: Port, Pin definition       */
#define LED_PORT_D109    &MODULE_P14,1                                           /* LED PORT D109: Port, Pin definition       */
#define LED_PORT_D110    &MODULE_P14,2                                           /* LED PORT D110: Port, Pin definition       */

#define WAIT_TIME1        250  /* Wait time1 in milliseconds */
#define WAIT_TIME2        500  /* Wait time2 in milliseconds */
#define WAIT_TIME3        750  /* Wait time3 in milliseconds */
#define WAIT_TIME4        1000  /* Wait time4 in milliseconds */

#define FIFO_CAPACITY     8U

static uint16 get2ByteRandomnumber(void);

/* Circular buffer structure */
typedef struct
{
    uint16  buffer[FIFO_CAPACITY];
    uint16 *pHead;      /* Pointer to next write location */
    uint16 *pTail;      /* Pointer to next read location  */
    uint16 *pBufStart;  /* Fixed pointer to start of array */
    uint16 *pBufEnd;    /* Fixed pointer to end of array   */
    uint32  count;      /* Number of active items in FIFO  */
} RingBuffer_t;

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/

/* Initialize pointers inside the struct */
void fifoInit(RingBuffer_t *pFifo)
{
    if (pFifo == NULL_PTR)
    {
        return;
    }

    pFifo->pBufStart = &pFifo->buffer[0];
    pFifo->pBufEnd   = &pFifo->buffer[FIFO_CAPACITY]; /* One past last element */
    pFifo->pHead     = pFifo->pBufStart;
    pFifo->pTail     = pFifo->pBufStart;
    pFifo->count     = 0U;
}

/* Below function initializes the ports pins which drives the LEDs */
void initializeLEDPorts(void)
{
    /* Initialization of the LED port D107 used in this example */
    IfxPort_setPinModeOutput(LED_PORT_D107, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED_PORT_D108, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED_PORT_D109, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED_PORT_D110, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    /* Switch OFF the LED (low-level active) */
    IfxPort_setPinHigh(LED_PORT_D107);
    IfxPort_setPinHigh(LED_PORT_D108);
    IfxPort_setPinHigh(LED_PORT_D109);
    IfxPort_setPinHigh(LED_PORT_D110);
}

/* Function to initialize the random seed using the hardware System Timer */
void initializeRandomSeed(void)
{
    unint32 seed = 0;

    /* Read the lower 32 bits of the free-running STM0 timer */
    seed = IfxStm_getLower(&MODULE_STM0);
    srand(seed);
}


/* Push a 2-byte sample into the ring buffer using pointers */
boolean fifoPush(RingBuffer_t *pFifo, uint16 sample)
{
    if ((pFifo == NULL_PTR) || (pFifo->count >= FIFO_CAPACITY))
    {
        return (FALSE); /* Buffer full or invalid pointer */
    }

    /* Write sample to the address pointed to by pHead */
    *(pFifo->pHead) = sample;
    pFifo->pHead++;

    /* Wrap pHead back to start if it reaches the end of the array */
    if (pFifo->pHead >= pFifo->pBufEnd)
    {
        pFifo->pHead = pFifo->pBufStart;
    }

    pFifo->count++;
    return (TRUE);
}

/* Pop a 2-byte sample out via an output pointer */
boolean fifoPop(RingBuffer_t *pFifo, uint16 *pOutSample)
{
    if ((pFifo == NULL_PTR) || (pOutSample == NULL_PTR) || (pFifo->count == 0U))
    {
        return (FALSE); /* Buffer empty or invalid pointer */
    }

    /* Read from pTail and write into caller's variable */
    *pOutSample = *(pFifo->pTail);
    pFifo->pTail++;

    /* Wrap pTail back to start if it reaches the end */
    if (pFifo->pTail >= pFifo->pBufEnd)
    {
        pFifo->pTail = pFifo->pBufStart;
    }

    pFifo->count--;
    return (TRUE);
}

/* Read-only pointer traversal (const RingBuffer_t *) to calculate average */
uint16 fifoAverage(const RingBuffer_t *pFifo)
{
    uint32 sum = 0U;
    uint32 i = 0;
    const uint16 *pCursor;
    uint32 average = 0U;


    if ((pFifo == NULL_PTR) || (pFifo->count == 0U))
    {
        return 0U;
    }
    
    pCursor = pFifo->pTail;

    for (i = 0U; i < pFifo->count; i++)
    {
        sum += *pCursor;
        pCursor++;

        if (pCursor >= pFifo->pBufEnd)
        {
            pCursor = pFifo->pBufStart;
        }
    }

    average = (uint16)(sum / pFifo->count);

    return ;
}


/* This function toggles the port pins and wait in different constant intervals of milliseconds */
void toggleLEDs(void)
{

    uint16 randomValue = get2ByteRandomnumber();  /* Generate a 2-byte random number */

    /*wait for some time which is random*/
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, randomValue));        /* Wait 250 milliseconds            */

    /* Now on, follow fixed time intervals in sequence*/ /* Improve this logic better in future by using an array of wait times */
    IfxPort_togglePin(LED_PORT_D107);                                                /* Toggle the state of the LED      */
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME1));        /* Wait 250 milliseconds            */

    IfxPort_togglePin(LED_PORT_D108);                                                /* Toggle the state of the LED      */
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME2));        /* Wait 500 milliseconds            */

    IfxPort_togglePin(LED_PORT_D109);                                                /* Toggle the state of the LED      */
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME3));        /* Wait 750 milliseconds            */
    
    IfxPort_togglePin(LED_PORT_D110);                                                /* Toggle the state of the LED      */
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME4));        /* Wait 1000 milliseconds            */
}


/* Below function generates a 2-byte (16-bit) random number */
static uint16 get2ByteRandomnumber(void)
{
    /* Mask with 0xFFFF to ensure a 16-bit (2-byte) value: 0 to 65535 */
    return (uint16)(rand() & 0xFFFFU);
}


