#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

#define AGENT "libcurl-agent/1.0"
#define SITE "https://wttr.in/"
#define FORMAT "?format=j1"

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t write_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

char *get_url(char *city)
{
    return city;
}

char *get_weather(char* city)
{
    CURL *curl;
    CURLcode result;

    struct MemoryStruct chunk;

    result = curl_global_init(CURL_GLOBAL_ALL);
    if(result != CURLE_OK)
        return (int)result;

    chunk.memory = malloc(1); /* grown as needed by the realloc above */
    chunk.size = 0;           /* no data at this point */


    curl = curl_easy_init();
    char *url = get_url(city); //"https://wttr.in/moscow?format=j1";
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, AGENT);
        result = curl_easy_perform(curl);
    }
    if(result != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(result));
    } else {
        printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
        fwrite(chunk.memory, 1, chunk.size, stdout);
    }
    curl_easy_cleanup(curl);
 
    free(chunk.memory);
    curl_global_cleanup();
 
    return (int)result;
}
