#include "mbed.h"

DigitalOut myled(LED1); // Alert LED
DigitalIn PIR(PTC7); // PIR Signal   //C8

int main()
{
    int PIR_sensor;   // declare variable for PIR input data
    while(1) {
        PIR_sensor = PIR; // Check input signal from PIR pin
        //pc.printf("PIR Value : %d\r\n",PIR_sensor);
        myled =! PIR_sensor; // PIR(HIGH) == LED On // PIR(LOW) == LED Off
        wait(0.1);
    }
}

