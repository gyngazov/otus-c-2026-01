#include <stdlib.h>

#include "daemon.h"
#include "config.h"

int main(int argc, char **argv)
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
    int next = 0;
    int ret = 0;
    while (1) {
        next = accept(srv_id, (struct sockaddr *) &addr, &addrlen);
        log_ip(addr);
        update_size();
        ret = dialog(next);
        if (ret != EXIT_SUCCESS)
            return ret;
    }
    return EXIT_SUCCESS;
}

