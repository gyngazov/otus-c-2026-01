
#define DEFAULT_PORT    8080
#define DEFAULT_NAME    "/tmp/otus.log"
#define FILE_LEN        128
#define CFG_FILE        "config.cfg"

struct Params {
    int port;
    char file[FILE_LEN];
};

struct Params get_params(const char *cfg_name);