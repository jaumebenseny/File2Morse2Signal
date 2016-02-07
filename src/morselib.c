/* Includes */
#include <stdio.h>      /* Input/Output */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdlib.h>     /* General Utilities */
#include <string.h>     /* String Utilities */

#define MORSEMAXLENGTH 8
#define MORSEDICTLENGTH 101

/* we define BLANK SPACE as " ","......." */
/* we define END OF LINE as "\n","-......" */
/* we define ERROR MORSE as "?","........" so we return a ? */
/* the vector let[] knows 49 chars */
/* the vector end with and space to indicate END OF MORSEWORD */

char *let[MORSEDICTLENGTH]={"E",".","T","-","I","..","A",".-","N","-.","M","--","S","...","U","..-","R",".-.","W",".--","D","-..","K","-.-","G","--.","O","---","H","....","V","...-","F","..-.","L",".-..","P",".--.","J",".---","B","-...","X","-..-","C","-.-.",
"Y","-.--","Z","--..","Q","--.-","0","-----","1",".----","2","..---","3","...--","4","....-","5",".....","6","-....","7",
"--...","8","---..","9","----.","/","-..-.",",","--..--",".",".-.-.-","?","..--..","=","-...-","+",".-.-.","#","-.--.","$",
"...-.-","-","-....-",";","---...",";","-.-.-."," ",".......","\n","-......","?","........",""}; 

/* Seeks a character in all even poistions of vector let[]. Then, it shares the memory address of the following vector of chars which corresponds to the morse codification of the character */
char * chartomorse(char m)
{
	int i;
	for( i=0 ; i < (MORSEDICTLENGTH-1) ; i=i+2){
		if( *let[i] == m ){						
			return(let[i+1]);
		}
	}
	/* in case the char can not be found in the library, we send MORSE ERROR MESSAGE */
	return (let[MORSEDICTLENGTH-2]);
}

/* Seeks for a morse word in all odd positions of the vector let[]. Then, it shares the character which is located in the previous position along the same vector */
char morsetochar(char m[])
{
	int i,j;
	char morseword[MORSEMAXLENGTH];
	/* we go along every morseword in the vector let */
	for( i=1 ; i < (MORSEDICTLENGTH-1) ; i=i+2){
		/* for each morseword we need to know were it ends to compare with the one we are looking for */
   		for ( j = 0; j < MORSEMAXLENGTH ; j++)
   		{
			if (  *(m + j) == ' ' ) break;
			morseword[j] = *(m + j);
   		}		
			
		if( strcmp(let[i],morseword) == 0 ){	
			return((char)*let[i-1]);
		}
	}
	return((int)0);
}
