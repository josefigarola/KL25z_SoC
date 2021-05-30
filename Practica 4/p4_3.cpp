//implementacion codigo para la parte 3 practica 4
#include "mbed.h"
//librerias de apoyo
#include "TextLCD.h" //peter dresherd 2010 RW
#include "Keypad.h" //grant phillips 
#define c 262
#define d 294
//interrupciones
int IRQ; //global variable
InterruptIn pause(PTA1,PullUp);//pausa
InterruptIn rset(PTA2,PullUp);//reset global
void PORTA_IRQHandler(void);
void interrupt(void);
//inicializacion de actuadores
TextLCD lcd(PTD0, PTD2, PTD3, PTD4, PTD5, PTD6, PTD7); // rs, rw, e, d4-d7
Keypad Kpad(PTC4, PTC3, PTC0, PTC7, PTC11, PTC10, PTC6, PTC5); // col 1-4, row 1-4
PwmOut buzzer(PTA5); //buzzer init
void LED_init(void);
//funciones varias
void main_code(void);
int isA_number(char key); //is my key a numer or not
bool keypress(char key); //check if a key has been pressed
char key; //value from the keyboard
void no_flip(void);
//time functions
void counter(int n); //contador
void delayMs(int n);

int main()
{
    LED_init();
    //initialize both buttons
    pause.rise(&PORTA_IRQHandler);  // attach the function to the raising edge
    pause.fall(&no_flip);  // attach function to the falling edge

    rset.rise(&PORTA_IRQHandler);  // attach the function to the raising edge
    rset.fall(&no_flip);  // attach function to the falling edge

    IRQ=0;
    main_code();
}

void main_code(void)
{
    int num; //value from the keyboard
    int cont; //total amount of seconds of delay
    int pos; //pos for the cont
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
        PTB->PCOR = 0x40000; //turn on red led
        buzzer.period(1/float(c));    // set PWM period
        buzzer = 0.5;                  // set duty cycle
        wait(1.0);
        buzzer.period(1/float(d));    // set PWM period
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("Time's up");
        buzzer = 0.5;                  // set duty cycle
        wait(1.0);
        delayMs(1000);
        PTB->PSOR = 0x40000; // turn off red led
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
        PTD->PCOR = 0x02; //turn on blue led
        delayMs(800); //stop one second
        PTD->PSOR |= 0x02; /* turn off blue LED */
        delayMs(200);
        n -= 1;
    }

}

void PORTA_IRQHandler(void)
{
    IRQ=1;
    if(pause.read() == 1) {
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("PAUSED");
        while(key != '*' && keypress(key)) {
            key = Kpad.ReadKey();
        }
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("Counting...");
        //clear interrupt flag
        IRQ=0;
    }
    if(rset.read() == 1) {
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("reset");
        //clear interrupt flag
        IRQ=0;
        main_code();//reset
    }
}

void no_flip(void)
{
    IRQ = 0;
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
        {
        }
    }
    SysTick->CTRL = 0;
    /* Stop the timer (Enable = 0) */
}