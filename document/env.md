# 环境相关

* verison 0.0.1
  
* update 20220802


### 环境安装

* 先在 ubuntu 镜像中测试 安装 vim 等需要的 c++ 编程环境
  * apt-get update
  * apt-get install vim
  * apt-get install build-essential : 执行完后，就完成了gcc,g++,make的安装
  * apt install cmake , cmake 安装
  
  * apt install zip
  * apt install wget，安装下载工具


* cmake 安装 
    * apt autoremove cmake，卸载 cmake
    * 官网下载 cmake 编译，安装，refer: https://segmentfault.com/a/1190000021911081, cmake 官网 https://cmake.org/download/
    * apt-get install libssl-dev, cmake 需要安装 openssl
    * ./bootstrap --prefix=/usr/local/cmake, 编译 cmake
    * make，编译
    * make install，安装
    * alias cmake=/usr/local/cmake/bin/cmake， 成功之后，在bash_aliases加个别名， 

* 安装 opencv 环境
  * refer : https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html 
  * 去官网下载 opencv-3.4.6 refer : https://opencv.org/releases/page/3/
  * 安装依赖环境
    * apt-get install build-essential libgtk2.0-dev libavcodec-dev libavformat-dev libjpeg.dev libtiff4.dev libswscale-dev libjasper-dev
    * 安装 cmake
    * apt install wget，安装下载工具
  * mkdir build
  * cd build
  * cmake ..
  * make 
  * make install
  * 测试 opencv 是否安装成功 refer : https://blog.csdn.net/wuconghao2016/article/details/77050725
    * 将代码中的展示图片改为显示图片的大小即可，在 linux 中无法显示图片

* 安装 libtorch 环境
  * 下载
    * refer ： https://pytorch.org/
    * 可以选择 CPU 版本或者 GPU 版本 


* 安装 curl 用于下载图片
  * apt install libcurlpp-dev
  * apt-get install libcurl4-openssl-dev
  * 上面两个命令都要运行，不知道是什么原因

* 安装 tinyxml 解析 xml
  * apt install libtinyxml2-dev:amd64， 安装依赖库
  * CMakeList.txt 连接 tinyxml 库

    ```markdown
    target_link_libraries(
      example-app 
      tinyxml2
    )
    ``` 
  * 在 cpp 中使用

    ```CPP
    // 其中的 .h 别忘了
    #include <tinyxml2.h>
    ```

* crow 安装（需要先安装 boost）
  * 为了提供网络服务 
  * git, https://github.com/ipkn/crow
  * 头文件下载地址 https://github.com/ipkn/crow/releases/download/v0.1/crow_all.h
  * 直接引用这个头文件即可

* boost 的安装
  * boost 的安装，为了使用 crow 库，仿照 flask 写的 C++ 库，refer : https://www.cnblogs.com/liushui-sky/p/12843679.html
  * boost 只能使用低版本的（< boost 1.70）refer : https://github.com/rstudio/rstudio/issues/4636
  * boost 的写在，refer : https://blog.csdn.net/qq_26849233/article/details/123511752
  
* nlohmann 安装
  * 为了处理生成 json str
  * git, https://github.com/nlohmann/json
  * 安装：refer : https://zhuanlan.zhihu.com/p/359646630
  * 将 git 下载到目录下之后
    * mkdir build 
    * cd build 
    * cmake ..
    * make 
    * make install 
    * 配置 CMakeLists.txt （1）find_package(nlohmann_json REQUIRED) （2）target_link_libraries(XXX nlohmann_json::nlohmann_json) 

* openssl 安装和连接
  * apt install 安装即可
  * 连接，refer : https://wuruofan.com/2020/05/24/cmake-static-link-openssl-curl/

* cpp-httplib 安装
  * 访问网页
  * 提供文件服务
  * 下载图片 
  * refer : https://github.com/yhirose/cpp-httplib 
  * 下载好 git 之后 (1) mkdir build (2) cmake .. (3) make install 即可 

* mysql 安装
  * apt-get install libmysqld-dev
  * 

### 环境安装的通用步骤

* apt-cache search pkg_name ，去找可以安装的库的名字

* apt install pkg_name ，安装库

* 在 CMakeLists.txt 中链接库  target_link_libraries(${PROJECT_NAME} pkg_name)

* 如果上面操作不能成功的话

  * 安装一下查询的到其他的类似的库看下

  * 查询一下库所在的目录 find / -name "*pkg_name*"
    * 在 CMakeLists.txt 中添加对应的头文件查找目录，INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR} 文件的头文件所在的目录
    * set(CMAKE_PREFIX_PATH  ${CMAKE_PREFIX_PATH} 文件的头文件所在的目录 )
  


### docker 环境的配置

