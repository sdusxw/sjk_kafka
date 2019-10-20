#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
int main(int argc, char *argv[])
{
  CURL *curl;
  CURLcode res;

  struct curl_httppost *formpost = NULL;
  struct curl_httppost *lastptr = NULL;
  struct curl_slist *headerlist = NULL;
  static const char buf[] = "Expect:";

  curl_global_init(CURL_GLOBAL_ALL);
    
    FILE *fp;
    fp = fopen("1.jpg", "rb");
    
    printf("fp:\t%x\n", fp);
    
    int f_len = 362740;
    
    char * p_data = (char*)malloc(f_len);
    
    fread(p_data, f_len, 1, fp);
    
    printf("file length:\t%d\n", f_len);

  /* Fill in the file upload field */
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "file",
                 CURLFORM_BUFFER, "1.jpg",
                 CURLFORM_BUFFERPTR, p_data,
                 CURLFORM_BUFFERLENGTH, f_len,
                 CURLFORM_END);

  


  /* Fill in the submit field too, even if this is rarely needed */


  curl = curl_easy_init();
  /* initialize custom header list (stating that Expect: 100-continue is not
     wanted */
  headerlist = curl_slist_append(headerlist, buf);
  if(curl) {
    /* what URL that receives this POST */
    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:81/chpAnalyze");
    
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);

    /* then cleanup the formpost chain */
    curl_formfree(formpost);
    /* free slist */
    curl_slist_free_all(headerlist);
  }
  return 0;
}
