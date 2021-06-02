//code implementation for part 2
#include "mbed.h"

#include "TextLCD.h" //library to lcd with rw
#include "Keypad.h" //library to keypad
#include "DHT11.h" //library to control tmp sensor 2018
#include "DcFan.h" //library to control fan cooler

#define c 262
#define d 294
//buttons_init
DigitalIn B(PTA1);
DigitalIn G(PTA2);

//initialize sensors and actuators
TextLCD lcd(PTD0, PTD2, PTD3, PTD4, PTD5, PTD6, PTD7); // rs, rw, e, d4-d7
Keypad Kpad(PTC5, PTC6, PTC10, PTC11, PTC7, PTC0, PTC3, PTC4); // col 1-4, row 1-4
DHT11 DHT(PTA4); //DHT11 sensor init
DcFan myfan(PTA13, 1.0); //fan init port and activepwm
PwmOut buzzer(PTA12); //buzzer init
void LED_init(void);
//various functions
int isA_number(char key);
bool keyflag(char key);
void setTMP(void);
void alert1(void);
//time functions
void timer(int count,char type);
void delayMs(int n);

int data; //global variable for sensor data
int temp; //global variable for ideal temperature
char key; //value from the keyboard
int num; //value from the keyboard
int pos; //pos for the cont

int main() {
    LED_init();
   
    while(1){
        //reset all variables
        lcd.cls();
        pos = 0;
        temp = 0;
        key = '\0';
        setTMP(); //call the function for setting temperature
        delayMs(500);
        data = DHT.readData();
        if(data != DHT11::OK){ //verify connection
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Error! \r\n");
        }
        else{//chek ideal temp in a range of -+5 degrees
            while(temp+5>DHT.readTemperature() && temp-5<DHT.readTemperature()){
                lcd.cls();
                delayMs(20);
                lcd.locate(0,0);
                lcd.printf("T:%d H:%d",DHT.readTemperature(),DHT.readHumidity());
                delayMs(2000);
                timer(15,'t'); //wait one minute to activate fan (15 sec to demonstration)
                delayMs(500);
                if(G){//set ideal temperature, we should go out this loop
                    break;
                }
                else if(B){//no fan
                    lcd.cls();
                    delayMs(20);
                    lcd.locate(0,0);
                    lcd.printf("T:%d H:%d",DHT.readTemperature(),DHT.readHumidity());
                    delayMs(5000);
                }else{//keep temperature
                    PTB->PCOR |= 0x40000; /* turn on red LED */
                    myfan.speed(1.0); //fan at medium capacity
                    timer(5,'c'); //set cool for 5 sec
                    myfan.speed(0.0); //stop fan
                    PTB->PSOR |= 0x40000; /* turn off red LED */
                }
            }//we get out if temperature read by sensor is 5degrees over ideal temp
            alert1();//temperature out of range
            //cool until we get back in the range of ideal temp
            while(temp+5<=DHT.readTemperature() || temp-5>=DHT.readTemperature()){
                if(G){//set ideal temperature, we should go out this loop
                    break;
                }
                else if(B){
                    lcd.cls();
                    lcd.locate(0,0);
                    lcd.printf("T:%d H:%d",DHT.readTemperature(),DHT.readHumidity());
                    delayMs(2000);
                }else{
                    if(temp+5<=DHT.readTemperature()){ //heat secuence
                            PTB->PCOR |= 0x40000; /* turn on red LED */
                            timer(15,'h'); //set heat for 15 sec demonstration purposes
                            PTB->PSOR |= 0x40000; /* turn off red LED */
                    }
                    if(temp-5>=DHT.readTemperature()){//cool secuence
                        myfan.speed(1.0); //fan at medium capacity
                        timer(15,'c'); //set cool for 15 sec
                        myfan.speed(0.0); //stop fan
                    }
                }//else
            }//delicate
        }//sensor data fine
    } //go back no normal mode, first checking if sensor is fine      
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

void setTMP(void){
     //ask user to set ideal temperature
    lcd.locate(0,0);
    lcd.printf("Set the desired temperature:");
    delayMs(20);
     while(key != '#'){ //set # as enter key
        delayMs(50);
        key = Kpad.ReadKey();
        delayMs(20);
        num = isA_number(key);
        delayMs(20);
        //we do this code only if there is a numerical char
        if(num != -1 && keyflag(key)) { //condition + flag
            temp = temp*10 + num;
            lcd.locate(12+pos,1);
            lcd.printf("%d \n", num); //display the seconds
            pos += 1; //get to the next position
        }
    }//we have now set the ideal temperature
}

void alert1(void){//activate alarm
    buzzer.period(1/float(c));    // set PWM period
    buzzer = 0.5;                  // set duty cycle
    wait(1.0);
    buzzer.period(1/float(d));    // set PWM period      
    buzzer = 0.5;                  // set duty cycle
    wait(1.0);  
    buzzer = 0;
}       

bool keyflag(char key)
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

void timer(int count,char type){
    lcd.cls();
    lcd.locate(0,0);
    if(type == 't'){
        lcd.printf("counting...");
        while(count != 0) {
            lcd.locate(0,1);
            lcd.printf("%d \n", count);
            PTB->PCOR |= 0x80000; /* turn off green LED */
            delayMs(800); //stop one second
            PTB->PSOR |= 0x80000; /* turn off green LED */
            delayMs(200);
            count -= 1;
        }
    }
    if(type == 'c'){
        lcd.printf("Cooling time:");
        while(count != 0) {
            lcd.locate(13,0);
            lcd.printf("%d \n", count);
            lcd.locate(0,1);
            lcd.printf("Ideal:%d Read:%d",temp,DHT.readTemperature());
            PTD->PCOR = 0x02; //turn on blue led
            delayMs(800); //stop one second
            PTD->PSOR |= 0x02; /* turn off blue LED */
            delayMs(200);
            count -= 1;
        }
    }
    if(type == 'h'){
        lcd.printf("Heating time:");
        while(count != 0) {
            lcd.locate(13,0);
            lcd.printf("%d \n", count);
            lcd.locate(0,1);
            lcd.printf("Ideal:%d Read:%d",temp,DHT.readTemperature());
            delayMs(1000); //stop one second
            count -= 1;
        }
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