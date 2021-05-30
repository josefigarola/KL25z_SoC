/* Initialize and display “hello” on the LCD using 4-bit data mode.
* All interface uses Port D. Bit 7-4 are used for data.
* Bit 4, 2, 1 are used for control.
* This program does not poll the status of the LCD.
* It uses delay to wait out the time LCD controller is busy.
* Timing is more relax than the HD44780 datasheet to accommodate the
* variations of the devices.
* You may want to adjust the amount of delay for your LCD controller.
*/
#include <MKL25Z4.H>

#define RS 1 /* BIT0 mask */
#define RW 2 /* BIT1 mask */
#define EN 4 /* BIT2 mask */

void delayMs(int n);
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
int main(void)
{
LCD_init();
delayMs(1000);
for(;;)
	{
	LCD_command(1); /* clear display */
	delayMs(2000);
	LCD_command(0x80); /* set cursor at first line */
	LCD_data('H'); /* write the word */
	delayMs(200);
	LCD_data('e');
	delayMs(200);
	LCD_data('l');
	delayMs(200);
	LCD_data('l');
	delayMs(200);
	LCD_data('o');
	delayMs(200);
	LCD_command(0xC0);
	LCD_data('W');
	delayMs(200);
	LCD_data('o');
	delayMs(200);
	LCD_data('r');
	delayMs(200);
	LCD_data('l');
	delayMs(200);
	LCD_data('d');
	delayMs(500);
	}
}
void LCD_init(void)
{
SIM->SCGC5 |= 0x1000; /* enable clock to Port D */

PORTD->PCR[0] = 0x100; /* make PTD0 pin as GPIO */
PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
PORTD->PCR[2] = 0x100; /* make PTD2 pin as GPIO */
PORTD->PCR[4] = 0x100; /* make PTD4 pin as GPIO */
PORTD->PCR[5] = 0x100; /* make PTD5 pin as GPIO */
PORTD->PCR[6] = 0x100; /* make PTD6 pin as GPIO */
PORTD->PCR[7] = 0x100; /* make PTD7 pin as GPIO */

PTD->PDDR |= 0xF7; /* make PTD7-4, 2, 1, 0 as output pins */
	
delayMs(30); /* initialization sequence */
LCD_nibble_write(0x30, 0);
delayMs(20);
LCD_nibble_write(0x30, 0);
delayMs(20);

LCD_nibble_write(0x30, 0);
delayMs(20);
LCD_nibble_write(0x20, 0); /* use 4-bit data mode */
delayMs(20);
LCD_command(0x28); /* set 4-bit D, 2-line, 5x7 font */
LCD_command(0x06); /* move cursor right */
LCD_command(0x01); /* clr screen, move cursor home */
LCD_command(0x0F); /* turn on display, cursor blink*/
}
void LCD_nibble_write(unsigned char data, unsigned char control)
{
data &= 0xF0; /* clear lower nibble for control */

control &= 0x0F; /* clear upper nibble for data */

PTD->PDOR = data | control; /* RS = 0, R/W = 0 */
PTD->PDOR = data | control | EN; /* pulse E */

delayMs(10);
PTD->PDOR = data;
PTD->PDOR = 0;
}
void LCD_command(unsigned char command)
{
/* upper nibble first */

LCD_nibble_write(command & 0xF0, 0); 

/* then lower nibble */

LCD_nibble_write(command << 4, 0); 

if (command < 4)

delayMs(4); /* commands 1 and 2 need up to 1.64ms */
else
delayMs(40); /* all others 40 us */
}
void LCD_data(unsigned char data)
{/* upper nibble first */
LCD_nibble_write(data & 0xF0, RS);
/* then lower nibble */ 
LCD_nibble_write(data << 4, RS); 
delayMs(1);
}
/* Delay n milliseconds
The CPU core clock is set to MCGFLLCLK at 
41.94 MHz in SystemInit(). */
void delayMs(int n) {
int i;
int j;
for(i = 0 ; i < n; i++)
for(j = 0 ; j < 7000; j++) { }
}
