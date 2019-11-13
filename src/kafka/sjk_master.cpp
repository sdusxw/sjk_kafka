#include <stdexcept>
#include <iostream>
#include <csignal>
#include <time.h>
#include <boost/thread.hpp>
#include <curl/curl.h>
#include <json/json.h>  //jsoncpp

#include "cppkafka/consumer.h"
#include "cppkafka/configuration.h"

#include "common.h"
#include "pull_jpg2ram.h"
#include "push_jpg2lpa.h"
#include "concurrent_queue.h"

using std::string;
using std::exception;
using std::cout;
using std::endl;

using cppkafka::Consumer;
using cppkafka::Configuration;
using cppkafka::Message;
using cppkafka::TopicPartitionList;

typedef struct
{
    int msg_len;
    char message[2048];                     //JSON消息
}mesg, *p_mesg;

bool running = true;

concurrent_queue<string> g_queue_jpg_msg;

boost::thread thread_jpg_msg_handler;

void * alpr_handle(void *arg);

bool push_result(string url, string json_result)
{
    
}

void task_jpg_handler()
{
    while (true) {
        string msg_jpg;
        g_queue_jpg_msg.wait_and_pop(msg_jpg);
        cout << "Processing\t" << msg_jpg << endl;
        
        p_mesg pms = (p_mesg)malloc(sizeof(mesg));
        memcpy(pms->message, msg_jpg.c_str(), msg_jpg.length());
        pms->message[msg_jpg.length()] = '\0';
        pms->msg_len = msg_jpg.length();
        
        pthread_t tid_msg_handle;
        pthread_create(&tid_msg_handle,NULL,alpr_handle, pms);
        pthread_detach(tid_msg_handle);
    }
}

void * alpr_handle(void *arg)
{
    //消息接收时间戳
    long receivetime=get_unix_ts_ms();
    JpgPusher pusher;
    pusher.initialize();
    p_mesg pms = (p_mesg)arg;
    std::string msg_jpg = std::string(pms->message, pms->msg_len);
    Json::Reader reader;
    Json::Value json_object;
    
    if (!reader.parse(msg_jpg, json_object))
    {
        //JSON格式错误导致解析失败
        cout << "[json]Kafka Topic2解析失败" << endl;
        return nullptr;
    }
    
    //处理kafka的Topic2消息
    string string_img_url = json_object["imgURL"].asString();
    string passId = json_object["passId"].asString();
    string path = json_object["path"].asString();
    //下载图片
    JpgPuller jp;
    jp.initialize();
    if(jp.pull_image((char*)string_img_url.c_str()))
    {
        //上传图片到车辆分析引擎
        string url = "http://127.0.0.1:80/chpAnalyze";
        string res = pusher.push_image(url, jp.p_jpg_image, jp.jpg_size);
        cout << "Lpa Res->\n" << res << endl;
        Json::Reader res_reader;
        Json::Value res_json_object;
        if (!res_reader.parse(res, res_json_object))
        {
            //JSON格式错误导致解析失败
            cout << "[json]识别引擎返回消息解析失败" << endl;
            jp.free_memory();
            return nullptr;
        }
        //分析引擎返回结果
        Json::Value arrayObj = res_json_object["results"];
        int code = arrayObj[0]["code"].asInt();
        string plateNo = "NO_PLATE";
        int plateColor = 3;
        if(code == 0)
        {
            Json::Value result = arrayObj[0]["result"];
            plateNo = result["vehlic"].asString();
            plateColor = result["lic_color"].asInt();
        }
        Json::Value json_res;
        Json::StreamWriterBuilder writerBuilder;
        std::ostringstream os;
        //消息发送时间戳
        long sendtime = get_unix_ts_ms();
        Json::Value json_result;
        json_result["passId"]=passId;
        json_result["receivetime"]=(Json::Value::UInt64)receivetime;
        json_result["sendtime"]=sendtime;
        json_result["path"]=path;
        json_result["engineType"]="sjk-beichuang-lpa";
        json_result["engineId"]="beichuang_01";
        json_result["plateNo"]=plateNo;
        json_result["plateColor"]=std::to_string(plateColor);
        json_result["vehicleType"]="SUV";
        json_result["vehicleBrand"]="rolls-royce";
        json_result["vehicleModel"]="cullinan";
        json_result["vehicleYear"]="2035";
        json_result["vehicleColor"]="2";
        json_result["duration"]=(Json::Value::UInt64)(sendtime-receivetime);
        
        //
        json_res.append(json_result);
        std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
        jsonWriter->write(json_res, &os);
        string alpr_body = os.str();
        cout << "Send:\n" << alpr_body << endl;
        
    }else{
        printf("Pull jpg error");
    }
    jp.free_memory();
}

//./kafka_consumer -b 172.31.3.1:9092,172.31.3.2:9092,172.31.3.3:9092 -t handledImg-topic -g sjk-beichuang-lpa

int main(int argc, char* argv[]) {
    string brokers;
    string topic_name;
    string group_id;
    
    //初始化curl环境
    curl_global_init(CURL_GLOBAL_ALL);

    // Stop processing on SIGINT
    signal(SIGINT, [](int) { running = false; });
    
    thread_jpg_msg_handler = boost::thread(boost::bind(&task_jpg_handler));
    
    brokers = "172.31.3.1:9092,172.31.3.2:9092,172.31.3.3:9092";
    topic_name = "handledImg-topic";
    group_id = "sjk-beichuang-lpa";

    // Construct the configuration
    Configuration config = {
        { "metadata.broker.list", brokers },
        { "group.id", group_id },
        // Disable auto commit
        { "enable.auto.commit", false }
    };

    // Create the consumer
    Consumer consumer(config);

    // Print the assigned partitions on assignment
    consumer.set_assignment_callback([](const TopicPartitionList& partitions) {
        cout << "Got assigned: " << partitions << endl;
    });

    // Print the revoked partitions on revocation
    consumer.set_revocation_callback([](const TopicPartitionList& partitions) {
        cout << "Got revoked: " << partitions << endl;
    });

    // Subscribe to the topic
    consumer.subscribe({ topic_name });

    cout << "Consuming messages from topic " << topic_name << endl;

    // Now read lines and write them into kafka
    while (running) {
        // Try to consume a message
        Message msg = consumer.poll();
        if (msg) {
            // If we managed to get a message
            if (msg.get_error()) {
                // Ignore EOF notifications from rdkafka
                if (!msg.is_eof()) {
                    cout << "[+] Received error notification: " << msg.get_error() << endl;
                }
            }
            else {
                // Print the payload
                //cout << msg.get_payload() << endl;
                // Push the msg to queue
                g_queue_jpg_msg.push(string(msg.get_payload()));
                // Now commit the message
                consumer.commit(msg);
            }
        }
    }
}
