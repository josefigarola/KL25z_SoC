// Code implementation for part 3, we tried to add buzzers as alerts to the system
// but there is a signal conflict
// IRS tim 

#include "mbed.h"

#include "TextLCD.h"
#include "Keypad.h"

//initialize sensors and actuators
TextLCD lcd(PTD0, PTD2, PTD3, PTD4, PTD5, PTD6, PTD7); // rs, rw, e, d4-d7
Keypad Kpad(PTC5, PTC6, PTC10, PTC11, PTC7, PTC0, PTC3, PTC4); // col 1-4, row 1-4
void LED_init(void);
//buttons_init
DigitalIn A(PTA1); //call for an alert and stop the system
bool pause = false; //flag for pausing the system 
DigitalIn S(PTA2); //Set speed
//various functions
void Set_mode(void);
void Set_Duty_Cycle(char);
bool keyflag(char);
int valid_option(int);
//time function
void delayMs(int n);
//variables
char key;
char mode;
int num;
int main(){
    LED_init();
    lcd.cls();
    Set_mode();
    delayMs(20);
    while(1){
         if(A){
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Continue?");
            delayMs(20);
            while(key != '*' && keyflag(key)){
                key = Kpad.ReadKey();
                delayMs(20);
            }
            mode = 0;
            lcd.cls();
            Set_mode();
            delayMs(20);
            pause = false;
        }
        while(!pause && mode!=0){
            Set_Duty_Cycle(mode);
        }
    }
}

void LED_init(void){ // initialize for blue led for this code.
    
    SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
    PORTD->PCR[1] = 0x0400; /* PTD1 used by TPM0 */
    SIM->SCGC6 |= 0x01000000; /* enable clock to TPM0 */
    SIM->SOPT2 |= 0x01000000; /* use MCGFLLCLK as timer counter clock */
    
    TPM0->SC = 0; /* disable timer */
    /* edge-aligned, pulse high */
    TPM0->CONTROLS[1].CnSC = 0x20 | 0x08; 
    /* Set up modulo register for 60 kHz */
    TPM0->MOD = 43702; 
    TPM0->CONTROLS[1].CnV = 43702; 
    /* Set up channel value for 100% which is 0 dutycycle */
    TPM0->SC = 0x0C; /* enable TPM0 with prescaler /16 */
}

void Set_mode(void){
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Set Mode:");
    delayMs(20);
    lcd.locate(0,1);
    lcd.printf("1:M 2:A");
    delayMs(20);
    while(key != '#' && keyflag(key)){//set mode
    key = Kpad.ReadKey();
    delayMs(20);
    num = key - '0';
        if(key == '1'){
            lcd.locate(9,0);
            lcd.printf("M"); //display the mode
            mode = '1';
        }
         if(key == '2'){
            lcd.locate(9,0);
            lcd.printf("A"); //display the mode
            mode = '2';
        }
    }
}

void Set_Duty_Cycle(char m){
    int pulseWidth = 0;
    if(m == '1'){
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("Select Speed:");
        delayMs(20);
        lcd.locate(0,1);
        lcd.printf("1:L 2:M 3:MH 4:H");
        delayMs(20);
        while(!S){
            key = Kpad.ReadKey();
            delayMs(20);
            num = key - '0';
            if(valid_option(num)!=0){
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf("Mode: %d",num);
                delayMs(20);
                TPM0->CONTROLS[1].CnV = 43702-10925*num; //set new cycle 
            }
            if(A){
                pause = true;
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf("Paused");
                delayMs(1000);
                TPM0->CONTROLS[1].CnV = 43702;
                return;
            }
        }
        return;
    }
    if(m == '2'){
        while(!S) {
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Automatic Mode");
            delayMs(20);
            if(A){
                pause = true;
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf("Paused");
                delayMs(1000);
                TPM0->CONTROLS[1].CnV = 43702;
                return;
            }
            pulseWidth += 437;
            if (pulseWidth > 43702)
                pulseWidth = 0;
            TPM0->CONTROLS[1].CnV = pulseWidth;
            delayMs(20);
        }
        return;
    }   
    return;
}

int valid_option(int n){
    //check if a valid option has been entered
    for( int i=1; i<=4; i++) {
        if(n == i){
            return i;
        }
    }
    //if we get here no valid option has been pressed
    return 0; //return 0 in order to avoid an error
}

bool keyflag(char key)
{
    //check if a key has been pressed this function will be used as a flag
    return key != '/0';
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