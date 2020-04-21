/*
  lcd.c - Routines for sending data and commands to the LCD shield
*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "lcd.h"                // Declarations of the LCD functions
#include "musicbox.h"
#include "display.h"

/* This function not declared in lcd.h since
   should only be used by the routines in this file. */
void lcd_writenibble(unsigned char);

/* Define a couple of masks for the bits in Port B and Port D */
#define DATA_BITS ((1 << PD7)|(1 << PD6)|(1 << PD5)|(1 << PD4))
#define CTRL_BITS ((1 << PB1)|(1 << PB0))

/*
  lcd_init - Do various things to initialize the LCD display
*/
void lcd_init(void)
{
    DDRB |= (CTRL_BITS);        // Set the DDR register bits for ports B and D
    DDRD |= (DATA_BITS);        // Take care not to affect any unnecessary bits

    _delay_ms(15);              // Delay at least 15ms

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b0011
    _delay_ms(5);               // Delay at least 4msec

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b0011
    _delay_us(120);             // Delay at least 100usec

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b0011, no delay needed

    lcd_writenibble(0x20);      // Use lcd_writenibble to send 0b0010
    _delay_ms(2);               // Delay at least 2ms
    
    lcd_writecommand(0x28);     // Function Set: 4-bit interface, 2 lines

    lcd_writecommand(0x0f);     // Display and cursor on
}

/*
  lcd_moveto - Move the cursor to the row and column given by the arguments.
  Row is 0 or 1, column is 0 - 15.
*/
void lcd_moveto(unsigned char row, unsigned char col)
{
    unsigned char pos;
    if(row == 0) {
        pos = 0x80 + col;       // 1st row locations start at 0x80
    }
    else {
        pos = 0xc0 + col;       // 2nd row locations start at 0xc0
    }
    lcd_writecommand(pos);      // Send command
}

/*
  lcd_stringout - Print the contents of the character string "str"
  at the current cursor position.
*/
void lcd_stringout(char *str)
{
    int i = 0;
    while (str[i] != '\0') {    // Loop until next charater is NULL byte
        lcd_writedata(str[i]);  // Send the character
        i++;
    }
}

/*
  lcd_writecommand - Output a byte to the LCD command register.
*/
void lcd_writecommand(unsigned char cmd)
{
    /* Clear PB0 to 0 for a command transfer */
    PORTB &= ~(1 << 0); // set RS (Register Select) to 0 for command

    /* Call lcd_writenibble to send UPPER four bits of "cmd" argument */
    lcd_writenibble(cmd);

    /* Call lcd_writenibble to send LOWER four bits of "cmd" argument */
    lcd_writenibble(cmd << 4);

    /* Delay 2ms */
    _delay_ms(2); 

}

/*
  lcd_writedata - Output a byte to the LCD data register
*/
void lcd_writedata(unsigned char dat)
{
    /* Set PB0 to 1 for a data transfer */
    PORTB |= (1 << 0); // set RS (Register Select) to 1 for data

    /* Call lcd_writenibble to send UPPER four bits of "dat" argument */
    lcd_writenibble(dat);

    /* Call lcd_writenibble to send LOWER four bits of "dat" argument */
    lcd_writenibble(dat << 4);

    /* Delay 2ms */
    _delay_ms(2); 
}

/*
  lcd_writenibble - Output the UPPER four bits of "lcdbits" to the LCD
*/
void lcd_writenibble(unsigned char lcdbits)
{
    /* Load PORTD, bits 7-4 with bits 7-4 of "lcdbits" */
    PORTD &= ~(DATA_BITS);  // clear bits
    PORTD |= (lcdbits & DATA_BITS); // copy bits over

    /* Make E signal (PB1) go to 1 and back to 0 */
    PORTB |=  (1 << 1); // go to 1
    PORTB |=  (1 << 1); // go to 1 again (extend pulse)
    PORTB &= ~(1 << 1); // go to 0 (clear E signal)
}


void splash_screen(void){
    lcd_writecommand(1);// Clear screen and move to row 0, column 0

    /* Call lcd_stringout to print out your name */
    lcd_stringout("Jenna");

    /* Use snprintf to create a string with your birthdate */
    char bday[10];
    snprintf(bday, 10, "%d/%d/%d", 3, 22, 2000);

    /* Use lcd_moveto to start at an appropriate column 
        in the bottom row to appear centered */
    lcd_moveto(1,3);

    /* Use lcd_stringout to print the birthdate string */
    lcd_stringout(bday);

    _delay_ms(1000); // delay 1 second
}

// update note on rotary encoder change
void update_note(void){
    lcd_moveto(0, curr_col);
    _delay_ms(100);
    lcd_stringout(note_str[count]);
}
// update octave on rotary encoder change
void update_octave(void){
  lcd_moveto(1, curr_col);
  _delay_ms(100);
  display_octave(count, curr_col);
}