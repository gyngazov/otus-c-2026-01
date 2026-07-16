#define CLEN        150
#define VLEN        255
#define MAX_BATCH   10000
#define ROW_LEN     512

struct SourceRow {
    int id;
    char code[CLEN];
    char val[VLEN];
    int val_size;
};

struct Batch {
    int size; // fact batch size
    struct SourceRow rows[MAX_BATCH];
};