
int setnonblocking(const int sock);
void send_file(const int sockfd, const char *file_path) ;
void process_error(const int fd);
char *substring(const char *buf, const int start, const int end);
void ignore_broken_pipe();