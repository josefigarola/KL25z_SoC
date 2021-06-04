/* Generate 60Hz 33% PWM output
* TPM0 uses MCGFLLCLK which is 41.94 MHz.

* The prescaler is set to divide by 16.

* The modulo register is set to 43702 and the CnV

* register is set to 14568. See Example 3 for

* the calculations of these values.*/

#include "mbed.h"

int main (void)
{
    SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
    PORTD->PCR[1] = 0x0400; /* PTD1 used by TPM0 */
    SIM->SCGC6 |= 0x01000000; /* enable clock to TPM0 */
    SIM->SOPT2 |= 0x01000000; /* use MCGFLLCLK as timer counter clock */

    TPM0->SC = 0; /* disable timer */
    /* edge-aligned, pulse high */
    TPM0->CONTROLS[1].CnSC = 0x20 | 0x08;
    /* Set up modulo register for 60 kHz */
    TPM0->MOD = 43702;
    TPM0->CONTROLS[1].CnV = 0;
    //0-0
    //25-10925
    //50-21851
    //75-32777
    //100-43700
    /* Set up channel value for 25% dutycycle */
    TPM0->SC = 0x0C; /* enable TPM0 with prescaler /16 */
    while (1) { }
}


