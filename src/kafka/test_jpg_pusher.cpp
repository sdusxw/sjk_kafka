#include "common.h"
#include "push_jpg2lpa.h"
#include <stdio.h>

#include <iostream>

using std::string;

int main(int argc, char ** argv)
{
    FILE *fp;
    fp = fopen("1.jpg", "rb");
    
    printf("fp:\t%x\n", fp);
    
    int f_len = 362740;
    
    char * p_data = (char*)malloc(f_len);
    
    fread(p_data, f_len, 1, fp);
    
    printf("file length:\t%d\n", f_len);
    JpgPusher jp;
    jp.initialize();
    string url = "http://127.0.0.1:81/chpAnalyze";
    string curl_return = jp.push_image(url, p_data, f_len);
    {
        std::cout << "Fuck->\n" << curl_return << std::endl;
    }
    
    return 0;
}
