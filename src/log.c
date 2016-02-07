#include <stdio.h>   
#include <stdlib.h>     /* General Utilities */
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define LOGFILE	"log.txt"

/* Gets a lock of the indicated type on the fd which is passed. 
   The type should be either F_UNLCK, F_RDLCK, or F_WRLCK */
void getlock(int fd, int type) {
    struct flock lockinfo;

    /* we'll lock the entire file */
    lockinfo.l_whence = SEEK_SET;
    lockinfo.l_start = 0;
    lockinfo.l_len = 0;

    /* keep trying until we succeed */
    while (1) {
        lockinfo.l_type = type;
        /* if we get the lock, return immediately */
        if (!fcntl(fd, F_SETLK, &lockinfo)) return;

        /* find out who holds the conflicting lock */
        fcntl(fd, F_GETLK, &lockinfo);

	/* we wait for PARENT OR CHID to write their message */
	sleep(0.25);
    }
}

void Log (char *message)
{
	int fd;

    	fd = open(LOGFILE, O_RDWR | O_APPEND, 0666);
   	if (fd < 0) {
        	perror("ERROR : open log file");
        	exit(0);
    	}
	getlock(fd, F_RDLCK);
	if ( write(fd, message, strlen(message)) == EOF ) exit(0);
	if ( close(fd) != 0) exit(0);
}
