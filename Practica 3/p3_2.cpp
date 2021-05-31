/* Implementacion para la parte 1 de la practica 3, segun la tecla que presionemos en
el teclado, se desplegara a la LCD un valor unico segun nuestro decoder*/

/* Miguel Figarola A01632557
Giancarlo Franco A01638108
Diego Limon A01638247*/

#include "mbed.h"
#include "TextLCD.h"
#include "Keypad.h"
#define c 262
#define d 494

TextLCD lcd(PTD0, PTD2, PTD3, PTD4, PTD5, PTD6, PTD7); // rs, rw, e, d4-d7
Keypad Kpad(PTC4, PTC3, PTC0, PTC7, PTC11, PTC10, PTC6, PTC5); // col 1-4, row 1-4
PwmOut buzzer(PTA5); //Set buzzer with pwm for different frequencies

void LED_init(void);
void counter(int n); //contador
int isA_number(char key); //is my key a numer or not
void delayMs(int n); //delay milisec with internal clock
bool keypress(char key); //check if a key has been pressed

int main()
{
    char key; //value from the keyboard
    int num; //value from the keyboard
    int cont; //total amount of seconds of delay
    int pos; //pos for the cont
    LED_init();
    while(1) {
        lcd.locate(0,0);
        lcd.printf("Hello");
        delayMs(5000);
        lcd.cls();
        pos = 0;
        cont = 0;
        key = '*'; //reset all variables
        lcd.locate(0,0);
        lcd.printf("Introduce");
        lcd.locate(0,1);
        lcd.printf("Seconds: ");
        while(key != '#') {
            key = Kpad.ReadKey();
            num = isA_number(key);
            delayMs(200);
            //we do this code only if there is a numerical char
            if(num != -1 && keypress(key)) { //condition + flag
                cont = cont*10 + num;
                lcd.locate(8+pos,1);
                lcd.printf("%d \n", num); //display the seconds
                pos += 1; //get to the next position
            }
        }//we get out of the cicle when user press # or *
        //once here we call our timer
        counter(cont);
        PTB->PCOR = 0x40000; /*turn on red led*/
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("Time's up");
        buzzer.period(1/float(c)); // set PWM period
        buzzer = 0.5;// set duty cycle
        wait(0.5);
        buzzer.period(1/float(d));// set PWM period
        buzzer = 0.5;// set duty cycle
        wait(0.5);
        delayMs(2000);
        PTB->PSOR = 0x40000; /*turn off red led*/
        lcd.cls();
        buzzer = 0;
        //we go back again
    }
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

bool keypress(char key)
{
    //check if a key has been pressed this function will be used as a flag
    return key != '/0';
}

int isA_number(char key)
{
    //check if a numerical key has been pressed
    for( int i=0; i<=9; i++) {
        if((key-'0') == i) {
            return i;
        }
    }
    //if we get here no numerical key has been pressed
    return -1; //return -1 in order to avoid an error
}

void counter(int n)
{

    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Counting...");

    while(n != 0) {
        lcd.locate(0,1);
        lcd.printf("%d \n", n);
        PTD->PCOR = 0x02; /*turn on blue led*/
        delayMs(800); //stop one second
        PTD->PSOR |= 0x02; /* turn off blue LED */
        delayMs(200);
        n -= 1;
    }

}

/* Delay n milliseconds
The CPU core clock is set to MCGFLLCLK at
41.94 MHz in SystemInit(). */
void delayMs(int n)
{
    int i;
    SysTick->LOAD = 41940 - 1;
    SysTick->CTRL = 0x5; /* Enable the timer and choose sysclk as the clock source */

    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0)
            /* wait until the COUTN flag is set */
        { }
    }
    SysTick->CTRL = 0;
    /* Stop the timer (Enable = 0) */
}
