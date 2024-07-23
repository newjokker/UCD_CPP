# 说明

### todo

* 221 服务器 txkj 下面安装 conda 
* txkj 下面重新按照要求安装 py36 环境，不要拷贝 root 下面的环境
* txkj 下面 pip install cmake 


### 编译和执行代码

* rm -rf build          # 清空 build 
* cmake -B build        # 生成 Makefile 文件在 ./build 下面
* cmake --build build   # 进行编译，生成的可执行文件在 ./bin 下面

### 环境配置

* conda activate py37

### gdb 调试

* gdb 执行文件目录
* run
* bt
* ctrl + d 退出

### 查看对象的类型

```CPP
cout << typeid(a).name() << endl; // int
cout << typeid(&b).name() << endl;// int const *
cout << typeid(e).name() << endl; // int
cout << typeid(d).name() << endl; // int
cout << typeid(c).name() << endl; // class MyClass
cout << typeid(s).name() << endl; // Struct MyStruct
```


