/* Includes */
#include <stdio.h>      /* Input/Output */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include <assert.h>

/* Includes developed for the assignment 1*/
#include "morselib.h" /* Translation char to morse and viceversa */
#include "signallib.h" /* Translation char to morse and viceversa */
#include "log.h" /* Translation char to morse and viceversa */
#define MAXSTRING 650

static int sigpipe = 0;

/* Race-condition is avoided by using a pipe to buffer signals. */
static void sig_usr(int signo) {
	char ret = signo;
	if( write(sigpipe,&ret,1) == -1 ){
		printf("SIG_USR : ERROR");
		exit(0);
	}
}
/* We indicate CHILD that we are ready for a new signal */
int flowcontrol(FILE *stream){
	int res;				
	res = fprintf (stream, "%c", 'Z');
	if( res < 0 ){ 	
		return(1);
	} 
	fflush(stream);
	return(0);
}

int main(int argc, char *argv[])
{
	pid_t childpid; /* variable to store the child's pid */
	int pipeflow[2]; /* pipe to FLOW CONTROL among processes */
	char string[MAXSTRING];
	FILE *logfile;
 		
	/* we create log file and we close it*/
	logfile = fopen(LOGFILE, "w");
	if (logfile == NULL){
	      	printf("ERROR: LOG FILE CAN'T BE CREATED\n");
	      	return(1);
	}
	strcpy(string,"START OF LOG FILE\n");
	fputs(string, logfile);
	if(fclose(logfile)!=0) printf("ERROR: LOG FILE CAN'T BE CREATED\n");

	/* control of arguments */    
	if (argc != 3){
		printf("ERROR: Please introduce 2 valid filenames\n");
		strcpy(string,"ERROR: Please introduce 2 valid filenames\n");	
		Log(string);	
		return(1);	
	}

	/* creation of the pipe. */
	if (pipe (pipeflow)){
		strcpy(string,"ERROR: Pipe could not be created\n");	
		Log(string);
	      	return(1);
	}
	/* creation of new process */
	childpid = fork();
	    
	if (childpid >= 0) /* fork succeeded */
	{
		if (childpid == 0) /* fork() returns 0 to the child process */
		{

			int c;				
			char *morseword;		
			FILE *fin,*stream;

			strcpy(string,"CHILD: I am the child process, I'm alive\n");	
			Log(string);			
			/* Stream is opened in order to allow FLOW CONTROL */
			close (pipeflow[1]); /* reading side of the pipe is closed */				
			stream = fdopen (pipeflow[0], "r");
			if(stream == NULL){
				strcpy(string,"CHILD : ERROR: STREAM fdopen is null\n");	
				Log(string);
				return(1);	
			}
			/* file1 is opened in read mode */
			fin = fopen(argv[1],"r");
			if(fin == NULL){
				strcpy(string,"CHILD: ERROR: FILE1 CAN NOT BE READ \n");	
				Log(string);
				/* we notify the PARENT about file 1 */	
				kill(getppid(),SIGALRM);
				return(1);	
			}	
			/* we read char by char from file and execute chartomorse() and sendsignals() */	
			while(1){				
				c = fgetc(fin); 
				/* in case EOF is reached we inform the PARENT through SIGINT */
				if( feof(fin) ){ 
					if (kill(getppid(),SIGINT) != 0) {
						strcpy(string,"CHILD: ERROR: UNABLE TO SEND SIGINT\n");	
						Log(string);
						return(1);
					}
					break;	
				}
				/* we get a pointer to the MORSEWORD that equals char */
				morseword = chartomorse(c); 
				if (morseword == NULL){
					strcpy(string,"CHILD: ERROR: CHARTOMORSE FUNCTION: CHAR NOT FOUND\n");	
					Log(string);
					return(1);
				 }
				/* we send signals according to the morse word and parent's pid*/				
				if(sendsignals(morseword,getppid(),stream) != 0){
					strcpy(string,"CHILD: ERROR: SENDSIGNALS FUNCTION: UNABLE TO SEND SIGNALS\n");	
					Log(string);
					return(1);
				}
			}
			if(fclose(fin)!=0) {
				strcpy(string,"CHILD: ERROR: FILE CAN NOT BE CLOSED \n");	
				Log(string);	
			}
			if(fclose (stream)!=0){
				strcpy(string,"PARENT: ERROR: PIPE fclose ! 0\n");	
				Log(string);
			}
			strcpy(string,"CHILD: STREAM AMB FILE CLOSED. GOOD BYE\n");	
			Log(string);  
		}
		else /* fork() returns new pid to the parent process */
		{
			struct sigaction sig;
			int res,i = 0;
			char a, b, mysignal,morseword[MORSEMAXLENGTH];		
			FILE *stream,*fout;
			int pipefd[2];

			strcpy(string,"PARENT: I am the parent process!I'm alive!\n");	
			Log(string);
			/* stream is opened to control flow through pipe*/
			close (pipeflow[0]); /* reading side of the pipe is closed*/
			stream = fdopen (pipeflow[1], "w");
			if(stream == NULL){
				strcpy(string,"CHILD: ERROR: PIPEFLOW CAN NOT BE CREATED\n");	
				Log(string);
				return(1);	
			}
			/* signal handling pipe management */
			if (pipe(pipefd) != 0){
				strcpy(string,"PARENT: ERROR: STORING PIPE CAN NOT BE CREATED.\n");	
				Log(string);
				return(1);
			}
			sigpipe = pipefd[1];	
			/* definition of new signal handlers */
			sigemptyset(&sig.sa_mask);	
			sig.sa_flags=0;
			sig.sa_handler = sig_usr;

			if (sigaction(SIGUSR1,&sig,NULL) != 0) { 
				exit(0);
			}
			if (sigaction(SIGUSR2,&sig,NULL) != 0) { 
				exit(0);
			}
			if (sigaction(SIGALRM,&sig,NULL) != 0) { 
				exit(0);
			}
			if (sigaction(SIGINT,&sig,NULL) != 0) { 
				exit(0);
			}
			/* File is opened in order to write to it */		
			fout = fopen (argv[2], "w");
			if(fout == NULL){
				strcpy(string,"PARENT: ERROR: fopen is null\n");	
				Log(string);
				return(1);	
			}
			/* start i) reception of signals and ii) build morseword to afterthat iii) transform into char*/	
			for ( ; ; ) {
				/* When read is interrupted by a signal, it will return -1 and errno is EINTR.*/
				res = read( pipefd[0], &mysignal , 1); 
				if (res == 1) {
					/* we indicate CHILD to send next SIGNAL*/
					if (flowcontrol(stream)!= 0){
						strcpy(string,"PARENT : ERROR : FLOW CONTROL ERROR\n ");	
						Log(string);
						return(1);
					}
					a = signaltomorse(mysignal); /* it tranforms signal into MORSE UNIT */
					morseword[i] = a; /* it builds MORSE WORD by adding MORSE UNIT */

					/* we identify END OF MORSEWORD which means SIGALRM */
					if (a == '\0'){
						/* we identify SIGALRM as FILE1 ERROR */	 
						if(i == 0){
							strcpy(string,"PARENT : ERROR : FILE1 NOT FOUND \n ");	
							Log(string);
							return(1);	
						}					
						b = morsetochar(morseword); /* we get the char that corresponds to the MORSE WORD */
						if (b == (int)0) {
							strcpy(string,"PARENT : ERROR : UNABLE TO TRANSFORM TO CHAR\n ");	
							Log(string);
							break;
						}
						else{
							if(fprintf(fout,"%c", b) < 0){
								strcpy(string,"PARENT : ERROR : UNABLE TO WRITE TO FILE\n ");	
								Log(string);
								return(1); /* we write the char to file */
							}
							i=0; /* we start a new morseword */
						}					
					}
					/* we identify END OF FILE */
					else if (a == EOF){
						strcpy(string,"PARENT : INFO : END OF FILE\n");	
						Log(string);
						break;
					}
					else if (a == SIGNALERROR){
						strcpy(string,"PARENT : ERROR : UNKNOWN SIGNAL\n");	
						Log(string);
						break;
					}
					/*  no ENDOFMORSE and NO EOF. Which means SIGUSR1 or SIGUSR2 we keep adding values to morseword */
					else i++;

				}
			}
			if(fclose(fout)!=0) {
				strcpy(string,"CHILD: ERROR: FILE CAN NOT BE CLOSED \n");	
				Log(string);	
			}
			if(fclose (stream)!=0){
				strcpy(string,"PARENT: ERROR: PIPE fclose ! 0\n");	
				Log(string);
			}
			strcpy(string,"PARENT: STREAM AND FILE CLOSED. GOOD BYE\n");	
			Log(string);           
		}
	}
	else /* fork returns -1 on failure */
	{
		strcpy(string,"FORK ERROR \n");	
		Log(string);
		return(1); 
	}
	return(0);
}
