#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <confuse.h>

#include "config.h"

void get_params(struct Params *params) 
{
    cfg_opt_t opts[] = {
        CFG_STR("type", "sqlite", CFGF_NONE),
        CFG_STR("db", "test.db", CFGF_NONE),
        CFG_STR("table", "tab", CFGF_NONE),
        CFG_STR("column", "b", CFGF_NONE),
        CFG_END()
    };
    cfg_t *cfg;
    cfg = cfg_init(opts, CFGF_NONE);
    cfg_add_searchpath(cfg, "./");
    int res = cfg_parse(cfg, CFG_FILE);
    char *err;
    if (res == CFG_FILE_ERROR) {
        err = "Error opening config file.";
        goto err;
    }
    if (res == CFG_PARSE_ERROR) {
        err = "Error parsing config file.";
        goto err;
    }
    
    strncpy(params->type, cfg_getstr(cfg, "type"), STR_LEN - 1);
    strncpy(params->db, cfg_getstr(cfg, "db"), STR_LEN - 1);
    strncpy(params->table, cfg_getstr(cfg, "table"), STR_LEN - 1);
    strncpy(params->column, cfg_getstr(cfg, "column"), STR_LEN - 1);
    cfg_free(cfg);
    return;
err:
    cfg_free(cfg);
    puts(err);
    exit(EXIT_FAILURE);
}
