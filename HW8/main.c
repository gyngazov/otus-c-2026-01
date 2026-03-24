#include "daemon.h"
#include "config.h"

#define CFG_FILE "config.cfg"

void main(int argc, char **argv)
{
    char *cfg;
    if (argc == 2)
        cfg = argv[1];
    else 
        cfg = CFG_FILE;
    struct Params params = get_params(cfg);
    daemonize();
    struct sockaddr_in addr = set_addr(params.port);
    int srv_id = set_socket(addr);
    socklen_t addrlen = sizeof(addr);
    set_file_name(params.file);
    int next;
    while (1) {
        next = accept(srv_id, (struct sockaddr *) &addr, &addrlen);
        log_ip(addr);
        update_size();
        dialog(next);
    }
}

