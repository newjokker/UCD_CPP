
#include <iostream>
#include <string>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <regex>
#include <set>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pystring.h>
#include "../include/fileOperateUtil.hpp"

#define ERROR_COLOR         "\x1b[1;31m"    // 红色
#define HIGHTLIGHT_COLOR    "\033[1;35m"    // 品红
#define WARNNING_COLOR      "\033[1;33m"    // 橙色
#define STOP_COLOR          "\033[0m"       // 复原


bool is_file(std::string filename) 
{
    struct stat  buffer;
    return (stat (filename.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

bool is_dir(std::string filefodler) 
{
    struct stat   buffer;
    return (stat (filefodler.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

bool is_read_file(std::string file_path)
{
    if(! is_file(file_path))
    {
        return false;
    }

    if(access(file_path.c_str(), 4) != 0)
    {
        return false;
    }
    return true;
}

bool is_write_file(std::string file_path)
{
    if(! is_file(file_path))
    {
        return false;
    }
  
    if(access(file_path.c_str(), 2) != 0)
    {
        return false;
    }
    return true;
}

bool is_read_dir(std::string folder_path)
{
    if(! is_dir(folder_path))
    {
        return false;
    }

    if(access(folder_path.c_str(), 4) != 0)
    {
        return false;
    }
    return true;
}

bool is_write_dir(std::string folder_path)
{
    if(! is_dir(folder_path))
    {
        return false;
    }
  
    if(access(folder_path.c_str(), 2) != 0)
    {
        return false;
    }
    return true;
}

bool is_head_match(std::string assign_str, std::string regex_str)
{

    if(regex_str.size() <= 1)
    {
        std::cout << ERROR_COLOR << "regex_str's length need great then 1 : " << regex_str << STOP_COLOR << std::endl;
        throw "regex_str's length need great then 1 : " + regex_str;
    }

    int star_count = pystring::count(regex_str, "*");
    int assign_str_length = assign_str.size();

    if((star_count == 1) && (regex_str[0] == '*'))
    {
        int tail_str_count = regex_str.size() - 1;
        std::string tail_str = regex_str.substr(1);

        if(assign_str_length - tail_str_count < 0)
        {
            return false;
        }

        if(assign_str.substr(assign_str_length - tail_str_count, assign_str_length) == tail_str)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        std::cout << ERROR_COLOR << "need one * in regex_str's head : " << regex_str << STOP_COLOR << std::endl;
        throw "need one * in regex_str : " + regex_str;
    }
}

bool is_tail_match(std::string assign_str, std::string regex_str)
{
    if(regex_str.size() <= 1)
    {
        std::cout << ERROR_COLOR << "regex_str's length need great then 1 : " << regex_str << STOP_COLOR << std::endl;
        throw "regex_str's length need great then 1 : " + regex_str;
    }

    int star_count = pystring::count(regex_str, "*");
    int assign_str_length = assign_str.size();

    if((star_count == 1) && (regex_str[regex_str.size()-1] == '*'))
    {
        // 对比星号之外的几个字符
        int head_str_count = regex_str.size() - 1;
        std::string head_str = regex_str.substr(0, head_str_count);
        
        if(head_str_count > assign_str_length)
        {
            return false;
        }

        if(assign_str.substr(0, head_str_count) == head_str)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        std::cout << ERROR_COLOR << "need one * in regex_str's tail : " << regex_str << STOP_COLOR << std::endl;
        throw "need one * in regex_str : " + regex_str;
    }
}

bool is_match(std::string assign_str, std::string regex_str)
{
    // 目前只支持一个星号的匹配
    // 只支持星号的匹配
    // 三种情况，当星号在字符串最前面，在中间，在最后面


    if(regex_str.size() <= 1)
    {
        std::cout << ERROR_COLOR << "regex_str's length need great then 1 : " << regex_str << STOP_COLOR << std::endl;
        throw "regex_str's length need great then 1 : " + regex_str;
    }

    int star_count = pystring::count(regex_str, "*");
    int assign_str_length = assign_str.size();

    if(star_count == 1)
    {
        if(regex_str[0] == '*')
        {
            return is_head_match(assign_str, regex_str);
        }
        else if(regex_str[regex_str.size()-1] == '*')
        {
            return is_tail_match(assign_str, regex_str);
        }
        else
        {
            // 在中间的话，拆成两部分，前一部分符合 * 在尾部的规则，后一部分符合 * 在头部的规则，两部分都符合才算符合
            int star_index = pystring::index(regex_str, "*");
            std::string tail_regex = regex_str.substr(0, star_index + 1);
            std::string head_regex = regex_str.substr(star_index, assign_str_length);

            if(is_head_match(assign_str, head_regex) && is_tail_match(assign_str, tail_regex))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    else
    {
        std::cout << ERROR_COLOR << "need one * in regex_str : " << regex_str << ", " << star_count << STOP_COLOR << std::endl;
        throw "need one * in regex_str : " + regex_str;
    }
}

bool is_match_regex(std::string s, std::string regex_str)
{
    // 防止出现 abc 匹配到 
    std::regex r(regex_str);
    if(pystring::count(regex_str, "*") == 0)
    {
        if(s != regex_str)
        {
            return false;
        }
    }

    if (std::regex_search(s, r)) 
    {
        return true;
    } 
    else 
    {
        return false;
    }
}

std::string get_file_folder(std::string file_path)
{
    std::string folder = file_path.substr(0, file_path.find_last_of('/'));
    return folder;
}

std::string get_file_suffix(std::string file_path)
{
    // get file_name + suffix
    std::string file_name = get_file_name_suffix(file_path);
    // get suffix 
    auto point_index = file_name.find_last_of('.');
    if (point_index == std::string::npos)
    {
        return "";
    }
    else
    {
        std::string file_suffix = "." + file_name.substr(point_index + 1);
        return file_suffix;
    }
}

std::string get_file_name_suffix(std::string file_path)
{
    std::string file_name_suffix = file_path.substr(file_path.find_last_of('/') + 1);
    return file_name_suffix;
}

std::string get_file_name(std::string file_path)
{
    std::string file_suffix = "." + file_path.substr(file_path.find_last_of('.') + 1);
    std::string file_name_suffix = file_path.substr(file_path.find_last_of('/') + 1);
    std::string file_name = file_name_suffix.substr(0, file_name_suffix.length() - file_suffix.length());
    return file_name;
}

std::string get_folder_name(std::string folder_path)
{
    // 获取文件夹的文件名
    std::string folder_name = folder_path.substr(folder_path.find_last_of('/') + 1, folder_path.size());
    return folder_name;
}

std::vector<std::string> get_all_file_path(std::string path)
{
    std::vector<std::string> filenames;

    DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str())))
    {
        std::cout << ERROR_COLOR << "Folder doesn't Exist! " << STOP_COLOR << path << std::endl;
        throw "folder doesn't exists, " + path;           
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
        {
            if(is_file(path + "/" + ptr->d_name))
            {
                filenames.push_back(path + "/" + ptr->d_name);
            }
        }
    }
    closedir(pDir);
    return filenames;
}

std::vector<std::string> get_all_folder_path(std::string path)
{
    std::vector<std::string> folder_names;
    DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str())))
    {
        std::cout << ERROR_COLOR << "Folder doesn't Exist! " << STOP_COLOR << path << std::endl;
        throw "folder doesn't exists, " + path;           
        return folder_names;
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
        {
            if(is_dir(path + "/" + ptr->d_name))
            {
                folder_names.push_back(path + "/" + ptr->d_name);
            }
        }
    }
    closedir(pDir);
    return folder_names;
}

std::vector<std::string> filter_by_suffix(std::vector<std::string> filenames, std::set<std::string> suffixs)
{
    std::vector<std::string> new_file_names;
    for(int i=0; i<filenames.size(); i++)
    {
        std::string suffix = get_file_suffix(filenames[i]);
        if(suffixs.count(suffix) == 1)
        {
            new_file_names.push_back(filenames[i]);
        }
    }
    return new_file_names;
}

FilePath parse_file_path(std::string file_path_str)
{
    FilePath file_path;
    file_path.folder = get_file_folder(file_path_str);
    file_path.name = get_file_name(file_path_str);
    file_path.name_suffix = get_file_name_suffix(file_path_str);
    file_path.suffix = get_file_suffix(file_path_str);
    return file_path;
}

void create_folder(std::string folder_path)
{
    mkdir(folder_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    // mkdir(folder_path.c_str(), 0777);
}

static void _GetFileNames(std::string path, std::vector<std::string>& filenames)
{
    DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str())))
    {
        std::cout << ERROR_COLOR << "Folder doesn't Exist! " << STOP_COLOR << path << std::endl;
        return;
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
        {
            std::string new_path = path + "/" + ptr->d_name;
            if(is_file(new_path))
            {
                filenames.push_back(new_path);
                // std::cout << new_path << std::endl;
            }
            else if(is_dir(new_path))
            {
                _GetFileNames(new_path, filenames);
            }
        }
    }
    closedir(pDir);
}

