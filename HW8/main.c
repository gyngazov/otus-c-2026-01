#include "daemon.h"
#include "config.h"
#include <stdlib.h>

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
        pritnf("Can't create daemon\n");
        return ret;
    }
    ret = set_socket(params.port);
    if (ret != EXIT_SUCCESS) {
        pritnf("Can't create socket\n");
        return ret;
    }
    set_file_name(params.file);
    int next;
    while (1) {
        next = get_conn();
        if (next != EXIT_SUCCESS) {
            pritnf("Can't accept connection\n");
            return ret;
        }
        ret = log_ip();
        if (ret != EXIT_SUCCESS) {
            pritnf("Can't define ip\n");
            return ret;
        }
        update_size();
        ret = dialog(next);
        if (ret != EXIT_SUCCESS) {
            pritnf("Can't answer a file size\n");
            return ret;
        }
    }
    return EXIT_SUCCESS;
}

