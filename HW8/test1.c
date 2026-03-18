#include <stdio.h>
#include <time.h>
void get_date_time(char *output);
void main()
{
  time_t now;
  struct tm *timeinfo;
  time(&now);
  timeinfo = localtime(&now);
  printf("[%d-%02d-%02d %02d:%02d:%02d]\n", timeinfo->tm_year + 1900, 
          timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  char *output;
  get_date_time(output);
    printf("output: %s\n", output);
}

void get_date_time(char *output)
{
    time_t now;
    struct tm *timeinfo;
    time(&now);
    timeinfo = localtime(&now);
    char *template = "%d-%02d-%02d %02d:%02d:%02d";
    sprintf(output, template, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
           timeinfo->tm_mday, timeinfo->tm_hour,
           timeinfo->tm_min, timeinfo->tm_sec);
}

