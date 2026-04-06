
#include "config.h"
#include <stdio.h>

void main()
{
    struct Params params = get_params("config.cfg");
    printf("%d %s\n", params.port, params.file);
}
