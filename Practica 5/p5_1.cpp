/* A to D conversion of channel 0
* This program converts the analog input from channel 0 (PTE20)
* using software trigger continuously.
* Bits 10-8 are used to control the tri-color LEDs. LED code * is  copied from p2_7. Connect a potentiometer between 3.3V * and  ground.
* The wiper of the potentiometer is connected to PTE20.
When the potentiometer is turned, the LEDs should change   color. */

#include "mbed.h"
#include "TextLCD.h" //Peter Dresher 2010 RW

TextLCD lcd(PTD0, PTD2, PTD3, PTD4, PTD5, PTD6, PTD7); // rs, rw, e, d4-d7
void ADC0_init(void);
void LED_set(int s);
void LED_init(void);
void delayMs(int n);

int main (void)
{
    short int result;
    LED_init(); /* Configure LEDs */
    ADC0_init(); /* Configure ADC0 */
    while (1) {
        ADC0->SC1[0] = 26; /* start conversion on channel 26 temperature */
        while(!(ADC0->SC1[0] & 0x80)) { } /* wait for COCO */
        result = ADC0->R[0]; /* read conversion result and clear COCO flag */
        LED_set(result); /* display result on LED */
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("%d\n",result);
        delayMs(500);

    }
}

void ADC0_init(void)
{
    SIM->SCGC6 |= 0x8000000; /* clock to ADC0 */
    ADC0->SC2 &= ~0x40; /* software trigger */
    /*CLKDIV/4, LS time, single ended 12 bit, bus clock */
    ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00;
}
void LED_init(void)
{
    SIM->SCGC5 |= 0x400; /* enable clock to Port B */
    SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
    PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
    PTB->PDDR |= 0x40000; /* make PTB18 as output pin */
    PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */
    PTB->PDDR |= 0x80000; /* make PTB19 as output pin */
    PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
    PTD->PDDR |= 0x02; /* make PTD1 as output pin */
}

void LED_set(int s)
{
    if (s & 1) /* use bit 0 of s to control red LED */
        PTB->PCOR = 0x40000; /* turn on red LED */
    else
        PTB->PSOR = 0x40000; /* turn off red LED */

    if (s & 2) /* use bit 1 of s to control green LED */
        PTB->PCOR = 0x80000; /* turn on green LED */
    else
        PTB->PSOR = 0x80000; /* turn off green LED */

    if (s & 4) /* use bit 2 of s to control blue LED */
        PTD->PCOR = 0x02; /* turn on blue LED */
    else
        PTD->PSOR = 0x02; /* turn off blue LED */
}

void delayMs(int n)
{
    int i;
    SysTick->LOAD = 41940 - 1;
    SysTick->CTRL = 0x5; /* Enable the timer and choose sysclk as the clock source */

    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0)
            /* wait until the COUTN flag is set */
        {
        }
    }
    SysTick->CTRL = 0;
    /* Stop the timer (Enable = 0) */
}
