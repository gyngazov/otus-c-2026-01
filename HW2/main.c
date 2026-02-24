#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "weather.h"

#define SITE    "https://wttr.in/"
#define FORMAT  "?format=j1"

void set_url(char *url, char *city);

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("usage: %s city_name\n", argv[0]);
        return 8;
    }
    char *city = argv[1];
	int len = sizeof(SITE) + sizeof(city) + sizeof(FORMAT) - 2;
    char url[len];
	set_url(url, city);
    char *resp_body = get_weather(url);
    printf("city: %s\n", city);
    parse_json(resp_body);
    return 0;
}

void set_url(char *url, char *city)
{
	strncat(url, SITE, sizeof(SITE));
	strncat(url, city, sizeof(city));
	strncat(url, FORMAT, sizeof(FORMAT));
}
