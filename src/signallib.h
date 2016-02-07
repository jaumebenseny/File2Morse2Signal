#define SLEEPTIME 1
#define EOFF 'E'
#define SIGNALERROR 'F'

int sendsignals(char *morseword, int pid, FILE *stream);/* we read the morseword and we send signals according to -/. codification  */

char signaltomorse(char mysignal);/* according to the type of signal received we return the char required to complete the morseword */
