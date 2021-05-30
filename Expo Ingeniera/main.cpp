#include "mbed.h"

#include "TextLCD.h"
#include "Keypad.h"
#include "DHT11.h"

#define a 440
#define c 262
#define d 294
int timer; // total time inside incubator
int Hour,Min,Sec; //variables for controlling time
int ideal; //variable for controling ideal temperature for the baby
//initialize sensors and actuators
TextLCD lcd(PTD0, PTD2, PTD3, PTD4, PTD5, PTD6, PTD7); // rs, rw, e, d4-d7
Keypad Kpad(PTC5, PTC6, PTC10, PTC11, PTC7, PTC0, PTC3, PTC4); // col 1-4, row 1-4
DHT11 DHT(PTA4); //DHT11 sensor init
PwmOut buzzer(PTA12); //buzzer init
PwmOut tune(PTA5);
void LED_init(void);
//various functions
int read_flags();//function to set the state
void set_parameters(void); //menu for checking initial conditions for the baby
int save(void); //save one parameter
int isA_number(char key);
bool keyflag(char key);
void setTMP(void); //function to set ideal temperature
bool keepTemperature(bool);
bool recoverTemperature(void); //function to go back to the ideal temperature
void alert1(void);
void singleSound(void);
//time function
void disp_time(void);
void delayMs(int);

int main() {//cases with flags instead of states as interrupts
    int task;//variable for controlling code states
    bool baby = true; //digital control of having or not a baby
    bool fan = true; //flag for activate fan sequence
    while(!baby ){}//standby
    set_parameters();
    delayMs(100);
    while(baby){//set parameters and start working
        task = read_flags();
        delayMs(1000);
        disp_time();
        /*switch (task){
            case 1:
                //send data to dashboard
                //keep temperature
                fan = keepTemperature(fan);
                break;
            case 2:
                //send data and alerts
                if(!recoverTemperature()){alert1();}
                //try to return to state 1
                break;
            case 3: //flag de node mcu 
                //code for open incubator or any sensor not connected
                alert1();//alert notificate
                //no baby and reset
                baby = false;
                break;
            case 4:
                //code for baby health out of range
                //alert and notificate
                //check for vital signs
                //no baby and reset
                break;
            case 5:
                //code for system temperature out of range
                //fan or call an engineer
                //no baby and reset
                break;
            default: baby = false;
            break;
        }*/
    }
}

void LED_init(void){
    
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

int read_flags(){
    if(ideal+2>DHT.readTemperature() && ideal-2<DHT.readTemperature()){//case 1
        return 1;
    }
    if(ideal+5>DHT.readTemperature() && ideal-5<DHT.readTemperature()){//case 1
        return 2;
    }
    int data = DHT.readData();
    if(data != DHT11::OK /*||babyflag*/){ //verify connection
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("Error! DHT11 not connected\n");
        return 3;
    }
    return 0;//for safety return 0
}
void set_parameters(void){
    Hour = 0;//hour
    Min = 0;//minutes
    Sec = 0;//seconds
    char key='\0';
    while(key != '*'){//exit function all parameters are set
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("set parameters");
        delayMs(20);
        key = Kpad.ReadKey();
        delayMs(20);
        if(key == 'A'){//set time
            singleSound();
            //ask user to set ideal time
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Set the hours:");
            delayMs(100);
            Hour = save();
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Set the minutes:");
            delayMs(100);
            Min = save();
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Set the seconds:");
            delayMs(100);
            Sec = save();
            timer = Hour*3600 + Min*60 + Sec; //save total time in seconds
        }
        else if(key == 'B'){//set temperature
            singleSound();
        //ask user to set ideal temperature
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Set ideal tmp:");
            delayMs(100);
            ideal = save();
        }
        else if(key == 'C'){//health baby basic conditions
            singleSound();
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Healty baby conditions set");
            delayMs(2000);
            timer = 3*3600; //set time as 3 hours
            Hour = 3;
            Min = 0;
            Sec = 0;
            ideal = 35; //set ideal temperature as 35 C +-2
            key = '*'; //exit while
        }
        else if(key == 'D'){
            singleSound();
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Unstable baby conditions set");
            delayMs(2000);
            timer = 3600;
            ideal = 35; // +- 1
            key = '*'; //exit while
        }
    }
}

int save(void){
    int temp = 0;
    int pos = 0;
    char key = '\0';
    int num = -1;
    while(key != '#' && keyflag(key)){//save parameter
        delayMs(50);
        key = Kpad.ReadKey();
        delayMs(20);
        num = isA_number(key);
        delayMs(50);
        if(num != -1 && keyflag(key)){
            singleSound();
            temp = temp*10 + num;
            lcd.locate(pos,1);
            lcd.printf("%d \n", num); //display the seconds
            pos += 1; //get to the next position
        }
    }
    return temp;
}

bool keepTemperature(bool flag){
    for(int j=0; j<15; j++){//every 15 sec fan or led will turn on
        if(ideal+2<=DHT.readTemperature()){ //heat secuence
            PTB->PCOR |= 0x40000; /* turn on red LED */
            delayMs(1000); //delay one second
            disp_time();
            PTB->PSOR |= 0x40000; /* turn off red LED */
        }
        if(ideal-2>=DHT.readTemperature()){//cool secuence
            //myfan.speed(1.0); //fan at medium capacity
            delayMs(1000); //delay one second
            disp_time();
            //myfan.speed(0.0); //stop fan
        }
    }
    return !flag; 
}

bool recoverTemperature(){
    for(int i=0; i<2; i++){
        if(ideal+1<=DHT.readTemperature() || ideal-1>=DHT.readTemperature()){
            return true;
        }
        for(int j=0; j<60; j++){
            if(ideal+1<=DHT.readTemperature()){ //heat secuence
                PTB->PCOR |= 0x40000; /* turn on red LED */
                delayMs(1000); //delay one second
                disp_time();
                PTB->PSOR |= 0x40000; /* turn off red LED */
            }
            if(ideal-1>=DHT.readTemperature()){//cool secuence
                //myfan.speed(1.0); //fan at medium capacity
                delayMs(1000); //delay one second
                disp_time();
               //myfan.speed(0.0); //stop fan
            }
        }
    } 
    return false;          
}
            
void disp_time(void){
    if(timer-1 < 0){return;} //flag for security
    if(Sec-1 < 0){
        if(Min-1 < 0){
            if(Hour-1 >= 0){
                Hour -= 1;
                Min = 60;
            }
        }
        Min -= 1;
        Sec = 60;
    }
    Sec -= 1;
    timer -= 1;
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Time left");
    lcd.locate(0,1);
    lcd.printf("%d:%d:%d",Hour,Min,Sec);
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

void alert1(void){//activate alarm
    buzzer.period(1/float(c));    // set PWM period
    buzzer = 0.5;                  // set duty cycle
    wait(1.0);
    disp_time();
    buzzer.period(1/float(d));    // set PWM period      
    buzzer = 0.5;                  // set duty cycle
    wait(1.0);
    disp_time();  
    buzzer = 0;
}       

void singleSound(void){
    tune.period(1/float(a));    // set PWM period      
    tune = 0.5;                  // set duty cycle
    wait(0.5);  
    tune = 0;
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
