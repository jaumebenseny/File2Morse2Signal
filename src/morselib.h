#define MORSEMAXLENGTH 8

char * chartomorse(char m); /* Seeks a character in all even poistions of vector let[]. Then, it shares the memory address of the following vector of chars which corresponds to the morse codification of the character */

char morsetochar(char m[]);/* Seeks for a morse word in all odd positions of the vector let[]. Then, it shares the character which is located in the previous position along the same vector */
