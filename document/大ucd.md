
# env

### 规划

* 后面 ucd 编译为一个大的 可执行文件，直接 ./ucd 就行，aliaes 自己配置之类的，不用在用复杂的 so 文件链接之类的

* 每一个环境生成一个 docker，发布版本的时候 ucd 在 各个环境docker 上进行编译，然后放到 80 上，这样就完美了

* ucd 链接 80 服务器那边还是要优化的

* ucd名字上说明是在哪个服务器上运行的，ucd_v3.0.6_ubuntu_16.04, ucd_v2.0.6_ubuntu_20.04
  
* ubuntu:16.04 上静态编译的在 ubuntu:20.04 上也能直接使用

* ucd 在所有server都要支持，ucd update 直接就好了，不需要在手动操作了


### 缓存文件夹所有用户都有读写权限，新创建的文件夹也如此

* setfacl -d -m u::rwx,g::rwx,o::rwx /home/disk3/ucd_cache/



### 静态编译 opencv

* wget https://github.com/opencv/opencv/archive/3.4.6.zip
* unzip ./3.4.6.zip
* 新建 build 文件夹
* cmake -D BUILD_SHARED_LIBS=OFF -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
* make -j$(nproc)
* make install


### 静态编译 openssl 

* wget https://www.openssl.org/source/openssl-1.1.1k.tar.gz
* tar -xvf openssl-1.1.1k.tar.gz
* ./config no-shared --prefix=/usr/local  , --prefix 指定安装的路径
* make depend
* make



### ubuntu:20.04 最后最大的 CMakeLists.txt 内容


```cmake

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(example-app)


# 不知道为什么要这一行，没有的话报错，to string 不是 std 的函数之类的
SET(CMAKE_CXX_FLAGS "-std=c++11 -O3")


# 指定头文件搜索路径
INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
    "/usr/include"
    "/usr/include/mysql"
)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

SET(OpenCV_STATIC_LIBS
	/usr/local/lib/libopencv_core.a
	/usr/local/lib/libopencv_imgcodecs.a
	/usr/local/lib/libopencv_imgproc.a
	/usr/lib/x86_64-linux-gnu/libpng16.a
	/usr/lib/x86_64-linux-gnu/libjpeg.a
	)

SET(other_STATIC_LIBS
	/usr/lib/gcc/x86_64-linux-gnu/9/libstdc++.a
	)



# 动态库文件目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
# 可执行文件目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)



# mysql -------------------------------------------------------

set(MYSQL_INCLUDE_DIR "/usr/include/mysql")

set(MYSQL_LIB_DIR "/usr/lib/x86_64-linux-gnu")

include_directories(${MYSQL_INCLUDE_DIR})


find_package(OpenCV REQUIRED )
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)


find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)



include_directories(${OPENSSL_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCLIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/tools SRCLIST)



# openssl -------------------------------------------------------

set(OPENSSL_INCLUDE_DIR "/usr/include/openssl")
set(OPENSSL_LIB_DIR "/usr/lib/x86_64-linux-gnu")
include_directories(${OPENSSL_INCLUDE_DIR})

# -------------------------------------------------------


# add_library(saturntools_v3.0.6 STATIC ${SRCLIST})   
add_library(saturntools_v3.0.7 STATIC ${SRCLIST})   

add_executable(ucd main.cpp)
link_directories(${OpenCV_LIB_DIR})


# 设置要链接的 glibc 库路径和名称
if (${CMAKE_SYSTEM_VERSION} VERSION_LESS "20.04")
    # Ubuntu 16.04
    set(GLIBC_LIBS "/path/to/glibc1.1/libc.so;/path/to/glibc1.2/libc.so")
else()
    # Ubuntu 20.04
    set(GLIBC_LIBS "/usr/lib/x86_64-linux-gnu/libc.so.6;/usr/lib/x86_64-linux-gnu/libc.so")
endif()


target_link_libraries(
    ucd 
	
	${OpenCV_LIBS}
    saturntools_v3.0.7
	nlohmann_json::nlohmann_json
	
	${OpenCV_STATIC_LIBS}
	${CMAKE_DL_LIBS}
	${JPEG_LIBRARIES}
	PNG::PNG
		

	${MYSQL_LIB_DIR}/libmysqlclient.a
	
	${OPENSSL_LIB_DIR}/libssl.a 
	${OPENSSL_LIB_DIR}/libcrypto.a
	
	${other_STATIC_LIBS}
	
  
    # Link against libgcc_s.a
    -static-libgcc
  
	
)


set_property(TARGET ucd PROPERTY CXX_STANDARD 14)
```



### ubuntu:16.04 最后的CMakeLists.txt 

```cmake

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(example-app)


# 不知道为什么要这一行，没有的话报错，to string 不是 std 的函数之类的
SET(CMAKE_CXX_FLAGS "-std=c++11 -O3")


# 指定头文件搜索路径
INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
    "/usr/include"
    "/usr/include/mysql"
	"/usr/lib/x86_64-linux-gnu"
)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

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



# 动态库文件目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
# 可执行文件目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)



# mysql -------------------------------------------------------

set(MYSQL_INCLUDE_DIR "/usr/include/mysql")

set(MYSQL_LIB_DIR "/usr/lib/x86_64-linux-gnu")

include_directories(${MYSQL_INCLUDE_DIR})


find_package(OpenCV REQUIRED )
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)


find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)



include_directories(${OPENSSL_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCLIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/tools SRCLIST)



# openssl -------------------------------------------------------

set(OPENSSL_INCLUDE_DIR "/usr/include/openssl")
set(OPENSSL_LIB_DIR "/usr/lib/x86_64-linux-gnu")
include_directories(${OPENSSL_INCLUDE_DIR})

# -------------------------------------------------------


# add_library(saturntools_v3.0.6 STATIC ${SRCLIST})   
add_library(saturntools_v3.0.7 STATIC ${SRCLIST})   

add_executable(ucd main.cpp)
link_directories(${OpenCV_LIB_DIR})


# 设置要链接的 glibc 库路径和名称
if (${CMAKE_SYSTEM_VERSION} VERSION_LESS "20.04")
    # Ubuntu 16.04
    set(GLIBC_LIBS "/path/to/glibc1.1/libc.so;/path/to/glibc1.2/libc.so")
else()
    # Ubuntu 20.04
    set(GLIBC_LIBS "/usr/lib/x86_64-linux-gnu/libc.so.6;/usr/lib/x86_64-linux-gnu/libc.so")
endif()


target_link_libraries(
    ucd 
	
	${OpenCV_LIBS}
    saturntools_v3.0.7
	nlohmann_json::nlohmann_json
	
	${OpenCV_STATIC_LIBS}
	${CMAKE_DL_LIBS}
	${JPEG_LIBRARIES}
	PNG::PNG
		

	${MYSQL_LIB_DIR}/libmysqlclient.a
	
	${OPENSSL_LIB_DIR}/libssl.a 
	${OPENSSL_LIB_DIR}/libcrypto.a
	
	${other_STATIC_LIBS}
	
  
    # Link against libgcc_s.a
    -static-libgcc
  
	
)


set_property(TARGET ucd PROPERTY CXX_STANDARD 14)

```
