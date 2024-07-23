cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(ucd-app)


# 指定头文件搜索路径
INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
    "/usr/include"
    "/usr/include/mysql"
	"/usr/lib/x86_64-linux-gnu"
	"/usr/local/include"
)

# 不知道为什么要这一行，没有的话报错，to string 不是 std 的函数之类的
SET(CMAKE_CXX_FLAGS "-std=c++11 -O3")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)


SET(OpenCV_STATIC_LIBS
	/usr/local/lib/libopencv_core.a
	/usr/local/lib/libopencv_imgcodecs.a
	/usr/local/lib/libopencv_imgproc.a
	/usr/lib/x86_64-linux-gnu/libpng.a
	/usr/lib/x86_64-linux-gnu/libjpeg.a
	)

SET(other_STATIC_LIBS
	/usr/lib/gcc/x86_64-linux-gnu/5/libstdc++.a
	/usr/lib/x86_64-linux-gnu/libjasper.a
	)

SET(MYSQL_INCLUDE_DIR 	"/usr/include/mysql")
SET(MYSQL_LIB_DIR 		"/usr/lib/x86_64-linux-gnu")
SET(OPENSSL_INCLUDE_DIR "/usr/include/openssl")
SET(OPENSSL_LIB_DIR 	"/usr/lib/x86_64-linux-gnu")
# set(CMAKE_PREFIX_PATH 	)


FIND_PACKAGE(OpenCV REQUIRED )
FIND_PACKAGE(OpenSSL REQUIRED)
FIND_PACKAGE(nlohmann_json REQUIRED)
FIND_PACKAGE(JPEG REQUIRED)
FIND_PACKAGE(PNG REQUIRED)

INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})


aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCLIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/tools SRCLIST)

# add_library(saturn_ucd_tools SHARED ${SRCLIST})   
add_library(saturn_ucd_tools STATIC ${SRCLIST})   
add_executable(ucd main.cpp)

# 可执行文件需要链接的库文件，命令将库链接到目标时，库之间的链接顺序非常重要。一般来说，库应该按照依赖关系的顺序链接。
target_link_libraries(
    ucd 
	
	${OpenCV_LIBS}
    saturn_ucd_tools
	nlohmann_json::nlohmann_json
	
	${OpenCV_STATIC_LIBS}
	${CMAKE_DL_LIBS}
	${JPEG_LIBRARIES}
	PNG::PNG

	/usr/code/include/hiredis/libhiredis.a
	# ncurses

	${MYSQL_LIB_DIR}/libmysqlclient.a
	${OPENSSL_LIB_DIR}/libssl.a 
	${OPENSSL_LIB_DIR}/libcrypto.a
	${other_STATIC_LIBS}


    # Link against libgcc_s.a
    -static-libgcc
  
)


set_property(TARGET ucd PROPERTY CXX_STANDARD 14)