#include "logger.h"
#include <stdio.h>

void main()
{
    init_logger();
    printf("logger: %p\n", logger);
    write_log("msg 1");
    write_log("msg 2");
}

