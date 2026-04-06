#include <netinet/in.h>

void set_file_name(char *name);
void update_size();
int daemonize();
struct sockaddr_in set_addr(int port);
int set_socket(int port);
int log_ip();
int dialog(int next);
