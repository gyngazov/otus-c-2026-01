#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define RUNNING_DIR	"/tmp/"
#define LOCK_FILE	RUNNING_DIR"otus.lock"
#define LOG_FILE	RUNNING_DIR"otus.log"
#define ERROR_OPENLOG   -11
#define ERROR_LOG       -12
#define ERROR_DEVNUL    -13
#define ERROR_LOCK      -14
#define ERROR_GETPID    -15

void log_message(char *filename, char *message)
{
    FILE *logfile;
	logfile = fopen(filename,"a");
	if (logfile == NULL) {
        printf("can not open log file\n");
        exit(ERROR_OPENLOG);
    }
	if (fprintf(logfile,"%s\n",message) < 0) {
        printf("cannot write log\n");
        exit(ERROR_LOG);
    }
	fclose(logfile);
}

void signal_handler(int sig)
{
	switch(sig) {
	case SIGHUP:
		log_message(LOG_FILE,"hangup signal catched\n");
		break;
	case SIGTERM:
		log_message(LOG_FILE,"terminate signal catched\n");
		exit(0);
		break;
	}
}

void daemonize()
{
	if (getppid() == 1) 
        return;
	int i = fork();
	if (i < 0) 
        exit(errno);
	if (i > 0)
        exit(0);
	if (setsid() == -1)
        exit(errno);
	for (i = getdtablesize(); i >= 0; --i) 
        close(i); 
	i = open("/dev/null", O_RDWR);
    if (i != 0)
        exit(ERROR_DEVNUL);
    if (dup(i) == -1 || dup(i) == -1)
        exit(errno);
	umask(027); 
	if (chdir(RUNNING_DIR) == -1)
        exit(errno);
	int lfp = open(LOCK_FILE, O_RDWR | O_CREAT, 0640);
	if (lfp < 0) 
        exit(1); 
	if (lockf(lfp, F_TLOCK, 0) < 0) 
        exit(ERROR_LOCK); 
    char str[10];
	if (sprintf(str, "%d\n", getpid()) < 0)
        exit(ERROR_GETPID);
	if (write(lfp, str, sizeof(str)) == -1)
        exit(errno);
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR || 
	    signal(SIGTSTP, SIG_IGN) == SIG_ERR ||
	    signal(SIGTTOU, SIG_IGN) == SIG_ERR ||
	    signal(SIGTTIN, SIG_IGN) == SIG_ERR ||
	    signal(SIGHUP, signal_handler) == SIG_ERR ||
	    signal(SIGTERM, signal_handler) == SIG_ERR)
        exit(errno);
}

void main()
{
	daemonize();
	while(1) 
        sleep(1); 
}
