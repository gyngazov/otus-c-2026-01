
#define DEFAULT_PORT    8080
#define DEFAULT_NAME    "/tmp/otus.log"
#define FILE_LEN        128

struct Params {
    int port;
    const char *file;
};

struct Params get_params(const char *cfg_name);