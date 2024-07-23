#ifndef _CONFIG_FILE_HPP_
#define _CONFIG_FILE_HPP_

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include <string.h>
#include"./xini_file.h"

using namespace std;

// refer : https://gitee.com/qinwanlin/inifile


// 分割字符串
vector<string> split_str(const string& str, const string& delim)
{
    vector<string> res;
	if("" == str) return res;
	char * strs = new char[str.length() + 1] ;
	strcpy(strs, str.c_str()); 
 
	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());
 
	char *p = strtok(strs, d);
	while(p) {
		string s = p;
		res.push_back(s);
		p = strtok(NULL, d);
	}
    return res;
}

// 读取 config 文件信息
// (1) 指定标签读取为指定类型，classes 读取为 str 类型 conf 读取为 float 类型，其余目标读取为 str 类型
// (2) 读取为一个字典结构，或者一个 struct 结构，所有的关键字都作为属性进行存放

class ConfigFile
{
    // 属性设置先按照 yolov5 的来，后续的模型再加上就行
    public:
    // 
    vector<string> classes; 
    vector<string> visible_classes;
    int img_size;
    float conf_threshold;
    float iou_threshold;
    bool augment;
    std::string model_path;
    //
    void parse_config(string config_path, string option);
    void print_info();
};

// 解析配置文件
void ConfigFile::parse_config(string config_path, string option){

    // 解析 config 文件
    // 拿到指定 option 下面的内容

    xini_file_t xini_file(config_path);

    // classes
    string classes_str = (const std::string &)xini_file[option]["classes"];
    ConfigFile::classes = split_str(classes_str, ",");

    // model_path
    string model_path = (const std::string &)xini_file[option]["model_path"];
    ConfigFile::model_path = model_path;

    // visible_classes
    string visible_classes_str = (const std::string &)xini_file[option]["classes"];
    ConfigFile::visible_classes = split_str(visible_classes_str, ",");

    // img_size
    ConfigFile::img_size = (int)xini_file[option]["img_size"];

    // conf_threshold
    ConfigFile::conf_threshold = (float)xini_file[option]["conf_threshold"];

    // augment
    ConfigFile::augment = (bool)xini_file[option]["augment"];

}

// 打印配置信息
void ConfigFile::print_info()
{
    // classes
    std::cout << "--------------------------------" << std::endl;
    std::cout << "classes : ";
    for (int i=0; i< ConfigFile::classes.size(); i++)
    {
        std::cout << ConfigFile::classes[i] << ",";
    }
	std::cout << "" << std::endl;

    // visible_classes
    std::cout << "visible_classes : ";
    for (int i=0; i< ConfigFile::visible_classes.size(); i++)
    {
        std::cout << ConfigFile::visible_classes[i] << ",";
    }
	std::cout << "" << std::endl;

    // img_size
    std::cout << "img_size : " << ConfigFile::img_size << std::endl;

    // model_path
    std::cout << "model_path : " << ConfigFile::model_path << std::endl;

    // conf_threshold
    std::cout << "conf_threshold : " << ConfigFile::conf_threshold << std::endl;

    // augment
    std::cout << "augment : " << ConfigFile::augment << std::endl;
    std::cout << "--------------------------------" << std::endl;

}

#endif  