* 使用的镜像 pytorch/libtorch-cxx11-builder:cuda10.2
  * cmake 已经安装好了
  * CMakeLists.txt
    * set(CMAKE_PREFIX_PATH  ${CMAKE_PREFIX_PATH} "/usr/del/libtorch" ) 
    * target_link_libraries(example-app ${TORCH_LIBRARIES})
    * find_package( OpenCV REQUIRED )

* 安装 opencv 
  * 在尝试使用已经编译好的 opencv 不用自己再去编译
  
* ldd 可执行文件路径，查看依赖的动态库

* 减小 docker 体积的方法
  * 动态库全部复制到一个文件夹下
  * export LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH， 执行这个命令
  * 执行 可执行文件即可，我测试过，不会报错

### opencv 使用需要的配置

* CMakeLists.txt 增加如下几行

  ```markdown

  //
  find_package( OpenCV REQUIRED )

  // 链接 OpenCV_LIBS  
  target_link_libraries(
      example-app 
      ${OpenCV_LIBS}
  )
  ```

* 代码中增加引用

  ```CPP
  #include <opencv2/opencv.hpp>
  using namespace cv;

  Mat image;
  image = imread("/usr/del/test.jpg", 1 );
  std::cout << "img size : " << image.size() << std::endl;

  ```

### hiredis 的安装

* https://blog.csdn.net/qq_41134622/article/details/120626744

* target_link_libraries 增加一行即可 /usr/code/include/hiredis/libhiredis.a

* redis 无法从其他服务器访问，起来redis 对外访问权限即可，改一下配置文件

### libtorch 使用需要的配置

* CMakeLists.txt 需要增加的部分

  ```markdown

  // 配置上 libtorch 的查找目录
  set(CMAKE_PREFIX_PATH  ${CMAKE_PREFIX_PATH} "~/libtorch/libtorch")

  // 查找
  find_package(Torch REQUIRED)

  // 链接
  target_link_libraries(
      example-app 
      "${TORCH_LIBRARIES}"
  )
  ```

### libcurl 配置

* CMakeLists.txt 中增加的部分

  ```markdown
  target_link_libraries(
      example-app 
      ${TORCH_LIBRARIES}
      ${OpenCV_LIBS}
      curlpp
      #utilspp
      curl
  )
  ```

*  cpp 使用例子

    ```CPP
    #include <curlpp/cURLpp.hpp>
    #include <curlpp/Easy.hpp>
    #include <curlpp/Options.hpp>
    #include <iostream>
    #include <unistd.h>

    // using namespace curlpp::options;


    void ImageDownloader(std::string image_url, std::string save_address)
    {
        CURL* curl;
        CURLcode res;

        curl = curl_easy_init();

        FILE* fp = fopen(save_address.c_str(), "wb");
        res = curl_easy_setopt(curl, CURLOPT_URL, image_url.c_str());
        if(res != CURLE_OK)
        {
            curl_easy_cleanup(curl);
            return;
        }

        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        res = curl_easy_perform(curl);
        fclose(fp);

        curl_easy_cleanup(curl);
    }
    ```



### configure, cmake, make, make install 的区别

* cinfigure, 用于生成 Makefile
* cmake 根据 CMakeLists.txt 生成 Makefile
* make 执行 Makefile
* make install 将可执行文件、第三方依赖包和文档复制到正确的路径


### 完整的 CMakeLists.txt


```cmake

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(example-app)


# 不知道为什么要这一行，没有的话报错，to string 不是 std 的函数之类的
SET(CMAKE_CXX_FLAGS "-std=c++11 -O3")


# 指定头文件搜索路径
INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
     "/usr/include"
	"/usr/del/example-app/tools/"
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")


#find_package( CUDA REQUIRED )

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)


# 设置静态库文件目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
# 动态库文件目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
# 可执行文件目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)


#set(CMAKE_PREFIX_PATH  ${CMAKE_PREFIX_PATH} "~/libtorch/libtorch")
set(CMAKE_PREFIX_PATH  ${CMAKE_PREFIX_PATH} "/home/libtorch" )
set(CMAKE_PREFIX_PATH  ${CMAKE_PREFIX_PATH} "/usr/local/cuda-10.2" )

find_package(Torch REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# SET(Torch_DIR /usr/del/torch/share/cmake/Torch)

find_package( OpenCV REQUIRED )

FIND_PACKAGE(nlohmann_json REQUIRED)


add_executable(example-app main.cpp)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCLIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/tools SRCLIST)

add_library(saturntools SHARED ${SRCLIST})   

target_link_libraries(
    example-app 
    ${TORCH_LIBRARIES}
    ${OpenCV_LIBS}
    curlpp
    curl
    tinyxml2
    saturntools
	nlohmann_json::nlohmann_json
)
    
set_property(TARGET example-app PROPERTY CXX_STANDARD 14)

MESSAGE(STATUS, "------------------------------------------------------------")

MESSAGE("${TORCH_LIBRARIES}")

MESSAGE("${OpenCV_LIBS}")


MESSAGE("${PROJECT_SOURCE_DIR}")

MESSAGE("${TORCH_INCLUDE_DIRS}")

MESSAGE("${OpenCV_INCLUDE_DIRS}")

MESSAGE(STATUS, "------------------------------------------------------------")


```

