#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define RUNNING_DIR	    "/tmp/"
#define LOCK_FILE	    RUNNING_DIR"otus.lock"
#define LOG_FILE	    RUNNING_DIR"otus.log"
#define ERROR_OPENLOG   -11
#define ERROR_LOG       -12
#define ERROR_DEVNUL    -13
#define ERROR_LOCK      -14
#define ERROR_GETPID    -15

#define SOCK_BUF        1024
#define PORT            8080
#define PENDING         3

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

struct sockaddr_in set_addr()
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    return address;
}

int set_socket(struct sockaddr_in address)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        exit(errno);
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT
                , &opt, sizeof(opt)) == -1)
        exit(errno);
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1)
        exit(errno);
    if (listen(server_fd, PENDING) == -1)
        exit(errno);
    return server_fd;
}

void dialog(int next)
{
    if (next == -1)
        exit(errno);
    char buffer[SOCK_BUF];
    if (read(next, buffer, SOCK_BUF - 1) == -1)
        exit(errno);
    log_message(LOG_FILE, "query received: %s\n", buffer);
    char *hello = "Hello\n";
    if (send(next, hello, sizeof(hello), 0) == -1)
        exit(errno);
    if (close(next) == -1)
        exit(errno);
}

int main()
{
	daemonize();
    struct sockaddr_in addr = set_addr();
    int srv_id = set_socket(addr);
    socklen_t addrlen = sizeof(addr);
    int next;
    while (1) {
        next = accept(srv_id, (struct sockaddr*)&addr, &addrlen);
        dialog(next);
    }
    return 0;
}
