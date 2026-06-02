#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main()
{
    char buf[16] = {0};
    char tmp[16] = {0};
    snprintf(buf, 16, "%d", 12345);
    printf("%s %d\n", buf, strlen(buf));
    snprintf(tmp, 16, "%s%s%s", "asd", buf, "CXZ");
    printf("%s %d\n", tmp, strlen(tmp));

    printf("%d\n", atoi("dd"));
}