std::vector<std::string> get_all_file_path_recursive(const std::string folder_path)
{
    // get file path
    std::vector<std::string> file_path_vector;
    _GetFileNames(folder_path, file_path_vector);

    // // print
    // for(int i=0; i<file_path_vector.size(); i++)
    // {
    //     std::cout << file_path_vector[i] << std::endl;
    // }

    return file_path_vector;
}

std::vector<std::string> get_all_file_path_recursive(const std::string folder_path, const std::set<std::string> suffixs)
{
    // get file path
    std::vector<std::string> file_path_vector;
    _GetFileNames(folder_path, file_path_vector);

    // filter by suffix
    std::vector<std::string> file_path_suffix = filter_by_suffix(file_path_vector, suffixs);

    // // print
    // for(int i=0; i<file_path_suffix.size(); i++)
    // {
    //     std::cout << file_path_suffix[i] << std::endl;
    // }

    return file_path_suffix;
}

std::vector<std::string> get_all_file_path(std::string folder_path, std::set<std::string> suffixs)
{
    // get file path
    std::vector<std::string> file_path_vector = get_all_file_path(folder_path);

    // filter by suffix
    std::vector<std::string> file_path_suffix = filter_by_suffix(file_path_vector, suffixs);

    // // print
    // for(int i=0; i<file_path_suffix.size(); i++)
    // {
    //     std::cout << file_path_suffix[i] << std::endl;
    // }

    return file_path_suffix;
}

