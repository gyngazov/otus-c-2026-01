#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include "weather.h"

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

/*
 * Получение курлом ответа на http-запрос.
 * */
void get_weather(char *url, struct MemoryStruct *chunk)
{
    CURL *curl;
    CURLcode result;

//    struct MemoryStruct *chunk;

    result = curl_global_init(CURL_GLOBAL_ALL);
    if (result != CURLE_OK) {
        printf("Ошибка инициализации curl.\n)");
        exit(17);
    }
/*
    chunk->memory = malloc(1);
    if(chunk->memory == NULL) {
        printf("not enough memory\n");
	    exit(17);
    }
    chunk->size = 0;   
*/
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, AGENT);
        result = curl_easy_perform(curl);
    }
    if (result != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(result));
        exit(17);
    } 
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}
