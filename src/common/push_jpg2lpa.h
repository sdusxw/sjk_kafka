#include <iostream>
#include <string>
#include <curl/curl.h>

using std::string;

//JpgPusher  上传图片到车辆分析引擎
class JpgPusher
{
public:
    bool initialize();
    string push_image(string url, char* p_jpg_data, int jpg_length);

private:
    pthread_mutex_t mutex_lock;        //互斥锁
};
