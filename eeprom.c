#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include "encoder.h"
#include "musicbox.h"


// check data read in from eeprom is between 0 & 25
unsigned char check_eeprom_data_valid(void){
    unsigned char eeprom_valid = 1;
    for(int i = 0; i < 21; i++){
      unsigned char data = eeprom_data[i];
      if(data > 25){
        eeprom_valid = 0;
        break;
      }
    }
    return eeprom_valid;
}

// set notes array = eeprom_data array
void update_notes(void){
    for(int i = 0; i < 21; i++){
        notes[i] = eeprom_data[i];        
    }
}