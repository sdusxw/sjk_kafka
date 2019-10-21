#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include <iostream>

using namespace std;

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    
    char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

int main(int argc, char *argv[])
{
    string str_json = "[{\"passId\":\"sxjgl_lxgs_320300011050204007720_20191021_1571634478275_002878\",\"receivetime\":1571634476887,\"sendtime\":1571634476891,\"path\":\"cloud_lpr/jiangsu/lianxugs/321000_03/sxjgl_lxgs_320300011050204007720/20191021/sxjgl_lxgs_320300011050204007720_20191021_1571634478275_002878.jpg\",\"engineType\":\"sjk-beichuang-lpa\",\"engineId\":\"beichuang_01\",\"plateNo\":\"苏MF8U21\",\"plateColor\":\"3\",\"vehicleColor\":\"yellow\",\"vehicleType\":\"客车\",\"vehicleBrand\":\"奔驰\",\"vehicleModel\":\"S600\",\"vehicleYear\":\"2017\",\"duration\":27,\"rect\":{\"top\":865,\"left\":1048,\"bottom\":905,\"right\":1214}}]";
  CURL *curl;
  CURLcode res;
    
    struct MemoryStruct chunk;
    
    chunk.memory = (char*)malloc(1);  /* will be grown as needed by realloc above */
    chunk.size = 0;    /* no data at this point */

  struct curl_httppost *formpost = NULL;
  struct curl_httppost *lastptr = NULL;
  struct curl_slist *headerlist = NULL;
  static const char buf[] = "Expect:";

  curl_global_init(CURL_GLOBAL_ALL);

  


  /* Fill in the submit field too, even if this is rarely needed */


  curl = curl_easy_init();
  /* initialize custom header list (stating that Expect: 100-continue is not
     wanted */
  headerlist = curl_slist_append(headerlist, buf);
    
    headerlist = curl_slist_append(headerlist, "Accept: application/json");
    headerlist = curl_slist_append(headerlist, "Content-Type: application/json");
    headerlist = curl_slist_append(headerlist, "charsets: utf-8");
    
    /* send all data to this function  */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    
    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  if(curl) {
    /* what URL that receives this POST */
    curl_easy_setopt(curl, CURLOPT_URL, "http://172.31.49.252/data-collect/report/engine");
      
      /* ask libcurl to show us the verbose output */
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str_json.c_str());


    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

      printf("Fuck:\n%s\n",chunk.memory);
      
    /* always cleanup */
    curl_easy_cleanup(curl);

    /* then cleanup the formpost chain */
    curl_formfree(formpost);
    /* free slist */
    curl_slist_free_all(headerlist);
  }
  return 0;
}
