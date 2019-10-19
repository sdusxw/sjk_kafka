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

add_executable(kafka_consumer
	consumer.cpp
)

target_link_libraries(kafka_consumer pthread cppkafka boost_program_options)


