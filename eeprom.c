#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include "encoder.h"
#include "musicbox.h"

#define start_addr 0

eeprom_data;

// read tune from EEPROM
// void read_tune(void){
//     return eeprom_data;
// }

// Check values read in are valid
// unsigned char check_eeprom_values(void){

// }

// // save/write tune to EEPROM (non-volatile memory)
// void write_tune(void){

// }