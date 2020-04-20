#include <avr/io.h>
#include <util/delay.h>
#include "adc.h"

#define ADC_CHANNEL
#define margin_of_error 10

#define MUX_BITS  ( (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0) )
#define ADPS_BITS  ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))

// Initialize the ADC
void adc_init(void)
{
    // Step 1: Set reference voltage
    // Set REF bits to choose AVCC to give analog range of 0V - 5V
    // ADMUX register big, REFS1 = 0, REFS0 = 1
    ADMUX &= ~ (1 << REFS1); 
    ADMUX |= (1 << REFS0);


    // Step 2: ADC Clock Generation
    // Set prescalar = 128 to reduce clock frequency
    // Turn on ADPS2, ADPS1, ADPS0 bits in ADSCRA by setting them to 1
    ADCSRA |= ( ADPS_BITS ); 


    // Step 3: Scaling (reduce accuracy to 8-bits, fits nicely in a single byte of memory)
    // Set ADLAR bit to 1 in ADMUX register (1 = 8 bit results, 0 = 1024 bit results)
    ADMUX |= (1 << ADLAR);


    // Step 4: Turn ADC on (enable it), enable bit = on/off switch
    // Set ADEN bit to 1 in ADCSRA register
    ADCSRA |= (1 << ADEN); // note: ADEN corresponds to 7th bit , so do (1 << ADEN) not (1 << 7)
}



// Convert an analog input and return the 8-bit result
unsigned char adc_sample(unsigned char channel)
{
    // Step 0: Choose which channel to use (Set the channel to sample)
    // Set ADC input mux bits to 'channel' value
    // copy channel bits to ADMUX register
    // Set MUX bits in ADMUX register to desired channel number
    // if want channel A3, set mux bits to 0011
    // set binary number corresponding to whatever number channel
    ADMUX &= ~ ( MUX_BITS);
    ADMUX |= ( channel  & MUX_BITS); // isolate lower 4 bits only 


    // Step 1: Set ADSC bit in the ADCSRA register 
    // tells ADSCRA to take a sample by setting ADSC (start bit) to 1
    ADCSRA |= (1 << ADSC); // turn ADSC bit to a 1 (tells it to go and begin sampling process)
    

    // Step 2: Wait until sampling is finished
    // sampling process takes time, so we know it's safe to collect results
    // when ADSC bit is automatically cleared to 0
    while( (ADCSRA & (1 << ADSC)) != 0){} // wait until ADSC bit = 0 = 'sampling is done'
    

    // Step 3: Collect the sample data
    // retrieve the 8-bit sample result in ADCH result
    unsigned char result = ADCH;
    return result; 
}


int check_button_press(unsigned char input, int adc1, int adc2){
    
    int pressed_1 = 0;
    int pressed_2 = 0;
    
    // check for first button
    if( (adc1 - margin_of_error <= input) && (input <= adc1 + margin_of_error)){
        _delay_ms(200);
        pressed_1 = 1;
    }

    // check for second button press
    if( (adc2 - margin_of_error <= input) && (input <= adc2 + margin_of_error)){
        _delay_ms(200);
        pressed_2 = 1;
    }

    return (pressed_1 || pressed_2);
}