// screen 
extern unsigned char curr_screen;
extern int curr_col;

// musicbox
extern char *note_str[];
extern unsigned char notes[21];
extern unsigned int note_freq[26];

// rotary encoder
extern unsigned char new_state, old_state;
extern unsigned char changed;   // Flag for state change
extern unsigned int count;		// Count to display
extern unsigned char a, b;
extern unsigned char encoder_used;

// LED
extern unsigned int note_count;
extern unsigned int total_notes;
extern unsigned int pwm_width;
