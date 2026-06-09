

#define CFG_FILE    "config.cfg"
#define STR_LEN     64

struct Params {
    char type[STR_LEN];
    char db[STR_LEN];
    char table[STR_LEN];
    char column[STR_LEN];
};

int get_params(struct Params *params);