#include <netinet/in.h>

void set_file_name(char *name);
void update_size();
void daemonize();
struct sockaddr_in set_addr(int port);
int set_socket(struct sockaddr_in address);
void log_ip(struct sockaddr_in client);
void dialog(int next);
