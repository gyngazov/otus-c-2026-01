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
#include <time.h>
#include <arpa/inet.h>

#define RUNNING_DIR	    "/tmp/"
#define LOCK_FILE	    RUNNING_DIR"otus.lock"
#define LOG_FILE	    RUNNING_DIR"otus.log"
#define ERROR_OPENLOG   -11
#define ERROR_LOG       -12
#define ERROR_DEVNUL    -13
#define ERROR_LOCK      -14
#define ERROR_GETPID    -15
#define ERROR_NOMEM     -16
#define ERROR_SPRINT    -18
#define FILE_NAME       LOG_FILE
#define RESP_LEN        32

#define SOCK_BUF        1024
#define PORT            8080
#define PENDING         3

static char *get_date_time()
{
    time_t now;
    struct tm *timeinfo;
    time(&now);
    timeinfo = localtime(&now);
    char *template = "%d-%02d-%02d %02d:%02d:%02d";
    char *output = (char *) malloc(sizeof(template) + 1);
    if (output == NULL)
        exit(ERROR_NOMEM);
    int ret = sprintf(output, template, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
            timeinfo->tm_mday, timeinfo->tm_hour,
            timeinfo->tm_min, timeinfo->tm_sec);
    if (ret < 0)
        exit(ERROR_SPRINT);
    return output;
}

void log_message(char *message)
{
    FILE *logfile;
	logfile = fopen(LOG_FILE, "a");
	if (logfile == NULL) {
        printf("can not open log file\n");
        exit(ERROR_OPENLOG);
    }
    char *dt;
    dt = get_date_time();
	if (fprintf(logfile, "%s %s\n", dt, message) < 0) {
        printf("cannot write log\n");
        exit(ERROR_LOG);
    }
    free(dt);
	fclose(logfile);
}

void signal_handler(int sig)
{
	switch(sig) {
	case SIGHUP:
		log_message("hangup signal catched");
		break;
	case SIGTERM:
		log_message("terminate signal catched");
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

void log_ip(struct sockaddr_in client)
{
    struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&client;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    char *ip;
    ip = (char *) malloc(INET_ADDRSTRLEN + 1);
    if (ip == NULL)
        exit(ERROR_NOMEM);
    if (inet_ntop(AF_INET, &ipAddr, ip, INET_ADDRSTRLEN) == NULL)
        exit(errno);
    char msg[INET_ADDRSTRLEN + 13];
    int ret = sprintf(msg, "Client ip: %s", ip);
    if (ret < 0)
        exit(ERROR_SPRINT);
    log_message(msg);
    free(ip);
}

long get_file_size(char *name)
{
    struct stat sb;
    if (stat(name, &sb) == -1)
        exit(errno);
    return sb.st_size;
}

void dialog(int next)
{
    if (next == -1)
        exit(errno);
    char buffer[SOCK_BUF];
    if (read(next, buffer, SOCK_BUF - 1) == -1)
        exit(errno);
    int blen = sizeof(buffer);
    char msg[blen + 16];
    if (sprintf(msg, "Query received: %s", buffer) < 0)
        exit(ERROR_SPRINT);
    log_message(msg);
    char resp[RESP_LEN];
    if (sprintf(resp, "File %s has size %ld", FILE_NAME, get_file_size(FILE_NAME)) < 0)
        exit(ERROR_SPRINT);
    log_message(resp);
    if (send(next, resp, sizeof(resp) + 1, 0) == -1)
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
        next = accept(srv_id, (struct sockaddr *) &addr, &addrlen);
        log_ip(addr);
        dialog(next);
    }
    return 0;
}
