#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h>
#include <string.h>

#include "config.h"

#define RUNNING_DIR	    "/tmp/"
#define LOCK_FILE	    RUNNING_DIR"otus.lock"
#define LOG_FILE	    RUNNING_DIR"otus.log"
#define LOCKMODE        (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define ERROR_OPENLOG   -11
#define ERROR_LOG       -12
#define ERROR_LOCK      -14
#define ERROR_NOMEM     -16
#define ERROR_SPRINT    -18
#define FILE_NAME       LOG_FILE
#define RESP_LEN        128

#define SOCK_BUF        1024
#define PENDING         3
#define MAX_RL          1024
#define DATE_TIME       "%d-%02d-%02d %02d:%02d:%02d" 
#define PID_BUF         16

static char *file_name = FILE_NAME;
static long file_size = 0L;
static int server_fd = 0;
static char *datetime;
static struct sockaddr_in address;

static int get_date_time()
{
    time_t now;
    struct tm *timeinfo;
    time(&now);
    timeinfo = localtime(&now);
    size_t dt_len = sizeof(DATE_TIME) + 1;
    datetime = (char *) malloc(dt_len);
    if (datetime == NULL)
        return ERROR_NOMEM;
    if (snprintf(datetime, dt_len, DATE_TIME, 
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
            timeinfo->tm_mday, timeinfo->tm_hour,
            timeinfo->tm_min, timeinfo->tm_sec) < 0)
        return ERROR_SPRINT;
    return EXIT_SUCCESS;
}

static int log_message(const char *message)
{
    FILE *logfile;
	logfile = fopen(LOG_FILE, "a");
	if (logfile == NULL) {
        printf("Can't open log file\n");
        return ERROR_OPENLOG;
    }
    int ret = get_date_time();
    if (ret != EXIT_SUCCESS) {
        printf("Can't obtain date and time\n");
        return ret;
    }
	if (fprintf(logfile, "%s %s\n", datetime, message) < 0) {
        printf("Can't write log\n");
        return ERROR_LOG;
    }
    free(datetime);
	fclose(logfile);
}

static void signal_handler(int sig)
{
    int ret;
	switch(sig) {
	case SIGHUP:
		ret = log_message("hangup signal catched");
        if (ret != EXIT_SUCCESS) {
            printf("Can't log sighup\n");
            exit(ret);
        }
		break;
	case SIGTERM:
		ret = log_message("terminate signal catched");
        if (ret != EXIT_SUCCESS) {
            printf("Can't log sigterm\n");
            exit(ret);
        } else {
            exit(EXIT_SUCCESS);
        }
		break;
	}
}

int ftruncate(int fd, off_t length);

int set_lock()
{
    int fd = open(LOCK_FILE, O_RDWR | O_CREAT, LOCKMODE);
    if (fd == -1)
        return errno;
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    if (fcntl(fd, F_SETLK, &fl) == -1)
        return errno;
    if (ftruncate(fd, 0) == -1)
        return errno;
    char str[PID_BUF];
    if (snprintf(str, PID_BUF, "%d\n", getpid()) < 0)
        return ERROR_SPRINT;
    if (write(fd, str, sizeof(str)) == -1)
        return errno;
    return EXIT_SUCCESS;
}

int getdtablesize(void);

int daemonize()
{
	if (getppid() == 1) 
        return EXIT_SUCCESS;
	int i = fork();
	if (i < 0) 
        return errno;
	if (i > 0)
        return EXIT_SUCCESS;
	if (setsid() == -1)
        return errno;
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) == -1)
        return errno;
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = MAX_RL;
    unsigned long j;
    for (j = 0; j < rl.rlim_max; j++)
        close(j);
    i = open("/dev/null", O_RDWR);
    if (i == -1)
        return errno;
    if (dup(i) == -1 || dup(i) == -1)
        return errno;
    umask(027);
    if (chdir(RUNNING_DIR) == -1)
        return errno;
    int sl = set_lock();
    if (sl != EXIT_SUCCESS)
        return sl;
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR || 
	    signal(SIGTSTP, SIG_IGN) == SIG_ERR || 
	    signal(SIGTTOU, SIG_IGN) == SIG_ERR || 
	    signal(SIGTTIN, SIG_IGN) == SIG_ERR || 
	    signal(SIGHUP, signal_handler) == SIG_ERR || 
	    signal(SIGTERM, signal_handler) == SIG_ERR)
        return errno;
    return EXIT_SUCCESS;
}

struct sockaddr_in set_addr(int port)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    return address;
}

int set_socket(int port)
{
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        return errno;
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        return errno;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1)
        return errno;
    if (listen(server_fd, PENDING) == -1)
        return errno;
    return EXIT_SUCCESS;
}

int get_server_fd()
{
    return server_fd;
}

int get_conn()
{
    return accept(server_fd, (struct sockaddr *) &address, &address);
}

int log_ip()
{
    struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&address;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    char *ip;
    ip = (char *) malloc(INET_ADDRSTRLEN + 1);
    if (ip == NULL)
        return ERROR_NOMEM;
    if (inet_ntop(AF_INET, &ipAddr, ip, INET_ADDRSTRLEN) == NULL)
        return errno;
    char msg[INET_ADDRSTRLEN + 13];
    int ret = sprintf(msg, "Client ip: %s", ip);
    if (ret < 0)
        return ERROR_SPRINT;
    log_message(msg);
    free(ip);
    return EXIT_SUCCESS;
}

void set_file_name(const char *name)
{
    file_name = name;    
}

void update_size()
{
    struct stat stt;
    file_size = stat(file_name, &stt) == -1 ? -1L : stt.st_size;
}

int dialog(int next)
{
    if (next == -1)
        return errno;
    char buffer[SOCK_BUF];
    if (read(next, buffer, SOCK_BUF - 1) == -1)
        return errno;
    int msg_len = SOCK_BUF + 16;
    char msg[msg_len];
    if (snprintf(msg, msg_len, "Query received: %s", buffer) < 0)
        return ERROR_SPRINT;
    log_message(msg);
    char resp[RESP_LEN];
    int ret;
    if (file_size == -1L)
        ret = snprintf(resp, RESP_LEN, "File %s access error", file_name);
    else
        ret = snprintf(resp, RESP_LEN, "File %s has size %ld", file_name, file_size);
    if (ret < 0)
        return ERROR_SPRINT;
    log_message(resp);
    if (send(next, resp, strlen(resp) + 1, 0) == -1)
        return errno;
    if (close(next) == -1)
        return errno;
    return EXIT_SUCCESS;
}
