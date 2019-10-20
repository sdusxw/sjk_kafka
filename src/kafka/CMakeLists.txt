include_directories(
	${SOLUTION_INCLUDE_DIRS}
	/usr/local/include/cppkafka/
)
link_directories(
    /usr/local/lib
    /usr/local/lib64
)

#add_library(lpr_alg
#	rv_anpr_interface.h
#	lpr_alg.h
#	lpr_alg.cpp
#)

add_executable(sjk_master
    sjk_master.cpp
)

target_link_libraries(sjk_master pthread cppkafka common curl jsoncpp)

add_executable(kafka_consumer
	consumer.cpp
)

target_link_libraries(kafka_consumer pthread cppkafka boost_program_options)

add_executable(test_jpg_puller
	test_jpg_puller.cpp
)

target_link_libraries(test_jpg_puller pthread common curl)

add_executable(test_curl_upload
	test_curl_upload.cpp
)

target_link_libraries(test_curl_upload pthread curl)
