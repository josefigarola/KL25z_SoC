/* Toggling LEDs using SysTick counter

This program let the SysTick counter run freely and dumps the counter values to the tri-color LEDs continuously.

The counter value is shifted 4 places to the right so that the changes of LEDs will be slow enough to be visible.

SysTick counter has 24 bits.
The red LED is connected to PTB18.
The green LED is connected to PTB19.
*/
#include <MKL25Z4.H>
int main (void) {
int c;
SIM->SCGC5 |= 0x400; /* enable clock to Port B */
PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */
PTB->PDDR |= 0xC0000; /* make PTB18, 19 as output pin */

/* Configure SysTick */
SysTick->LOAD = 0xFFFFFF; /* reload reg. with max val */
SysTick->CTRL = 5; /* enable it, no interrupt, use system clock */
while (1) {
c = SysTick->VAL; /* read current val of down counter */
PTB->PDOR = c >> 4; /* line up counter MSB with LED */
}
}
