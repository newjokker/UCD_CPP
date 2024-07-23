# CMake 学习


### 动态链接库和静态链接库

* refer : https://www.runoob.com/w3cnote/cpp-static-library-and-dynamic-library.html

* 静态

  * g++ -c hello.cpp                  // 生成 .o 文件

  * ar -crv libhello.a hello.o        // 生成 lib**.a 文件

  * g++ main.cpp -L../build -lhello   // 生成可执行文件


* 动态

  * g++ -fPIC -shared -o libhello.so hello.cpp

  * g++ main.cpp -L../build -lhello

  * 执行报错，因为：当系统加载可执行代码时候，能够知道其所依赖的库的名字，但是还需要知道绝对路径 refer : https://www.runoob.com/w3cnote/cpp-static-library-and-dynamic-library.html

  * vim /etc/ld.so.conf, 添加库文件所谓目录  /home/ldq/build
  * ldconfig, 重建/etc/ld.so.cache
  * 将创建的动态库复制到/usr/lib下面，然后运行测试程序


### CMake 设置静态链接库

* 我之前一直运行不成功是因为，CMakeList.txt 中编译动态库和调用动态库是分开做的，我合并起来了，所以一直报错

* 生成静态库，注意
```markdown
cmake_minimum_required(VERSION 3.5)

project(
    test
    VERSION 0.0.1 
)

//添加到函数库中去 
add_library(hello STATIC ./build/hello.cpp)

```

* 调用静态库，记得调用和生成两部分要分开来做

```markdown
cmake_minimum_required(VERSION 3.5)

project(
    test
    VERSION 0.0.1 
)

add_executable(
    test 
    main.cpp
)

target_link_libraries(test /home/ldq/build/libhello.a)

```










