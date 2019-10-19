#include <stdexcept>
#include <iostream>
#include <csignal>
#include <time.h>
#include <boost/thread.hpp>

#include <json/json.h>  //jsoncpp

#include "cppkafka/consumer.h"
#include "cppkafka/configuration.h"

#include "common.h"
#include "pull_jpg2ram.h"
#include "concurrent_queue.h"

using std::string;
using std::exception;
using std::cout;
using std::endl;

using cppkafka::Consumer;
using cppkafka::Configuration;
using cppkafka::Message;
using cppkafka::TopicPartitionList;

bool running = true;

concurrent_queue<string> g_queue_jpg_msg;

boost::thread thread_jpg_msg_handler;

void task_jpg_handler()
{
    while (true) {
        string msg_jpg;
        g_queue_jpg_msg.wait_and_pop(msg_jpg);
        cout << "Processing\t" << msg_jpg << endl;
        Json::Reader reader;
        Json::Value json_object;
        
        if (!reader.parse(msg_jpg, json_object))
        {
            //JSON格式错误导致解析失败
            cout << "[json]解析失败" << endl;
            continue;
        }
        
        //处理kafka的Topic2消息
        string string_img_url = json_object["imgURL"].asString();
        //下载图片
        JpgPuller jp;
        jp.initialize();
        if(jp.pull_image(string_img_url.c_str()))
        {
            printf("OK, jpg size:\t%d", (int)jp.jpg_size);
        }else{
            printf("Pull jpg error");
        }
        jp.free_memory();
    }
}

//./kafka_consumer -b 172.31.3.1:9092,172.31.3.2:9092,172.31.3.3:9092 -t handledImg-topic -g sjk-beichuang-lpa

int main(int argc, char* argv[]) {
    string brokers;
    string topic_name;
    string group_id;

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
