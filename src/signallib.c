/* Includes */
#include <stdio.h>      /* Input/Output */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
#include <signal.h>	/* Signals */

/* Includes developed for the assignment 1*/
#include "morselib.h" /* Translation char to morse and viceversa */

#define SLEEPTIME 1
#define SIGNALERROR 'F'

int waitcontrolflow(FILE *stream){
	int c;
	c = fgetc (stream);
	if (c == EOF){
		return(4);
	}
	return(0);
}

/* we read the morseword and we send signals according to -/. codification  */
int sendsignals(char *morseword,int pid,FILE *stream)
{
	int i;
	for ( i = 0; i < MORSEMAXLENGTH+1 ; i++)
	{
		/* send signal according to . */
		if (  *(morseword + i) == '.' ){
			if( kill(pid,SIGUSR1) != 0) return (1);
			if(waitcontrolflow(stream)!= 0) return (4);
		}
		/* send signal according to - */	
		else if  (  *(morseword + i) == '-' ){
			if( kill(pid,SIGUSR2) != 0) return (2);
			if(waitcontrolflow(stream)!= 0) return (4);		
		}
		/* we send SIGALRM to define END OF MORSE WORD */
		else{			
			if( kill(pid,SIGALRM) != 0) return (3);
			if(waitcontrolflow(stream)!= 0) return (4);
			break;
		}
	}
	return (0);
}

/* according to the type of signal received we return the char required to complete the morseword */
char signaltomorse(char mysignal)
{
	if (mysignal == SIGUSR1){        
		return('.');
	}
	else if (mysignal == SIGUSR2){   
		return('-');
	}
	/* END OF CHARACTER - WE RETURN \0 */
	else if (mysignal == SIGALRM){   
		return('\0'); 
	}
	/* END OF FILE */
	else if (mysignal == SIGINT){   
		return(EOF);
	}
	return(SIGNALERROR);
}
