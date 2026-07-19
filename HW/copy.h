#include <postgresql/libpq-fe.h>

#include "utils.h"

int insert(PGconn *conn, struct Batch batch);
PGconn *get_conn();
