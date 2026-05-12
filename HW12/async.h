
int setnonblocking(const int sock);
int do_read(const int fd, char *buffer);
void send_file(const int sockfd, const char *dir, const char *buffer) ;
void process_error(const int fd);