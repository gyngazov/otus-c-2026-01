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
    printf("Current weather\ncity: %s\n", city);
    parse_json(resp_body);
	free(resp_body);
    return 0;
}

void set_url(char *url, char *city)
{
	strncat(url, SITE, strlen(SITE) + 1);
	strncat(url, city, strlen(city) + 1);
	strncat(url, FORMAT, strlen(FORMAT) + 1);
}
