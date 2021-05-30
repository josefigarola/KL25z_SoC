 #include "mbed.h"
 #include "TextLCD.h"
 
 TextLCD lcd(PTD0, PTD2, PTD4, PTD5, PTD6, PTD7); // rs, e, d4-d7
 void delayMs(int n); //delay milisec
 void delayUs(int n); //delay microsec
 void keypad_init(void);
 char keypad_getkey(void); //get value
 void LED_init(void);
 void LED_set(int value); //decoder
 
 int main() {
        unsigned char key; 
        
        while(1){    
            lcd.locate(2,0);
            lcd.printf("Press Button");
            lcd.locate(2,1);
            lcd.printf("R:1 G:2 B:3");
            LED_init();
            keypad_init();
            key = keypad_getkey();
            LED_set(key); //set LEDs according to the key code 
        }  
 }
 
 void keypad_init(void)
{
    SIM->SCGC5 |= 0x0800;  /* enable clock to Port C */
    PORTC->PCR[0] = 0x103; /* PTD0, GPIO, enable pullup*/
    PORTC->PCR[1] = 0x103; /* PTD1, GPIO, enable pullup*/
    PORTC->PCR[2] = 0x103; /* PTD2, GPIO, enable pullup*/
    PORTC->PCR[3] = 0x103; /* PTD3, GPIO, enable pullup*/
    PORTC->PCR[4] = 0x103; /* PTD4, GPIO, enable pullup*/
    PORTC->PCR[5] = 0x103; /* PTD5, GPIO, enable pullup*/
    PORTC->PCR[6] = 0x103; /* PTD6, GPIO, enable pullup*/
    PORTC->PCR[7] = 0x103; /* PTD7, GPIO, enable pullup*/
    PTD->PDDR = 0x0F; /* make PTD7-0 as input pins */
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

char keypad_getkey(void)
{

    int row, col;
    const char row_select[] = {0x01, 0x02, 0x04, 0x08};
    /* one row is active
    check to see any key pressed */

    PTC->PDDR |= 0x0F; /* enable all rows */
    PTC->PCOR = 0x0F;
    delayUs(2); /* wait for signal return */
    col = PTC-> PDIR & 0xF0; /* read all columns */
    PTC->PDDR = 0; /* disable all rows */
    if (col == 0xF0)
        return 0; /* no key pressed */
    /* If a key is pressed, we need find out which key.*/
    for (row = 0; row < 4; row++) {
        PTC->PDDR = 0; /* disable all rows */

        PTC->PDDR |= row_select[row]; /* enable one row */
        PTC->PCOR = row_select[row]; /* drive active row low*/

        delayUs(2); /* wait for signal to settle */
        col = PTC->PDIR & 0xF0; /* read all columns */

        if (col != 0xF0) break;
        /* if one of the input is low, some key is pressed. */
    }
    PTC->PDDR = 0; /* disable all rows */

    if (row == 4)
        return 0; /* if we get here, no key is pressed */

    /* gets here when one of the rows has key pressed
    check which column it is*/

    if (col == 0xE0) return row * 4 + 1; /* key in column 0 */
    if (col == 0xD0) return row * 4 + 2; /* key in column 1 */
    if (col == 0xB0) return row * 4 + 3; /* key in column 2 */
    if (col == 0x70) return row * 4 + 4; /* key in column 3 */
    return 0; /* just to be safe */
}

void LED_set(int value)
{
    /* use cases to control leds */
    switch(value) {
        case 1:  
            //lcd.printf("SIUUU");
            PTB->PCOR = 0x40000;  /* turn on red LED */
            delayUs(2000);
            PTB->PSOR = 0x40000; /* turn off red LED */
            break;
        case 2:
            PTB->PCOR = 0x80000;  /* turn on green LED */
            delayUs(1000);
            PTB->PSOR = 0x80000; /* turn off green LED */
            break;
        case 3:
            PTD->PCOR = 0x02;  /* turn on blue LED */
            delayUs(1000);
            PTD->PSOR = 0x02; /* turn off blue LED */
            break;
        case 4:
            PTB->PCOR = 0x40000; //turn on yellow led
            PTB->PCOR = 0x80000;
            delayUs(1000);
            PTB->PSOR = 0x40000; //turn off yellow led
            PTB->PSOR = 0x80000;
            break;
        case 5:
            PTB->PCOR = 0x80000; //turn on cyan led
            PTD->PCOR = 0x2;
            delayUs(1000);
            PTB->PSOR = 0x80000; //turn off cyan led
            PTD->PSOR = 0x2;
            break;
        case 6:
            PTB->PCOR = 0x40000; //turn on magenta led
            PTD->PCOR = 0x2;
            delayUs(1000);
            PTB->PSOR = 0x40000; //turn off magenta led
            PTD->PSOR = 0x2;
            break;
        case 7:
            PTB->PCOR = 0x80000; //turn on white led
            PTD->PCOR = 0x2;
            PTB->PCOR = 0x40000;
            delayUs(1000);
            PTB->PSOR = 0x40000; //turn off white led
            PTB->PSOR = 0x80000;
            PTD->PSOR = 0x2;
            break;
        case 8:
            PTB->PCOR = 0x40000; //turn on magenta led
            PTD->PCOR = 0x2;
            delayUs(1000);
            PTB->PSOR = 0x40000; //turn off magenta led
            PTD->PSOR = 0x2;
            break;
        case 9:
            PTB->PCOR = 0x80000; //turn on cyan led
            PTD->PCOR = 0x2;
            delayUs(1000);
            PTB->PSOR = 0x80000; //turn off cyan led
            PTD->PSOR = 0x2;
            break;
        case 10:
            PTB->PCOR = 0x40000; //turn on yellow led
            PTB->PCOR = 0x80000;
            delayUs(1000);
            PTB->PSOR = 0x40000; //turn off yellow led
            PTB->PSOR = 0x80000;
            break;
        case 11:
            PTD->PCOR = 0x02;  /* turn on blue LED */
            delayUs(1000);
            PTD->PSOR = 0x02; /* turn off blue LED */
            break;
        case 12:
            PTB->PCOR = 0x80000;  /* turn on green LED */
            delayUs(1000);
            PTB->PSOR = 0x80000; /* turn off green LED */
            break;
        case 13:
            PTB->PCOR = 0x40000;  /* turn on red LED */
            delayUs(1000);
            PTB->PSOR = 0x40000; /* turn off red LED */
            break;
        case 14:
            PTB->PCOR = 0x80000;  /* turn on green LED */
            delayUs(1000);
            PTB->PSOR = 0x80000; /* turn off green LED */
            break;
        case 15:
            PTD->PCOR = 0x02;  /* turn on blue LED */
            delayUs(1000);
            PTD->PSOR = 0x02; /* turn off blue LED */
            break;
        case 16:
            PTB->PCOR = 0x40000; //turn on yellow led
            PTB->PCOR = 0x80000;
            delayUs(1000);
            PTB->PSOR = 0x40000; //turn off yellow led
            PTB->PSOR = 0x80000;
            break;
        default:
            PTB->PSOR = 0x40000; //turn off white led
            PTB->PSOR = 0x80000;
            PTD->PSOR = 0x2;
            break;
    }
}
 
 /* Delay n milliseconds
The CPU core clock is set to MCGFLLCLK at
41.94 MHz in SystemInit(). */
void delayMs(int n)
{
    int i;
    int j;
    for(i = 0 ; i < n; i++)
        for(j = 0 ; j < 7000; j++) { }
}

/* Delay n milliseconds
The CPU core clock is set to MCGFLLCLK at
41.94 MHz in SystemInit(). */
void delayUs(int n)
{
    int i;
    int j;
    for(i = 0 ; i < n; i++)
        for(j = 0 ; j < 7000; j++) { }
}