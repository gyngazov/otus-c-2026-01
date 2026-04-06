#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <confuse.h>

#include "config.h"

struct Params get_params(const char *cfg_name) 
{
    cfg_opt_t opts[] = {
        CFG_STR(FILE_CFG, DEFAULT_NAME, CFGF_NONE),
        CFG_INT(PORT_CFG, DEFAULT_PORT, CFGF_NONE),
        CFG_END()
    };
    cfg_t *cfg;
    cfg = cfg_init(opts, CFGF_NONE);
    if (cfg_parse(cfg, cfg_name) == CFG_PARSE_ERROR) {
        printf("Error parsing config file.\n");
        exit(-1);
    }
    struct Params params;
    params.port = cfg_getint(cfg, PORT_CFG);
    strncpy(params.file, cfg_getstr(cfg, FILE_CFG), FILE_LEN - 1);
    cfg_free(cfg);
    return params;
}
