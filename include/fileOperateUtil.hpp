#ifndef _FILEOPERSTEUTIL_HPP_
#define _FILEOPERSTEUTIL_HPP_


#include <string>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <string.h>
#include <sys/stat.h>

// 文件路径
struct FilePath
{
    std::string folder;
    std::string name;
    std::string name_suffix;
    std::string suffix;
};

bool is_file(std::string);
bool is_dir(std::string);

// 文件是否能写入
bool is_read_file(std::string);

// 文件对否能读取
bool is_write_file(std::string);

// 文件夹是否能存在，同时可读取
bool is_read_dir(std::string);

// 文件夹是否存在，同时可写入
bool is_write_dir(std::string);

// 字符串是否匹配 a 是被匹配的句子，b 是带通配符
bool is_match(std::string a, std::string b);
bool is_match_regex(std::string a, std::string regex_str);
bool is_head_match(std::string a, std::string b);
bool is_tail_match(std::string a, std::string b);
std::string get_file_folder(std::string);
std::string get_file_suffix(std::string);
std::string get_file_name(std::string);
std::string get_file_name_suffix(std::string);
std::string get_folder_name(std::string);


std::vector<std::string> get_all_file_path(std::string);
std::vector<std::string> get_all_file_path(std::string, std::set<std::string>);
std::vector<std::string> get_all_file_path_recursive(std::string);
std::vector<std::string> get_all_file_path_recursive(std::string, std::set<std::string>);
std::vector<std::string> get_all_folder_path(std::string);
std::vector<std::string> filter_by_suffix(std::vector<std::string>, std::set<std::string>);

// 解析文件路径，返回 FilePath 结构体，可以方便拿到文件路径的各个信息
FilePath parse_file_path(std::string);

// 创建文件夹
void create_folder(std::string);

// 拷贝文件
void copy_file(std::string src, std::string des);

// just use rename 
void move_file(const std::string& src, const std::string& dst, const bool keep_src=false);

// 
void move_file_vector_to_dir(std::vector<std::string> file_vector, std::string save_dir);

// 删除文件
void remove_file(std::string);

// 获取文件大小
int get_file_size(std::string file_path);

// 判断指定后缀的文件是否存在
std::string get_file_by_suffix_set(std::string folder, std::string name, std::set<std::string>);


// 获取可执行文件的目录 rerfer : https://stackoverflow.com/questions/23943239/how-to-get-path-to-current-exe-file-on-linux


#endif

