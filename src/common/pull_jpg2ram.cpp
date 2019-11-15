#include "pull_jpg2ram.h"

#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <string>

//CURL函数的写内存回调函数
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
                                  void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *) userp;
    
    mem->memory = (char*) realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}
/*********************类JpgPuller实现************************************/
bool JpgPuller::initialize()
{
    pthread_mutex_init(&mutex_lock, NULL);
    p_jpg_image = (char*) malloc(2000000);    //jpg图像大小不超过2M
    if (p_jpg_image)
    return true;
    else
    return false;
}
bool JpgPuller::pull_image(char * url)
{
    bool result = false;
    char char_msg[100] = "";
    std::string log_msg = "";
    pthread_mutex_lock(&mutex_lock);
    CURL *curl_handle;
    CURLcode res;
    chunk.memory = (char*) malloc(1); /* will be grown as needed by the realloc above */
    chunk.size = 0; /* no data at this point */
    jpg_size = 0;
    
    /* init the curl session */
    curl_handle = curl_easy_init();
    curl_setopt(curl_handle, CURLOPT_NOSIGNAL,1L);
    /* specify URL to get */
    char ipc_url[1024] =
    { 0 };
    sprintf(ipc_url, "%s", url);
    curl_easy_setopt(curl_handle, CURLOPT_URL, ipc_url);
    
    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION,
                     WriteMemoryCallback);
    
    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void * )&chunk);
    
    /* multi thread safe */
    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1L);
    
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 1);
    
    /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    
    /* get it! */
    res = curl_easy_perform(curl_handle);
    
    /* check for errors */
    if (res != CURLE_OK)
    {
        sprintf(char_msg, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    else
    {
        /*
         * Now, our chunk.memory points to a memory block that is chunk.size
         * bytes big and contains the remote file.
         *
         * Do something nice with it!
         */
        if (chunk.size != 0)
        {
            jpg_size = chunk.size;
            memcpy(p_jpg_image, chunk.memory, chunk.size);
            result = true;
        }
    }
    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);
    if (chunk.memory)
    free(chunk.memory);
    pthread_mutex_unlock(&mutex_lock);
    if (!result)
    {
        sprintf(char_msg, "get_snapshot failed: %s\n", url);
    }
    return result;
}

void JpgPuller::free_memory()
{
    if (p_jpg_image)
    {
        free(p_jpg_image);
        p_jpg_image = nullptr;
    }
}
/***************************end****************************************/
