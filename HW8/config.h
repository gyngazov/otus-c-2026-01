
#define DEFAULT_PORT    8080
#define DEFAULT_NAME    "/tmp/otus.log"
#define FILE_LEN        128
#define FILE_CFG        "file"
#define PORT_CFG        "port"

struct Params {
    int port;
    char file[FILE_LEN];
};

struct Params get_params(const char *cfg_name);