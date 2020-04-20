
#include "musicbox.h"
#include "display.h"

/*
  Code for showing notes on the screen and playing the notes.
*/

// display all notes on a single screen
void display_notes_and_octaves(void)
{
  lcd_moveto(0, 1);               // display on 1st row w/ cursor

  int tune_idx = -1;
  if(curr_screen == 1){           // 1st set of 7 notes (0 - 6)
    tune_idx = 0;
  } else if (curr_screen == 2){   // 2nd set of 7 notes (7 - 13)
    tune_idx = 7;
  } else if (curr_screen == 3){   // 3rd set of 7 notes (14 - 20)
    tune_idx = 14;
  }

  for(int c = 1; c <= 13; c+=2){          // print in columns (1, 3, ..., 13)

    int note_index = notes[tune_idx];     // get index mapping to note string/octave/frequency

    lcd_moveto(0, c);                     // print note in 
    lcd_stringout(note_str[note_index]);  // 0th row, cth column

    lcd_moveto(1, c);                     // print octave in
    display_octave(note_index, c);        // 1st row, cth column

    tune_idx += 1;                        // increment index of next note in tune
  }
}

// display an individual octave for a note on the 2nd row
void display_octave(int index, int col)
{
  int octave = 0;

  if ((1 <= index) && (index <= 12)){
    octave = 3;
  }
  else if ((13 <= index) && (index <= 24)){
    octave = 4;
  }
  else if (index == 15){
    octave = 5;
  }

  // display octave on LCD if non-rest
  if(octave != 0){
    char octave_str[2];
    snprintf(octave_str, 2, "%d", octave);
    lcd_stringout(octave_str);
  } else { // rest
    lcd_stringout(" ");
  }
}

// display previous/next arrows
void display_arrows(){
    if (curr_screen == 1){
      lcd_moveto(0, 0);   // delete left arrow
      lcd_stringout(" ");
      
      lcd_moveto(0, 15);  // show right arrow 
      lcd_stringout(">");
    }

    else if (curr_screen == 2){
      lcd_moveto(0, 0);   // show left arrow
      lcd_stringout("<");

      lcd_moveto(0, 15);  // show right arrow 
      lcd_stringout(">");

    } else if (curr_screen == 3){
      lcd_moveto(0, 15);  // delete right arrow
      lcd_stringout(" ");

      lcd_moveto(0, 0);   //show left arrow
      lcd_stringout("<"); 
    }
}

// display screen number at bottom left
void display_screen_num(void) {
  lcd_moveto(1, 0); // move curstor to bottom left

  // print screen num
  if(curr_screen == 1) {
    lcd_stringout("1");
  } else if(curr_screen == 2) {
    lcd_stringout("2");
  } else if (curr_screen == 3) {
    lcd_stringout("3");
  }
}

void show_screen(void){
    display_screen_num();         // display screen number
    display_arrows();             // display arrow
    display_notes_and_octaves();  // display notes/octaves
}