### 读取指定路径下的动态库

* 指定读取 /home/ldq/Apps_jokker 下面的动态库在有些地方报错，不知道原因
* https://blog.csdn.net/bandaoyu/article/details/113181179?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-113181179-blog-89405715.pc_relevant_3mothn_strategy_recovery&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-113181179-blog-89405715.pc_relevant_3mothn_strategy_recovery&utm_relevant_index=1
* 如果是段错误。将 /home/ldq/Apps_jokker/ 这个路径改名字就不会出现段错误，会出现找不到动态库，再用下面的方法试一下
* cd /etc/ld.so.conf.d
* vim ucd.conf
* 增加要检查的路径 /home/ldq/Apps_jokker/so_dir
* sudo ldconfig 执行命令生效

* 写一个自动化的代码（1）指定安装目录 （2）指定安装的名字，如果已经有同样的名字询问是否覆盖安装
* 写一个自动化脚本
  * 检查指定的库文件和执行文件是否存在，指定文件是否符合 ucd_version 的样式
  * 检查文件夹是否存在 /etc/ld.so.conf.d
  * 检查指定的文件名是否已经有了配置文件，有了的话询问是否重复安装
  * 增加 ucd.conf 文件，写上需要增加的内容
  * 将动态库对应的内容拷贝到指定的安装文件夹下面
  * 指定 ldconfig 命令（不知道脚本内部是否能执行，不能的话提示在外面进行执行）
  * 提示需要使用 alias 修改别名
  
* 


### 最小的 docker 

* 在一个 docker 上编译代码 : nvidia/cuda:10.2-cudnn7-devel-ubuntu16.04
* 在一个 docker 上运行代码 : nvidia/cuda:10.2-cudnn7-runtime-ubuntu16.04

* 编译后的代码可以在运行的环境跑有两个方案
  * 在运行的环境上装上 (1) curlpp (2) tinyxml (3) opencv (4) cmake  等依赖包
  * 将使用到 opencv 的代码打包成静态代码，直接执行，这样会更小，但是运行环境中不能直接调试

* 运行环境中的 cudnn 是阉割版的，所以 nvcc -V 命令不能显示 cuda 版本，编译环境中的不是阉割版本，所以可以使用 nvcc -V 命令

#### stage 1

  * apt-get update
  * apt-get install build-essential
  * 切换到 cmake 安装包，安装 cmake 最新版本
  * todo 编译运行代码

#### stage 2

  * 运行环境中安装好必须的环境
  * 编译好的代码拷贝到运行环境中的相同目录下



## ucd 编译为一个大的文件

### 编译 opencv

* 新建 build 文件夹
* cmake -D BUILD_SHARED_LIBS=OFF -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
* make -j$(nproc)
* sudo make install


> CMakeLists.txt 中为 
  cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
  project(example-app)

  >
  # 不知道为什么要这一行，没有的话报错，to string 不是 std 的函数之类的
  SET(CMAKE_CXX_FLAGS "-std=c++11 -O3")


  # 指定头文件搜索路径
  INCLUDE_DIRECTORIES(
      ${PROJECT_SOURCE_DIR}
      "/usr/include"
      "/usr/include/mysql"
  )


  set(MYSQL_LIBS mysqlclient pthread z m rt atomic ssl crypto dl)

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")


  SET(OpenCV_STATIC_LIBS
    /usr/local/lib/libopencv_core.a
    /usr/local/lib/libopencv_imgcodecs.a
    /usr/local/lib/libopencv_imgproc.a
    )



  # 动态库文件目录
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
  # 可执行文件目录
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)


  find_package(OpenCV REQUIRED )
  find_package(OpenSSL REQUIRED)
  find_package(nlohmann_json REQUIRED)



  find_package(JPEG REQUIRED)
  find_package(PNG REQUIRED)


  include_directories(${OPENSSL_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


  aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCLIST)
  aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/tools SRCLIST)



  # add_library(saturntools_v3.0.6 STATIC ${SRCLIST})   
  add_library(saturntools_v3.0.7 STATIC ${SRCLIST})   

  add_executable(ucd main.cpp)
  link_directories(${OpenCV_LIB_DIR})



  # 增加关键字 第二位 +1
  # 修改关键字 第三位 +1
  target_link_libraries(
      ucd 
      
    ${OpenCV_LIBS}
      saturntools_v3.0.7
    OpenSSL::Crypto OpenSSL::SSL
    nlohmann_json::nlohmann_json
    
    ${OpenCV_STATIC_LIBS}
    ${CMAKE_DL_LIBS}
    ${JPEG_LIBRARIES}
    PNG::PNG
    
    ${MYSQL_LIBS}

  )


  set_property(TARGET ucd PROPERTY CXX_STANDARD 14)




