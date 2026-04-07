
#define DEFAULT_PORT    8080
#define DEFAULT_NAME    "/tmp/otus.log"
#define CFG_FILE        "config.cfg"
#define NAME_LEN        128

struct Params {
    int port;
    char file[NAME_LEN];
};

struct Params get_params(const char *cfg_name);

