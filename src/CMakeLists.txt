cmake_minimum_required(VERSION 2.8)

project(sjk_master)

set(CMAKE_CXX_STANDARD 14)

#cmake_policy(SET CMP0053 OLD)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g -O3")

#
set(CMAKE_USE_RELATIVE_PATHS ON)

#find_package(CppKafka REQUIRED)

#
set(src_dir "${PROJECT_SOURCE_DIR}")
set(bin_dir "${PROJECT_BINARY_DIR}")
set(lib_dir "${PROJECT_BINARY_DIR}")

set(SOLUTION_INCLUDE_DIRS
	${src_dir}/common
	${src_dir}/kafka
    /usr/include/jsoncpp
)
set(SOLUTION_LINK_DIRS
	/usr/local/lib64
)

SET(EXECUTABLE_OUTPUT_PATH ${bin_dir}/bin)
SET(LIBRARY_OUTPUT_PATH ${lib_dir}/lib)

# add subdirectories

add_subdirectory(common)
add_subdirectory(kafka)
#add_subdirectory(integrate)
