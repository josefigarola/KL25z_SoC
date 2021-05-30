#include "mbed.h"
 
#define c 262
#define d 294
#define e 330
#define f 349
#define g 392
#define a 440
#define b 494
 
PwmOut buzzer(PTA5) ;
 
int main() {
    while (1) { 
        buzzer.period(1/float(c)) ;    // set PWM period
        buzzer = 0.5 ;                  // set duty cycle
        wait(0.5);
        buzzer.period(1/float(d)) ;    // set PWM period
        buzzer = 0.5 ;                  // set duty cycle
        wait(0.5);        
        buzzer.period(1/float(e)) ;    // set PWM period
        buzzer = 0.5 ;                  // set duty cycle
        wait(0.5);
        buzzer.period(1/float(f)) ;    // set PWM period
        buzzer = 0.5 ;                  // set duty cycle
        wait(0.5);
        buzzer.period(1/float(g)) ;    // set PWM period
        buzzer = 0.5 ;                  // set duty cycle
        wait(0.5);                
        buzzer.period(1/float(a)) ;    // set PWM period
        buzzer = 0.5 ;                  // set duty cycle
        wait(0.5);                
        buzzer.period(1/float(b)) ;    // set PWM period
        buzzer = 0.5 ;                  // set duty cycle
        wait(0.5);                                
        buzzer = 0;
        wait(2);     
    }
}