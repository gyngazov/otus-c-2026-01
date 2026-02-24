#include <stdlib.h>

#define AGENT   "libcurl-agent/1.0"
#define TIME_OUT 30

struct MemoryStruct {
  char *memory;
  size_t size;
};

char *get_weather(char *url);