std::string get_file_by_suffix_set(std::string folder, std::string name, std::set<std::string> suffixs)
{
    auto iter = suffixs.begin();
    while(iter != suffixs.end())
    {
        std::string file_path = folder + '/' + name + iter->data();
        if(is_file(file_path))
        {
            return file_path;
        }
        iter ++;
    }
    return "";
}

void move_file_vector_to_dir(std::vector<std::string> file_vector, std::string save_dir)
{
    if(! is_dir(save_dir))
    {
        std::cout << ERROR_COLOR << "save folder doesn't Exist! " << STOP_COLOR << std::endl;
        throw "save_dir not exists";
    }

    for(int i=0; i<file_vector.size(); i++)
    {
        std::string file_path = file_vector[i];
        std::string file_name = get_file_name_suffix(file_vector[i]);
        std::string save_path = save_dir + "/" + file_name;
        std::cout << "move " << file_path << " -> " << save_path << std::endl;
        rename(file_path.c_str(), save_path.c_str());
    }
}

void remove_file(std::string file_path)
{
    remove(file_path.c_str());
}

void copy_file(std::string src, std::string des)
{
    int len;
    char buff[1024];
	FILE *in,*out; 
	in = fopen(src.c_str(), "r+");
	out = fopen(des.c_str(), "w+");
 
	while(len = fread(buff, 1, sizeof(buff), in))
	{
		fwrite(buff, 1, len, out);
	}
    fclose(in);
    fclose(out);
}

int get_file_size(std::string file_path)
{
    struct stat statbuf;  
    stat(file_path.c_str(), &statbuf);  
    int size=statbuf.st_size;  
    return size;  
}




