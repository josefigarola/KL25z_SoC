#include "mbed.h"

InterruptIn blue(PTA1);
InterruptIn green(PTA2);

int IRQ; //global variable

void flip(void);
void no_flip(void);
void LED_init(void);
void PORTA_IRQHandler(void);
void delayMs(int n);

int main()
{
    LED_init();
    //initialize both buttons
    blue.mode(PullUp);
    blue.rise(&flip);  // attach the function to the raising edge
    blue.fall(&no_flip);  // attach function to the falling edge

    green.mode(PullUp);
    green.rise(&flip);  // attach the function to the raising edge
    green.fall(&no_flip);  // attach function to the falling edge

    IRQ=0;

    while(1) {
        PTB->PTOR |= 0x40000; /* toggle red LED */
        delayMs(500);
        if(IRQ==1) {
            PORTA_IRQHandler();
        }  // end if
    } // end while
}

void LED_init(void)
{
    SIM->SCGC5 |= 0x400; /* enable clock to Port B */
    SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
    PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
    PTB->PDDR |= 0x40000; /* make PTB18 as output pin */
    PTB->PSOR |= 0x40000; /* turn off red LED */
    PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */
    PTB->PDDR |= 0x80000; /* make PTB19 as output pin */
    PTB->PSOR |= 0x80000; /* turn off green LED */
    PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
    PTD->PDDR |= 0x02; /* make PTD1 as output pin */
    PTD->PSOR |= 0x02; /* turn off blue LED */
}

void PORTA_IRQHandler(void)
{
    /* toggle green LED (PTB19) three times */
    for (int i = 0; i < 3; i++) {
        PTB->PDOR &= ~0x80000; /* turn on green LED */
        delayMs(500);
        PTB->PDOR |= 0x80000; /* turn off green LED */
        delayMs(500);
    }
    //clear interrupt flag
    IRQ=0;
}

void flip(void)
{
    IRQ=1;
}

void no_flip(void)
{
    IRQ = 0;
}

/* Delay n milliseconds */
void delayMs(int n)
{
    int i;
    int j;
    for(i = 0 ; i < n; i++) {
        for (j = 0; j < 7000; j++) {}
    }
}