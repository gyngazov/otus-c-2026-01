
#include <confuse.h>
#include <stdio.h>

struct Param {
    int port;
    const char *name;
};

void main()
{
    cfg_opt_t opts[] = {
        CFG_STR("file", "a", CFGF_NONE),
        CFG_INT("port", 1234, CFGF_NONE),
        CFG_END()
    };
    cfg_t *cfg;
    cfg = cfg_init(opts, CFGF_NONE);
    if (cfg_parse(cfg, "config.cfg") == CFG_PARSE_ERROR) {
        printf("Error parsing config file.\n");
        return;
    }
    struct Param par;
    par.port = cfg_getint(cfg, "port");
    par.name = cfg_getstr(cfg, "file");
    printf("p: %d n: %s\n", par.port, par.name);

}
