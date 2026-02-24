#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "weather.h"

#define SITE    "https://wttr.in/"
#define FORMAT  "?format=j1"

char *get_url(char *city);

int main(int argc, char** argv)
{
    //char *url = "https://www.example.com";
    char *url = "https://wttr.in/moscow?format=j1";
    if (argc != 2) {
        printf("usage: %s city_name\n", argv[0]);
        return 8;
    }
	char *url = get_url(argv[1]);
	printf("city: %s\n", url);
    char *res = get_weather(url);
    parse_json(res);
    return 0;
}

char *get_url(char *city)
{
	int len = sizeof(SITE) + sizeof(city) + sizeof(FORMAT) - 2;
	char url[len];
	strncat(url, SITE, sizeof(SITE));
	strncat(url, city, sizeof(city));
	strncat(url, FORMAT, sizeof(FORMAT));
	printf("%s\n", url);
	return url;	
}
