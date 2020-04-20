extern unsigned char curr_screen;
extern unsigned char notes[21];
void display_arrows(void);
void display_notes_and_octaves(void);
void display_octave(int, int);
void show_screen(void);

extern unsigned char new_state, old_state;
extern char *note_str[];

extern unsigned char changed;  // Flag for state change
extern unsigned int count;		// Count to display
extern unsigned char a, b;