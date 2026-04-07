#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "daemon.h"
#include "config.h"

#define CFG_FILE "config.cfg"

int main(int argc, char **argv)
{
    char *cfg;
    if (argc == 2)
        cfg = argv[1];
    else 
        cfg = CFG_FILE;
    struct Params params = get_params(cfg);
    int ret = daemonize();
    if (ret != EXIT_SUCCESS) {
        log_message("Can't create daemon");
        return ret;
    }
    ret = set_socket(params.port);
    if (ret != EXIT_SUCCESS) {
        log_message("Can't create socket");
        return ret;
    }
    set_file_name(params.file);
    int next;
    while (1) {
        next = get_conn();
        if (next == -1) {
            log_message("Can't accept connection");
            return errno;
        }
        ret = log_ip();
        if (ret != EXIT_SUCCESS) {
            log_message("Can't define ip");
            return ret;
        }
        update_size();
        ret = dialog(next);
        if (ret != EXIT_SUCCESS) {
            log_message("Can't answer a file size");
            return ret;
        }
    }
    return EXIT_SUCCESS;
}

