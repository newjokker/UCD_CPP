
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "include/paramInfo.hpp"
#include "include/pystring.h"

#define ERROR_COLOR         "\x1b[1;31m"    // 红色
#define HIGHTLIGHT_COLOR    "\033[1;35m"    // 品红
#define WARNNING_COLOR      "\033[1;33m"    // 橙色
#define STOP_COLOR          "\033[0m"       // 复原


int min_edit_cost(std::string s1, std::string s2, int ic, int dc, int rc) 
{
    // s1, s2 两个字符串计算最小编辑代价，ic insert, dc delete, rc replace 
    int m=s1.length(),n=s2.length();
    std::vector<std::vector<int> > v(m+1, std::vector<int>(n+1,0));
    for(int i=0;i<m+1;i++) 
    {
        for(int j=0;j<n+1;j++) 
        {
            if(i==0 && j!=0) v[i][j]=j*ic;
            else if(i!=0 && j==0) v[i][j]=i*dc;
            else if(i==0 && j==0) v[i][j]=0;
            else if(s1[i-1]==s2[j-1]) v[i][j]=v[i-1][j-1];
            else v[i][j] = std::min(std::min(v[i-1][j]+dc,v[i][j-1]+ic),v[i-1][j-1]+rc);
        }
    }
    return v[m][n];
}

ParamInfo::ParamInfo(std::string command)
{
    ParamInfo::command = command;
}

bool ParamInfo::is_similar(std::string other_command)
{
    if(ParamInfo::command == other_command)
    {
        return true;
    }

    // 当包含这个关键字的时候也认为是相似的
    if(pystring::find(ParamInfo::command, other_command) >= 0)
    {
        return true;
    }

    // 字符串的相似程度就是计算字符串的最小编辑代价
    int mec = min_edit_cost(ParamInfo::command, other_command, 1, 1, 1.5);
    float diff_index = (float)mec / (float)ParamInfo::command.size();

    // std::cout << "diff index : " << diff_index << std::endl;

    if(diff_index < 0.6)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ParamInfo::print_info()
{
    std::cout << HIGHTLIGHT_COLOR << ParamInfo::command << STOP_COLOR << std::endl;
    std::cout << WARNNING_COLOR << "   " << ParamInfo::grammar << STOP_COLOR << std::endl;

    auto iter = ParamInfo::args_info.begin();
    while (iter != ParamInfo::args_info.end())
    {
        std::cout << WARNNING_COLOR << "     " << iter->first << "    " << iter->second << STOP_COLOR << std::endl; 
        iter++; 
    }
    
    std::cout << "   " << ParamInfo::chinese_explain << std::endl;

    if(ParamInfo::demo.size() != 0)
    {
        std::cout << "   [demo]"  << std::endl;
        for(int i=0; i<ParamInfo::demo.size(); i++)
        {
            std::cout << "   " << ParamInfo::demo[i] << std::endl;
        }
    }
}


//

ParamInfo UcdParamOpt::get_param(std::string command)
{
    if(UcdParamOpt::param_map.count(command) == 0)
    {
        std::cout << "command not register : " << command << std::endl;
        throw "command not register";
    }
    else
    {
        return UcdParamOpt::param_map[command];
    }
}

bool UcdParamOpt::has_command(std::string command)
{
    if(UcdParamOpt::param_map.count(command) == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool UcdParamOpt::has_group(std::string group)
{
    auto iter = UcdParamOpt::param_map.begin();
    while(iter != UcdParamOpt::param_map.end())
    {
        if(iter->second.group == group)
        {
            return true;
        }
        iter++;
    }
    return false;
}

bool UcdParamOpt::has_simliar_command(std::string command)
{
    std::vector<std::string> similar_command_vector = UcdParamOpt::find_similar_command(command);
    if(similar_command_vector.size() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void UcdParamOpt::add_param(ParamInfo *param_info)
{
    if(UcdParamOpt::has_command(param_info->command))
    {
        std::cout << ERROR_COLOR << "command exists : " << param_info->command << "paramInfo.cpp" << STOP_COLOR<< std::endl;
    }
    else
    {
        UcdParamOpt::param_map[param_info->command] = *param_info;
    }
}

std::vector<std::string> UcdParamOpt::find_similar_command(std::string command)
{
    std::vector<std::string> command_vector;
    auto iter = UcdParamOpt::param_map.begin();
    while (iter != UcdParamOpt::param_map.end())
    {
        if(iter->second.is_similar(command))
        {
            command_vector.push_back(iter->second.command);
        }
        iter++;
    }
    return command_vector;
}

void UcdParamOpt::print_similar_command_info(std::string command)
{
    std::cout << "can not find command : " << command << ", find similar command :" << std::endl; 
    std::cout << std::endl;
    std::vector<std::string> similar_command_vector = UcdParamOpt::find_similar_command(command);
    if(similar_command_vector.size() > 0)
    {
        std::cout << "------------------------" << std::endl;

        for(int i=0; i<similar_command_vector.size(); i++)
        {
            ParamInfo param_info = UcdParamOpt::get_param(similar_command_vector[i]);
            param_info.print_info();
            std::cout << " " << std::endl;
        }
        std::cout << "------------------------" << std::endl;
    }
}

void UcdParamOpt::print_param_vector(std::vector<ParamInfo> param_info_vector)
{
    //
    std::cout << "------------------------" << std::endl;
    for(int i=0; i<param_info_vector.size(); i++)
    {
        // 
        param_info_vector[i].print_info();
    }
    std::cout << "------------------------" << std::endl;
}

void UcdParamOpt::print_command_info(std::string command)
{
    std::cout << "------------------------" << std::endl;
    ParamInfo param_info = UcdParamOpt::get_param(command);
    param_info.print_info();
    std::cout << "------------------------" << std::endl;
}

void UcdParamOpt::print_group_info(std::string group)
{
    std::cout << "--------------------------------------------------------------------------" << std::endl;
    std::cout << HIGHTLIGHT_COLOR << "                                " << group << STOP_COLOR << std::endl;
    std::cout << "--------------------------------------------------------------------------" << std::endl;
    std::cout << std::endl;
    auto iter = UcdParamOpt::param_map.begin();
    while(iter != UcdParamOpt::param_map.end())
    {
        if(iter->second.group == group)
        {
            std::cout << WARNNING_COLOR << std::setw(15) << std::left << iter->second.command << " " << STOP_COLOR;
            std::cout << std::setw(4) << std::left << " " << iter->second.grammar << std::endl;
            std::cout << std::endl;
        }
        iter++;
    }
    std::cout << "--------------------------------------------------------------------------" << std::endl;
}

void UcdParamOpt::print_command_info(std::vector<std::string> command_vector)
{
    //
    std::cout << "------------------------" << std::endl;
    for(int i=0; i<command_vector.size(); i++)
    {
        ParamInfo param_info = UcdParamOpt::get_param(command_vector[i]);
        param_info.print_info();
    }
    std::cout << "------------------------" << std::endl;
}

void UcdParamOpt::print_all_fun()
{
    // 计算模块属于的组，按照分组打印所有的帮助信息
    std::map<std::string, std::set<std::string> > gropu_map; 
    auto iter = UcdParamOpt::param_map.begin();
    while(iter != UcdParamOpt::param_map.end())
    {
        gropu_map[iter->second.group].insert(iter->first);
        iter ++ ;
    }
    // 按照分组打印所有命令
    std::cout << "------------------------" << std::endl;
    auto iter_group = gropu_map.begin();
    while(iter_group != gropu_map.end())
    {
        std::cout << WARNNING_COLOR << iter_group->first << STOP_COLOR << std::endl;
        auto iter_command = iter_group->second.begin();
        while(iter_command != iter_group->second.end())
        {
            ParamInfo param_info = UcdParamOpt::get_param(iter_command->data());
            std::cout << std::setw(25) << std::right << iter_command->data() << "," ;
            std::cout << std::setw(4) << std::left << " " << param_info.grammar << std::endl;
            iter_command++;
        }
        iter_group++;
    }
    std::cout << "------------------------" << std::endl;
}

void UcdParamOpt::print_castration_fun(std::string castration_fun)
{
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "                        阉割版语法文档" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::vector<std::string> c_func_list = pystring::split(castration_fun, ",");
    std::cout << "" << std::endl;
    
    for(int i=0; i<c_func_list.size(); i++)
    {
        std::string func = pystring::strip(c_func_list[i], " ");
        if(UcdParamOpt::has_command(func))
        {
            ParamInfo param_info = UcdParamOpt::get_param(func);
            std::cout << HIGHTLIGHT_COLOR << std::setw(15) << std::left << func << STOP_COLOR << std::left << " " << param_info.grammar << std::endl;
            std::cout << "" << std::endl;
        }
    }
    std::cout << "----------------------------------------------------------------" << std::endl;
}

void UcdParamOpt::print_castration_fun_chinese(std::string castration_fun)
{
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "                        阉割版语法文档" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::vector<std::string> c_func_list = pystring::split(castration_fun, ",");
    std::cout << "" << std::endl;
    
    for(int i=0; i<c_func_list.size(); i++)
    {
        std::string func = pystring::strip(c_func_list[i], " ");
        if(UcdParamOpt::has_command(func))
        {
            ParamInfo param_info = UcdParamOpt::get_param(func);
            std::cout << HIGHTLIGHT_COLOR << std::setw(15) << std::left << func << STOP_COLOR << std::left << " " << param_info.chinese_explain << std::endl;
            std::cout << "" << std::endl;
        }
    }
    std::cout << "----------------------------------------------------------------" << std::endl;
}

void UcdParamOpt::print_all_fun_chinese()
{
    // 计算模块属于的组，按照分组打印所有的帮助信息
    std::map<std::string, std::set<std::string> > gropu_map; 
    auto iter = UcdParamOpt::param_map.begin();
    while(iter != UcdParamOpt::param_map.end())
    {
        gropu_map[iter->second.group].insert(iter->first);
        iter ++ ;
    }
    // 按照分组打印所有命令
    std::cout << "------------------------" << std::endl;
    auto iter_group = gropu_map.begin();
    while(iter_group != gropu_map.end())
    {
        std::cout << WARNNING_COLOR << iter_group->first << STOP_COLOR << std::endl;
        auto iter_command = iter_group->second.begin();
        while(iter_command != iter_group->second.end())
        {
            ParamInfo param_info = UcdParamOpt::get_param(iter_command->data());
            std::cout << std::setw(20) << std::right << iter_command->data() << "," ;
            std::cout << std::setw(4) << std::left << " " << param_info.chinese_explain << std::endl;
            iter_command++;
        }
        iter_group++;
    }
    std::cout << "------------------------" << std::endl;
}

void UcdParamOpt::load_param_info()
{

    // check
    ParamInfo * param_check = new ParamInfo("check");
    param_check->group = "sync";
    param_check->grammar = "ucd check";
    param_check->args_info["--assign_uc"] = "查找包含某一个 uc 的所有 json 文件（只查找 official 数据库中最外层的 ucd:json）";
    param_check->english_explain = "get all ucd official|customer from server";
    param_check->chinese_explain = "查看服务器中所有 ucd 信息，包括官方 ucd 和 自定义 ucd";
    param_check->demo.push_back("ucd check                          (查看所有云端的 ucd)");   
    param_check->demo.push_back("ucd check --assign_uc Dka09ai      (查看 official 最外层 ucd:json 中包含 Dka09ai 进行返回)");   
    param_check->demo.push_back("ucd check | grep fzc               (查看所有云端中带有 fzc 关键字的 ucd)"); 
    UcdParamOpt::add_param(param_check);
    
    // save
    ParamInfo * param_save = new ParamInfo("save");
    param_save->group = "sync";
    param_save->grammar = "ucd save ucd_path save_dir save_mode(image,xml) {need_count}";
    param_save->args_info["-s"] = "增加 -s 代表 xml 不是从当前文件中解析出来的，而是从服务器下载的";
    param_save->english_explain = "load img from server and parse xml from json";
    param_save->chinese_explain = "从服务器下载 图片 保存到指定文件夹，从 json 解析 xml 保存到指定文件夹"; 
    param_save->demo.push_back("ucd save test.json ./img 10             (将 test.json 中带的所有图片保存到 ./img 路径下)");  
    param_save->demo.push_back("ucd save test.json ./img 01             (从 test.json 中解析出xml信息保存到 ./img 路径下)");  
    param_save->demo.push_back("ucd save test.json ./img 11             (将 test.json 中带的所有图片和xml保存到 ./img 路径下)");  
    param_save->demo.push_back("ucd save test.json ./img 01 -s          (从 服务器下载 test.json 中包含的 uc 对应的 xml)");  
    UcdParamOpt::add_param(param_save);
    
    // save_remote_ucd
    ParamInfo * param_save_remote_ucd = new ParamInfo("save_remote_ucd");
    param_save_remote_ucd->group = "sync";
    param_save_remote_ucd->grammar = "ucd save_remote_ucd save_dir";
    param_save_remote_ucd->chinese_explain = "从服务器将所有入库的 ucd 文件都下载到本地，用于同步数据防止丢失使用的";  
    param_save_remote_ucd->demo.push_back("ucd save_remote_ucd ./ucd_dir          (从 服务器下载所有 customer 和 official 文件，保存到 ucd_dir 文件夹)");  
    UcdParamOpt::add_param(param_save_remote_ucd);
    
    // save_cache
    ParamInfo * param_save_cache = new ParamInfo("save_cache");
    param_save_cache->group = "sync";
    param_save_cache->grammar = "ucd save_cache ucd_path save_mode(image,xml)";
    param_save_cache->english_explain = "load img|xml from server";
    param_save_cache->chinese_explain = "从服务器下载 图片|标注 保存到 ucd 缓存文件夹";   
    param_save_cache->demo.push_back("ucd save_cache test.json 10       (将 test.json 中带的所有图片保存到缓存文件夹)");  
    param_save_cache->demo.push_back("ucd save_cache test.json 01       (将 test.json 中带的所有xml保存到缓存文件夹)");  
    param_save_cache->demo.push_back("ucd save_cache test.json 11       (将 test.json 中带的所有图片和xml保存到缓存文件夹)");  
    UcdParamOpt::add_param(param_save_cache);
    
    // parse_xml
    ParamInfo * param_parse_xml = new ParamInfo("parse_xml");
    param_parse_xml->group = "convert";
    param_parse_xml->grammar = "ucd parse_xml json_path save_dir";
    param_parse_xml->english_explain = "parse xml from json";
    param_parse_xml->chinese_explain = "从 ucd 中解析出 xml"; 
    param_parse_xml->demo.push_back("ucd parse_xml test.json ./xml      (将 test.json中包含的xml数据解析到 ./xml 文件夹下)");  
    UcdParamOpt::add_param(param_parse_xml);
    
    // parse_volume_xml
    ParamInfo * param_parse_volume_xml = new ParamInfo("parse_volume_xml");
    param_parse_volume_xml->group = "convert";
    param_parse_volume_xml->grammar = "ucd parse_volume_xml json_path save_dir";
    param_parse_volume_xml->english_explain = "parse xml from json";
    param_parse_volume_xml->chinese_explain = "从 uci 中解析出 xml"; 
    param_parse_volume_xml->demo.push_back("ucd parse_volume_xml test.uci ./xml      (将 test.uci 中包含的xml数据解析到 ./xml 文件夹下)");  
    UcdParamOpt::add_param(param_parse_volume_xml);
    
    // parse_json
    ParamInfo * param_parse_json = new ParamInfo("parse_json");
    param_parse_json->group = "convert";
    param_parse_json->grammar = "ucd parse_xml json_path save_dir";
    param_parse_json->english_explain = "parse labelme_json from ucd";
    param_parse_json->chinese_explain = "从 ucd 中解析出 labelme_json";   
    UcdParamOpt::add_param(param_parse_json);
    
    // show
    ParamInfo * param_show = new ParamInfo("show");
    param_show->group = "info";
    param_show->grammar = "ucd show {uc}";
    param_show->english_explain = "show for all url supported";
    param_show->chinese_explain = "查看所有下载路径";  
    param_show->demo.push_back("ucd show Dae069p                        (查看 Dae069p uc 对应的网络路径, 图片 url 地址, json url 地址等)"); 
    UcdParamOpt::add_param(param_show);
    
    // delete
    ParamInfo * param_delete = new ParamInfo("delete");
    param_delete->group = "sync";
    param_delete->grammar = "ucd show {uc}";
    param_delete->english_explain = "ucd delete ucd_name";
    param_delete->chinese_explain = "删除在线数据集,无法删除官方数据集";  
    param_delete->demo.push_back("ucd delete del\\aqm_ps                (删除云上 del\\aqm_ps 地址的 ucd 数据集)");
    UcdParamOpt::add_param(param_delete);
    
    // load
    ParamInfo * param_load = new ParamInfo("load");
    param_load->group = "sync";
    param_load->grammar = "ucd load ucd_name save_path|save_dir";
    param_load->english_explain = "load ucd from server";
    param_load->chinese_explain = "下载在线数据集";   
    param_load->demo.push_back("ucd load docker\\\\del\\\\recommend_base                        (将云端上地址为 docker\\\\del\\\\recommend_base 的ucd 保存到本地路径, 文件名中带有云上位置的文件夹等级信息)");
    param_load->demo.push_back("ucd load docker\\\\del\\\\recommend_base recommend_base.json    (将云端上地址为 docker\\\\del\\\\recommend_base 的ucd 保存到本地的 recommend_base.json 路径)");
    UcdParamOpt::add_param(param_load);
    
    // upload
    ParamInfo * param_upload = new ParamInfo("upload");
    param_upload->group = "sync";
    param_upload->grammar = "ucd upload ucd_path {ucd_name}";
    param_upload->english_explain = "upload ucd to server";
    param_upload->chinese_explain = "上传数据集到网络";   
    param_upload->demo.push_back("ucd upload test.json del\\test        (将本地 test.json 数据集上传的云上 del\\test 位置)");
    UcdParamOpt::add_param(param_upload);
    
    // from_img
    ParamInfo * param_from_img = new ParamInfo("from_img");
    param_from_img->group = "convert";
    param_from_img->grammar = "ucd from_img img_dir ucd_save_path";
    param_from_img->english_explain = "get ucd from loacal data(img)";
    param_from_img->chinese_explain = "(递归)本地文件生成数据集, 读取图片解析获得图片的长宽信息";   
    param_from_img->demo.push_back("ucd from_img ./img test.json        (将 ./img 路径下的名字符合 uc 规则的图片生成 ucd)");
    UcdParamOpt::add_param(param_from_img);
    
    // from_xml
    ParamInfo * param_from_xml = new ParamInfo("from_xml");
    param_from_xml->group = "convert";
    param_from_xml->grammar = "ucd from_xml xml_dir ucd_save_path";
    param_from_xml->english_explain = "get ucd from loacal data(img, xml)";
    param_from_xml->chinese_explain = "(递归)本地 voc_xml 文件生成 ucd, ucd中包含 dete_obj 信息";   
    param_from_xml->demo.push_back("ucd from_xml ./xml test.json        (将 ./xml 路径下面的名字符合 uc 规则的所有 xml 制作成 ucd)");
    UcdParamOpt::add_param(param_from_xml);
    
    // from_huge_xml
    ParamInfo * param_from_huge_xml = new ParamInfo("from_huge_xml");
    param_from_huge_xml->group = "convert";
    param_from_huge_xml->grammar = "ucd from_huge_xml xml_dir save_path {volume_size}";
    param_from_huge_xml->english_explain = "get ucd from loacal data(img, xml)";
    param_from_huge_xml->chinese_explain = "(递归)本地 voc_xml 文件生成 ucd, ucd 中包含 dete_obj 信息";   
    param_from_huge_xml->demo.push_back("ucd from_huge_xml ./xml ./res.uci          (将 ./xml 路径下面的名字符合 uc 规则的所有 xml 制作成 uci 文件)");
    param_from_huge_xml->demo.push_back("ucd from_huge_xml ./xml ./res.uci  30      (将 ./xml 路径下面的名字符合 uc 规则的所有 xml 制作成 uci 文件, 卷大小为 30, 一卷数据大约 280M)");
    UcdParamOpt::add_param(param_from_huge_xml);
    
    // from_json
    ParamInfo * param_from_json = new ParamInfo("from_json");
    param_from_json->group = "convert";
    param_from_json->grammar = "ucd from_json json_dir ucd_save_path";
    param_from_json->english_explain = "get ucd from loacal data(img, json)";
    param_from_json->chinese_explain = "(递归)本地 labelme_json 文件生成 ucd, ucd中包含 dete_obj 信息";   
    param_from_json->demo.push_back("ucd from_json ./json test.json        (将 ./json 路径下面的名字符合 uc 规则的所有 json 制作成 ucd)");
    UcdParamOpt::add_param(param_from_json);
    
    // from_uc
    ParamInfo * param_from_uc = new ParamInfo("from_uc");
    param_from_uc->group = "convert";
    param_from_uc->grammar = "ucd from_uc ucd_path uc1 uc2 uc3 uc4";
    param_from_uc->english_explain = "get ucd from uc_list";
    param_from_uc->chinese_explain = "根据指定的 uc 生成 ucd, 会对 uc 的合法性进行检查 剔除不合法的 uc";   
    param_from_uc->demo.push_back("ucd from_uc ./test.json Fuc0001 Fuc0002 Fuc0003      (生成一个只有 Fuc0001 Fuc0002 Fuc0003 三个 uc 的 ucd)");
    UcdParamOpt::add_param(param_from_uc);

    // from_yolo_txt
    ParamInfo * param_from_yolo_txt = new ParamInfo("from_yolo_txt");
    param_from_yolo_txt->group = "convert";
    param_from_yolo_txt->grammar = "ucd from_yolo_txt txt_dir ucd_path";
    param_from_yolo_txt->args_info["--size_ucd"] = "yolo_txt 想恢复成 obj 必须获取原始图像长宽信息，当缓存中没有对应的图片时，可以指定存储长宽信息的 ucd:json 文件";
    param_from_yolo_txt->chinese_explain = "从符合 yolo 训练条件的 txt 数据中恢复 obj 信息";   
    param_from_yolo_txt->demo.push_back("ucd from_yolo_txt ./txt_dir res.json --size_ucd ./size_info.json      (从 txt_dir 的txt文件生成 ucd:json，指定从 size_info.json 中获取需要图片的长宽信息)");
    UcdParamOpt::add_param(param_from_yolo_txt);

    // from_dete_server
    ParamInfo * param_from_dete_server = new ParamInfo("from_dete_server");
    param_from_dete_server->group = "convert";
    param_from_dete_server->grammar = "ucd param_from_dete_server dete_server_dir ucd_path ucd_save_path";
    param_from_dete_server->english_explain = "";
    param_from_dete_server->chinese_explain = "在 dete_server 生成的文件夹下面找到 ucd 中包含的 uc 的检测结果，生成 ucd";   
    param_from_dete_server->demo.push_back("ucd param_from_dete_server dete_xml test.json save.json       (从 dete_xml 文件夹下挑选出 test.json 中包含的图片(uc)的xml,生成 ucd");
    param_from_dete_server->demo.push_back("----------------------------------------------");
    param_from_dete_server->demo.push_back("server dir format:");
    param_from_dete_server->demo.push_back("Dad/");
    param_from_dete_server->demo.push_back("    Dad00u1.xml");
    param_from_dete_server->demo.push_back("    Dad00u2.xml");
    param_from_dete_server->demo.push_back("    Dad00u3.xml");
    param_from_dete_server->demo.push_back("Dae/");
    param_from_dete_server->demo.push_back("    Dae00u1.xml");
    param_from_dete_server->demo.push_back("    Dae00u2.xml");
    param_from_dete_server->demo.push_back("----------------------------------------------");
    UcdParamOpt::add_param(param_from_dete_server);

    // info
    ParamInfo * param_info = new ParamInfo("info");
    param_info->group = "info";
    param_info->args_info["-a"] = "会增加打印 count_tags 输出的信息";
    param_info->grammar = "ucd info ucd_path | uci_path";
    param_info->english_explain = "show ucd info";
    param_info->chinese_explain = "查看数据集信息 (dataset_name, uc_count, model_name, model_version, add_time, update_time, describe, label_used, img_size_count)";   
    param_info->demo.push_back("ucd info test.json                          (查看 test.json ucd 的各个信息)");
    param_info->demo.push_back("ucd info test.uci                           (查看 test.uci 的各个信息)");
    UcdParamOpt::add_param(param_info);
    
    // meta
    ParamInfo * param_meta = new ParamInfo("meta");
    param_meta->group = "meta_info";
    param_meta->grammar = "ucd meta {attr_name}";
    param_meta->english_explain = "show config info";
    param_meta->chinese_explain = "查看配置信息, 可以指定配置的名字(host, port, config_path, sql_host, sql_port, sql_user, sql_pwd, sql_db, cache_dir)";   
    param_meta->demo.push_back("ucd meta                                    (查看 ucd 软件的所有 元信息)");
    param_meta->demo.push_back("ucd meta cache_dir                          (查看 ucd 软件的缓存位置信息)");
    UcdParamOpt::add_param(param_meta);
    
    // set
    ParamInfo * param_set = new ParamInfo("set");
    param_set->group = "meta_info";
    param_set->grammar = "ucd set key value";
    param_set->english_explain = "update config info";
    param_set->chinese_explain = "设置配置信息 (host, port, sql_host, sql_port, sql_user, sql_pwd, sql_db, cache_dir, redis_port, redis_host, castration_function) ";  
    param_set->demo.push_back("ucd set cache_dir /home/disk2/ucd_cache      (设置 ucd 的缓存文件夹)"); 
    param_set->demo.push_back("ucd set host 192.168.3.111                   (设置 ucd 服务的 host 信息)"); 
    UcdParamOpt::add_param(param_set);
    
    // param_merge
    ParamInfo * param_merge = new ParamInfo("merge");
    param_merge->group = "opt";
    param_merge->grammar = "ucd merge save_path ucd_path1 ucd_path2 ...";
    param_merge->english_explain = "merge ucd info";
    param_merge->chinese_explain = "合并 ucd, 合并 uc_list label_used object_info 其他信息清空"; 
    param_merge->demo.push_back("ucd merge merge.json test1.json test2.json test3.json      (合并 test1.josn test2.json test3.json 得到 merge.json)");  
    UcdParamOpt::add_param(param_merge);
    
    // minus_obj
    ParamInfo * param_minus_obj = new ParamInfo("minus_obj");
    param_minus_obj->group = "opt";
    param_minus_obj->grammar = "ucd minus_obj ucd_path1 ucd_path2 save_path";
    param_minus_obj->english_explain = "do minus operation between two ucd";
    param_minus_obj->chinese_explain = "减操作数据集, 删除存在 ucd_path2 中的 obj 信息 uc 不变";   
    param_minus_obj->demo.push_back("ucd minus_obj test1.json test2.json res.json       (将 test1.josn - test2.json 得到 res.json)");
    UcdParamOpt::add_param(param_minus_obj);

    // minus_uc
    ParamInfo * param_minus_uc = new ParamInfo("minus_uc");
    param_minus_uc->group = "opt";
    param_minus_uc->grammar = "ucd minus_uc ucd_path1 ucd_path2 save_path";
    param_minus_uc->english_explain = "do minus operation between two ucd";
    param_minus_uc->chinese_explain = "减操作数据集, 删除存在 ucd_path2 中的 uc 信息，当 uc 存在与 uc 对应的 size_info 和 obj_info 也都不存在了";   
    param_minus_uc->demo.push_back("ucd minus_uc test1.json test2.json res.json       (将 test1.josn - test2.json 得到 res.json)");
    UcdParamOpt::add_param(param_minus_uc);
    
    // diff
    ParamInfo * param_diff = new ParamInfo("diff");
    param_diff->group = "info";
    param_diff->grammar = "ucd diff ucd_path1 ucd_path2";
    param_diff->english_explain = "compare two ucd";
    param_diff->chinese_explain = "比较数据集, (A : 只 ucd1 中的 uc, AB: ucd1 ucd2 中都有的 uc, B: 只在 ucd2 中的 uc)";   
    param_diff->demo.push_back("ucd diff test1.json test2.json                  (比较 test1.json test2.json 中的 uc 有多少是重合的)");
    UcdParamOpt::add_param(param_diff);
    
    // rename_img
    ParamInfo * param_rename_img = new ParamInfo("rename_img");
    param_rename_img->group = "rename";
    param_rename_img->grammar = "ucd rename_img img_dir";
    param_rename_img->args_info["-c"] = "-c check_uc 强制根据文件 md5 找到对应的 uc, 否则当文件名为 uc 样式（不包括fake uc 即 Fuc 开始的 uc）则直接忽略重命名";
    param_rename_img->english_explain = "rename img by uc";
    param_rename_img->chinese_explain = "使用 uc 重命名图片(.jpg, .JPG, .png, .PNG), 未入库的数据不进行重命名";
    param_rename_img->demo.push_back("ucd rename_img ./img                      (将 ./img 路径下的所有图片进行重名名)");   
    UcdParamOpt::add_param(param_rename_img);
    
    // rename_img_xml
    ParamInfo * param_rename_img_xml = new ParamInfo("rename_img_xml");
    param_rename_img_xml->group = "rename";
    param_rename_img_xml->grammar = "ucd rename_img_xml img_dir xml_dir";
    param_rename_img_xml->args_info["-c"] = "-c check_uc 强制根据文件 md5 找到对应的 uc, 否则当文件名为 uc 样式（不包括fake uc 即 Fuc 开始的 uc）则直接忽略重命名";
    param_rename_img_xml->english_explain = "rename img xml by uc";
    param_rename_img_xml->chinese_explain = "使用 uc 重命名数据集, xml 名字跟随者 img 的名字的改变而改变";
    param_rename_img_xml->demo.push_back("ucd rename_img_xml ./img ./xml        (将 ./img 文件夹中的图片和 ./xml 文件夹中的 xml 同时使用 uc 重命名)");   
    UcdParamOpt::add_param(param_rename_img_xml);
    
    // rename_img_json
    ParamInfo * param_rename_img_json = new ParamInfo("rename_img_json");
    param_rename_img_json->group = "rename";
    param_rename_img_json->grammar = "ucd rename_img_json img_dir xml_dir";
    param_rename_img_json->args_info["-c"] = "-c check_uc 强制根据文件 md5 找到对应的 uc, 否则当文件名为 uc 样式（不包括fake uc 即 Fuc 开始的 uc）则直接忽略重命名";
    param_rename_img_json->english_explain = "rename img json by uc";
    param_rename_img_json->chinese_explain = "使用 uc 重命名数据集, json 名字跟随者 img 的名字的改变而改变";   
    param_rename_img_json->demo.push_back("ucd rename_img_xml ./img ./json        (将 ./img 文件夹中的图片和 ./json 文件夹中的 json 同时使用 uc 重命名)");   
    UcdParamOpt::add_param(param_rename_img_json);
    
    // count_tags
    ParamInfo * param_count_tags = new ParamInfo("count_tags");
    param_count_tags->group = "info";
    param_count_tags->grammar = "ucd count_tags ucd_path | uci_path";
    param_count_tags->english_explain = "count tags";
    param_count_tags->chinese_explain = "统计标签个数";
    param_count_tags->demo.push_back("ucd count_tags test.json                      (统计 test.json 中的各个标签的个数)");   
    param_count_tags->demo.push_back("ucd count_tags test.uci                       (统计 test.uci 中的各个标签的个数)");   
    UcdParamOpt::add_param(param_count_tags);
    
    // count_uc_by_tags
    ParamInfo * param_count_uc_by_tags = new ParamInfo("count_uc_by_tags");
    param_count_uc_by_tags->group = "info";
    param_count_uc_by_tags->grammar = "ucd count_uc_by_tags ucd_path | uci_path";
    param_count_uc_by_tags->english_explain = "count tags";
    param_count_uc_by_tags->chinese_explain = "统计每一个标签存在于多少个图片中";
    param_count_uc_by_tags->demo.push_back("ucd count_uc_by_tags test.json             (统计 test.json 中的各个标签对应的图片数)");   
    UcdParamOpt::add_param(param_count_uc_by_tags);
    
    // count_files
    ParamInfo * param_count_files = new ParamInfo("count_files");
    param_count_files->group = "info";
    param_count_files->grammar = "ucd count_files file_dir recursive(true|1|True|false|0|False)";
    param_count_files->english_explain = "statistics file by suffix";
    param_count_files->chinese_explain = "统计文件夹中各后缀的文件数, 默认穿透文件夹进行统计";   
    param_count_files->demo.push_back("ucd count_files ./test 1                     (统计 ./test 文件夹下面各个后缀数据的个数, 穿透文件夹)");
    param_count_files->demo.push_back("ucd count_files ./test 0                     (统计 ./test 文件夹下面各个后缀数据的个数, 不穿透文件夹)");
    UcdParamOpt::add_param(param_count_files);
    
    // cut_small_img
    ParamInfo * param_cut_small_img = new ParamInfo("cut_small_img");
    param_cut_small_img->group = "opt";
    param_cut_small_img->grammar = "ucd cut_small_img ucd_path save_dir is_split(true|1|True|false|0|False)";
    param_cut_small_img->args_info["--no_cache"] = "1|True|true 低缓存模式，使用完下载的图片之后会删除，本地已有缓存的不进行删除";
    param_cut_small_img->args_info["-s"] = "小图放到以其名字命名的文件夹中，设置的话，全部放到一个文件夹中";
    param_cut_small_img->english_explain = "cut img by dete obj";
    param_cut_small_img->chinese_explain = "裁剪出小图";   
    param_cut_small_img->demo.push_back("ucd cut_small_img test.json ./crop         (将 test.json 中对应的各个小图都截取出来，放到 ./crop 文件夹中，每个标签的的小图分文件夹存放)");
    param_cut_small_img->demo.push_back("ucd cut_small_img test.json ./crop -s      (将 test.json 中对应的各个小图都截取出来，放到 ./crop 文件夹中，所有小图放在一起)");
    UcdParamOpt::add_param(param_cut_small_img);
    
    // to_crop
    ParamInfo * param_to_crop = new ParamInfo("to_crop");
    param_to_crop->group = "opt";
    param_to_crop->grammar = "ucd to_crop ucd_path save_dir is_split(true|1|True|false|0|False)";
    param_to_crop->args_info["--no_cache"] = "1|True|true 低缓存模式，使用完下载的图片之后会删除，本地已有缓存的不进行删除";
    param_to_crop->args_info["-s"] = "小图放到以其名字命名的文件夹中，设置的话，全部放到一个文件夹中";
    param_to_crop->args_info["-c"] = "小图会根据其置信度改变标签名类似为 tag_(0.1-0.2] 用于分析不同置信度下得到的结果";
    param_to_crop->english_explain = "cut img by dete obj";
    param_to_crop->chinese_explain = "裁剪出小图";   
    param_to_crop->demo.push_back("ucd to_crop test.json ./crop         (将 test.json 中对应的各个小图都截取出来，放到 ./crop 文件夹中，每个标签的的小图分文件夹存放)");
    param_to_crop->demo.push_back("ucd to_crop test.json ./crop -s      (将 test.json 中对应的各个小图都截取出来，放到 ./crop 文件夹中，所有小图放在一起)");
    param_to_crop->demo.push_back("ucd to_crop test.json ./crop -s -c   (将 test.json 中对应的各个小图都截取出来，放到 ./crop 文件夹中，所有小图放在一起, 并改变标签包含置信度信息)");
    param_to_crop->demo.push_back("ucd to_crop test.json ./crop -c      (将 test.json 中对应的各个小图都截取出来，放到 ./crop 文件夹中，按照 tag_conf_range 划分为多个文件夹");
    UcdParamOpt::add_param(param_to_crop);
    
    // to_assign_crop_xml
    ParamInfo * param_to_assign_crop_xml = new ParamInfo("to_assign_crop_xml");
    param_to_assign_crop_xml->group = "opt";
    param_to_assign_crop_xml->grammar = "ucd to_assign_crop_xml ucd_path save_dir assign_tag";
    param_to_assign_crop_xml->args_info["--iou_th"] = "指定根据范围进行删选的时候使用的重复阈值";
    param_to_assign_crop_xml->chinese_explain = "指定标签，根据标签的范围裁剪小图和对应的xml，xml 中包含在指定标签内部的标签";   
    param_to_assign_crop_xml->demo.push_back("ucd to_assign_crop_xml test.json ./crop range --iou_th 0.85   (将 test.json 中 range 标签的范围裁剪出来，同时保存一份 xml 里面记录着包含在 range 标签内部的 obj 信息)");
    UcdParamOpt::add_param(param_to_assign_crop_xml);
    
    // to_assign_crop_txt
    ParamInfo * param_to_assign_crop_txt = new ParamInfo("to_assign_crop_txt");
    param_to_assign_crop_txt->group = "opt";
    param_to_assign_crop_txt->grammar = "ucd to_assign_crop_txt ucd_path save_dir assign_tag";
    param_to_assign_crop_txt->args_info["--iou_th"] = "指定根据范围进行删选的时候使用的重复阈值";
    param_to_assign_crop_txt->chinese_explain = "指定标签，根据标签的范围裁剪小图和对应的xml，xml 中包含在指定标签内部的标签";   
    param_to_assign_crop_txt->demo.push_back("ucd to_assign_crop_txt test.json ./crop range --iou_th 0.85   (将 test.json 中 range 标签的范围裁剪出来，同时保存一份 xml 里面记录着包含在 range 标签内部的 obj 信息)");
    UcdParamOpt::add_param(param_to_assign_crop_txt);

    // crop_to_xml
    ParamInfo * param_crop_to_xml = new ParamInfo("crop_to_xml");
    param_crop_to_xml->group = "opt";
    param_crop_to_xml->grammar = "ucd crop_to_xml crop_dir, save_dir";
    param_crop_to_xml->english_explain = "cut img to xml";
    param_crop_to_xml->chinese_explain = "截图生成xml";   
    param_crop_to_xml->demo.push_back("ucd crop_to_xml ./crop ./xml                 (将 ./crop 文件夹中的小图文件夹映射回 xml 保存在 ./xml 文件夹)");
    param_crop_to_xml->demo.push_back("* atention, crop 文件夹下面要有小图文件夹，每个文件夹的名字是里面小图的真实标签");
    UcdParamOpt::add_param(param_crop_to_xml);
    
    // search_similar
    ParamInfo * param_search_similar = new ParamInfo("search_similar");
    param_search_similar->group = "server";
    param_search_similar->grammar = "ucd search_similar img_path {save_path}";
    param_search_similar->args_info["--limit"] = "指定返回的相似的 uc 的个数";
    param_search_similar->chinese_explain = "在矢量数据库中搜索已入库的 uc";   
    param_search_similar->demo.push_back("ucd search_similar img_path                 (在矢量数据库中搜索已入库的相似的 uc)");
    param_search_similar->demo.push_back("ucd search_similar img_path  save_path   --limit 10   (在矢量数据库中搜索已入库的相似的 uc，将对应 10 个 uc 存放在ucd json save_path 中)");
    UcdParamOpt::add_param(param_search_similar);
    
    // query
    // std::string query, std::string system_content, int max_tokens, float threshold, bool search_database
    ParamInfo * param_query = new ParamInfo("query");
    param_query->group = "server";
    param_query->grammar = "ucd query 'your question'";
    param_query->args_info["--system_content"] = "系统的提示信息比如（你是一个辅助机器人，使用简洁的中文回答问题）";
    param_query->args_info["--max_tokens"] = "回复的最大的 token 数，如 1000";
    param_query->args_info["--threshold"] = "与数据库中的问题进行对比时使用的阈值，阈值越小，匹配到的问题越准确,如 0.4";
    param_query->args_info["-f"] = "是否在已有的数据库中进行查找，-f 代表不找数据库直接问 chartgpt";
    param_query->args_info["-l"] = "默认不实用较短的回答，对应的  content 为 你是一个优秀的助手,特别善于解决图像检测和声音信息处理方面的问题";
    param_query->chinese_explain = "在数据库中查找问题并返回答案，如果数据库中找不到匹配的结果去问 chartgpt";   
    param_query->demo.push_back("ucd query 'docker 查看所有的镜像'                 (在数据库中查找[docker 查看所有的镜像]这个问题的答案，找不到的话问chartgpt)");
    param_query->demo.push_back("ucd query 'docker 查看所有的镜像' --max_tokens 200 -f --system_content 'answer in english' （直接询问chartgpt问题，希望回答的结果使用英文并不要超过 200 个token）");
    UcdParamOpt::add_param(param_query);
    
    // crop_to_xml_with_origin_tag
    ParamInfo * param_crop_to_xml_with_origin_tag = new ParamInfo("crop_to_xml_with_origin_tag");
    param_crop_to_xml_with_origin_tag->group = "opt";
    param_crop_to_xml_with_origin_tag->grammar = "ucd crop_to_xml_with_origin_tag crop_dir, save_dir";
    param_crop_to_xml_with_origin_tag->english_explain = "cut img to xml";
    param_crop_to_xml_with_origin_tag->chinese_explain = "截图生成 xml，直接使用截图文件名中的标签，作为截图的标签，截图文件夹可以多层嵌套";   
    param_crop_to_xml_with_origin_tag->demo.push_back("ucd crop_to_xml_with_origin_tag ./crop ./xml                 (将 ./crop 文件夹中的小图文件夹映射回 xml 保存在 ./xml 文件夹)");
    UcdParamOpt::add_param(param_crop_to_xml_with_origin_tag);

    // say
    ParamInfo * param_say = new ParamInfo("say");
    param_say->group = "fun";
    param_say->grammar = "ucd say words {height} {width}";
    param_say->english_explain = "say something with big character";
    param_say->chinese_explain = "将汉字放大说点什么";
    param_say->demo.push_back("ucd say 你好 50 40                                   (将 你好 两个函数 高为 50 宽为 40 打印在屏幕上)");   
    param_say->demo.push_back("ucd say 你好 40                                      (将 你好 两个函数 高为 40 宽为 40 打印在屏幕上)");   
    param_say->demo.push_back("ucd say 你好                                         (将 你好 两个函数 高为 30 宽为 30 打印在屏幕上)");   
    UcdParamOpt::add_param(param_say);

    // cache_info
    ParamInfo * param_cache_info = new ParamInfo("cache_info");
    param_cache_info->group = "cache";
    param_cache_info->grammar = "ucd cache_info {ucd_path}";
    param_cache_info->english_explain = "viewing cache info";
    param_cache_info->chinese_explain = "查看当前服务器的缓存信息";   
    param_cache_info->demo.push_back("ucd cache_info                                (查看缓存文件夹下面有多少文件)");
    param_cache_info->demo.push_back("ucd cache_info test.json                      (查看缓存文件夹下面包含 test.json 中多少比例的文件)");
    param_cache_info->demo.push_back("----------------------------------------");
    param_cache_info->demo.push_back("server  :     cache_dir");
    param_cache_info->demo.push_back("----------------------------------------");
    param_cache_info->demo.push_back("221     :     /home/disk3/ucd_cache");
    param_cache_info->demo.push_back("101     :     /home/suanfa-6/ucd_cache");
    param_cache_info->demo.push_back("202     :     /home/suanfa-2/ucd_cache");
    param_cache_info->demo.push_back("21      :     /home/suanfa-2/ucd_cache");
    param_cache_info->demo.push_back("246     :     /home/ucd_cache/");
    param_cache_info->demo.push_back("34      :     /home/data/ucd_cache");
    param_cache_info->demo.push_back("107     :     /home/ucd_cache/");
    param_cache_info->demo.push_back("33      :     /home/data/ucd_cache");
    param_cache_info->demo.push_back("50      :     /home/suanfa-2/ucd_cache");
    param_cache_info->demo.push_back("----------------------------------------");
    UcdParamOpt::add_param(param_cache_info);
    
    // cache_clear
    ParamInfo * param_cache_clear = new ParamInfo("cache_clear");
    param_cache_clear->group = "cache";
    param_cache_clear->grammar = "ucd cache_clear {ucd_path}";
    param_cache_clear->args_info["-r"] = "reversal, 只保留指定的 json 中的文件";
    param_cache_clear->english_explain = "clear cache";
    param_cache_clear->chinese_explain = "清空缓存信息, 后续需要按 y 确认删除";   
    param_cache_clear->demo.push_back("ucd cache_clear                              (删除全部缓存图片)");
    param_cache_clear->demo.push_back("ucd cache_clear test.json                    (删除 test.json 对应的全部缓存图片)");
    param_cache_clear->demo.push_back("ucd cache_clear test.json -r                 (删除 test.json 除外的所有图片缓存)");
    UcdParamOpt::add_param(param_cache_clear);
    
    // cache_clean
    ParamInfo * param_cache_clean = new ParamInfo("cache_clean");
    param_cache_clean->group = "cache";
    param_cache_clean->grammar = "ucd cache_clean {img_folder}";
    param_cache_clean->english_explain = "clear cache";
    param_cache_clean->chinese_explain = "清洗缓存数据中大小为 0 的图片(有问题的图片)";   
    param_cache_clean->demo.push_back("ucd cache_clean                              (将缓存中(cache_dir/img_cache)错误的图片全部删除(大小为 0 的图片)))");
    param_cache_clean->demo.push_back("ucd cache_clean img_dir                      (将 img_dir 文件夹中错误的图片全部删除(大小为 0 的图片)))");
    UcdParamOpt::add_param(param_cache_clean);

    // to_yolo_txt
    ParamInfo * param_to_yolo_txt = new ParamInfo("to_yolo_txt");
    param_to_yolo_txt->group = "convert";
    param_to_yolo_txt->grammar = "ucd to_yolo_txt ucd_path save_dir {label_list}";
    param_to_yolo_txt->english_explain = "convert ucd to yolo train txt (format)";
    param_to_yolo_txt->chinese_explain = "将 ucd 转为 yolo txt 格式的数据，需要指定转换的标签，不指定的标签不转换";
    param_to_yolo_txt->demo.push_back("ucd to_yolo_txt test_1.json ./yolo_train_txt                         (使用 test_1.json 中的 label_used 属性，将 test_1.json 中的目标转为 yolo 格式的 txt 数据，存放到 ./yolo_train_txt 文件夹中)");   
    param_to_yolo_txt->demo.push_back("ucd to_yolo_txt test_2.json ./yolo_train_txt Fnormal,fzc_broken      (指定需要转为 txt 的标签)");   
    UcdParamOpt::add_param(param_to_yolo_txt);

    // to_yolo_train_data
    ParamInfo * param_to_yolo_train_data = new ParamInfo("to_yolo_train_data");
    param_to_yolo_train_data->group = "convert";
    param_to_yolo_train_data->args_info["--ratio"]  = "划分 train 和 val 的比例，default = 0.8";
    param_to_yolo_train_data->args_info["--tags"]   = "指定需要训练的标签";
    param_to_yolo_train_data->args_info["--iou_th"] = "裁剪范围时，确定被裁剪的目标是否保留使用的阈值";
    param_to_yolo_train_data->args_info["--assign_tag"] = "指定一个标签，作为裁剪范围";
    param_to_yolo_train_data->grammar = "ucd to_yolo_train_data save_dir";
    param_to_yolo_train_data->english_explain = "convert ucd to yolo train data";
    param_to_yolo_train_data->chinese_explain = "将 ucd 转为 yolo 训练的格式，train/images|labels, val/images|labels";
    param_to_yolo_train_data->demo.push_back("ucd to_yolo_train_data test.json train_dir --ratio 0.8                    (将 test.json 整理为 0.8 | 0.2 的 yolo 训练数据)");   
    param_to_yolo_train_data->demo.push_back("ucd to_yolo_train_data test.json train_dir --tags  tag1,tag2,tag3         (将 test.json 中的 tag1,tag2,tag3 三个标签整理为yolo 训练数据)");   
    param_to_yolo_train_data->demo.push_back("ucd to_yolo_train_data test.json train_dir --iou_th 0.85 --assign_tag kg  (将 test.json 使用阈值为 0.85 裁剪出 kg 的范围，制作 yolo 训练数据)");   
    UcdParamOpt::add_param(param_to_yolo_train_data);

    // to_yolo_classify_train_data
    ParamInfo * param_to_yolo_classify_train_data = new ParamInfo("to_yolo_classify_train_data");
    param_to_yolo_classify_train_data->group = "convert";
    param_to_yolo_classify_train_data->args_info["--ratio"]  = "划分 train 和 val 的比例，default = 0.8";
    param_to_yolo_classify_train_data->args_info["--tags"]   = "指定需要训练的标签用逗号隔开";
    param_to_yolo_classify_train_data->grammar = "ucd to_yolo_classify_train_data save_dir";
    param_to_yolo_classify_train_data->english_explain = "convert ucd to yolo train data";
    param_to_yolo_classify_train_data->chinese_explain = "将 ucd 转为 yolo 训练的格式，train/images|labels, val/images|labels";
    param_to_yolo_classify_train_data->demo.push_back("ucd to_yolo_classify_train_data test.json train_dir --ratio 0.8                    (将 test.json 整理为 0.8 | 0.2 的 yolo 训练数据)");   
    param_to_yolo_classify_train_data->demo.push_back("ucd to_yolo_classify_train_data test.json train_dir --tags  tag1,tag2,tag3         (将 test.json 中的 tag1,tag2,tag3 三个标签整理为yolo 训练数据)");      
    UcdParamOpt::add_param(param_to_yolo_classify_train_data);

    // to_img
    ParamInfo * param_to_img = new ParamInfo("to_img");
    param_to_img->group = "convert";
    param_to_img->grammar = "ucd to_img ucd_path save_dir";
    param_to_img->english_explain = "convert ucd to jpg (format)";
    param_to_img->chinese_explain = "将 ucd 中对应的图片保存到指定文件夹";
    param_to_img->demo.push_back("ucd to_img test.json ./img                        ( 将 test.json 中包含的图片保存到 ./img 文件夹下)");   
    UcdParamOpt::add_param(param_to_img);

    // to_xml
    ParamInfo * param_to_xml = new ParamInfo("to_xml");
    param_to_xml->group = "convert";
    param_to_xml->grammar = "ucd to_xml ucd_path save_dir";
    param_to_xml->english_explain = "convert ucd to xml (format)";
    param_to_xml->chinese_explain = "将 ucd 中对应的图片保存到指定文件夹";
    param_to_xml->demo.push_back("ucd to_xml test.json ./img                        ( 将 test.json 中包含的xml信息保存到 ./xml 文件夹下)");   
    UcdParamOpt::add_param(param_to_xml);

    // history
    ParamInfo * param_history = new ParamInfo("history");
    param_history->group = "info";
    param_history->grammar = "ucd history";
    param_history->args_info["--line"] = "指定处理最后多少行";
    param_history->args_info["-u"] = "输出时，去除重复命令的行，统计时不受影响";
    param_history->args_info["-i"] = "对使用的 ucd 命令进行统计";
    param_history->chinese_explain = "打印之前使用的 ucd 命令";
    param_history->demo.push_back("ucd history                  (打印 ucd 所有历史使用命令)");   
    param_history->demo.push_back("ucd history --line 20        (打印 ucd 使用的最后 20 行使用命令)");   
    param_history->demo.push_back("ucd history -i               (打印 ucd 关键字使用频次分布)");   
    param_history->demo.push_back("ucd history -i --line 100    (打印 ucd 关键字最后一百行命令使用频次分布)");   
    UcdParamOpt::add_param(param_history);

    // acc
    ParamInfo * param_acc = new ParamInfo("acc");
    param_acc->group = "analysis";
    param_acc->grammar = "ucd acc ucd_customer, ucd_standard {compare_res_ucd_path}";
    param_acc->args_info["--iou"] = "指定计算 acc 使用的 iou, 默认值为 0.5";
    param_acc->english_explain = "get acc rec from two ucd";
    param_acc->chinese_explain = "两个 ucd 之间计算精准率和召回率, ucd_customer 自定义检测结果, ucd_standard: 标准检测结果";   
    param_acc->demo.push_back("ucd acc test1.json test2.json                               (将 test1.json 与 test2.json 计算召回率和准确率打印出来)");
    param_acc->demo.push_back("ucd acc test1.json test2.json res.json                      (将 test1.json 与 test2.json 计算召回率和准确率，结果保存在 res.json 中)");
    param_acc->demo.push_back("-------------------");
    param_acc->demo.push_back("mistake a:b , 错把 a 检测成了 b");
    param_acc->demo.push_back("-------------------");
    UcdParamOpt::add_param(param_acc);

    // map
    ParamInfo * param_map = new ParamInfo("map");
    param_map->group = "analysis";
    param_map->grammar = "ucd map ucd_customer ucd_standard {save_path}";
    param_map->args_info["--iou"] = "指定计算 map 使用的 iou, 默认值为 0.5";
    param_map->chinese_explain = "以后一个 ucd 为标准结果计算 map, 可以保存检测结果为 TXT 使用 python 画图";   
    param_map->demo.push_back("ucd map test1.json test2.json                               (以 test2.json 为标准结果计算 test1.json 的 map");
    param_map->demo.push_back("ucd map test1.json test2.json  chart.txt                    (以 test2.json 为标准结果计算 test1.json 的 map, 将信息保存到 chart.txt 中去");
    UcdParamOpt::add_param(param_map);

    // attr
    ParamInfo * param_attr = new ParamInfo("attr");
    param_attr->group = "opt";
    param_attr->grammar = "ucd attr ucd_path attr_name attr_value";
    param_attr->english_explain = "change attr in ucd";
    param_attr->chinese_explain = "修改ucd中使用 ucd info 能看到的属性";   
    param_attr->demo.push_back("ucd attr test.json model_name test_model_name   (将 test.json 中的模型名更新为 test_model_name)");
    param_attr->demo.push_back("ucd attr test.json label_used nc,kkx            (将 test.json 中的label_used 属性更新为 nc加上kkx)");
    UcdParamOpt::add_param(param_attr);

    // help
    ParamInfo * param_help = new ParamInfo("help");
    param_help->group = "info";
    param_help->grammar = "ucd help command | group";
    param_help->english_explain = "print command info";
    param_help->chinese_explain = "打印指定 command 对应的信息";   
    param_help->demo.push_back("ucd help                                        (查看所有关键字的中文解释)");
    param_help->demo.push_back("ucd help help                                   (查看 help 关键字的使用方法)");
    param_help->demo.push_back("ucd help acc                                    (查看 acc 关键字的使用方法)");
    param_help->demo.push_back("ucd help opt                                    (查看 opt 组中的所有关键字)");
    UcdParamOpt::add_param(param_help);

    // uc_check
    ParamInfo * param_uc_check = new ParamInfo("uc_check");
    param_uc_check->group = "rename";
    param_uc_check->grammar = "ucd uc_check file_dir";
    param_uc_check->english_explain = "check if all file'name is uc";
    param_uc_check->chinese_explain = "查看是否所有文件以 UC 格式进行命名的(只检查 .jpg, .JPG, .png, .PNG, .json, .xml 类型的数据)";
    param_uc_check->demo.push_back("ucd uc_check ./img                          (检查 ./img 文件夹下指定后的数据有多少文件是符合 uc 命名的))");
    UcdParamOpt::add_param(param_uc_check);

    // buddha blass me
    ParamInfo * param_buddha_bless = new ParamInfo("buddha_bless");
    param_buddha_bless->group = "fun";
    param_buddha_bless->grammar = "ucd buddha_bless {name}";
    param_buddha_bless->english_explain = "buddha bless";
    param_buddha_bless->chinese_explain = "佛陀保佑";   
    param_buddha_bless->demo.push_back("ucd buddha_bless                        (敲电子木鱼，见机甲如来，阿弥陀佛)");
    UcdParamOpt::add_param(param_buddha_bless);

    // move_uc
    ParamInfo * param_move_uc = new ParamInfo("move_uc");
    param_move_uc->group = "rename";
    param_move_uc->grammar = "ucd move_uc file_dir save_dir";
    param_move_uc->english_explain = "move file with name in uc format";
    param_move_uc->chinese_explain = "将文件件中所有符合 uc 命名的移动到指定文件夹中，不限定后缀类型";  
    param_move_uc->demo.push_back("ucd move_uc ./test ./res                     (将 ./test 文件夹下符合 uc 命名规范的指定后缀的文件移动到 ./res 文件夹下面)"); 
    UcdParamOpt::add_param(param_move_uc);

    // move_not_uc
    ParamInfo * param_move_not_uc = new ParamInfo("move_not_uc");
    param_move_not_uc->group = "rename";
    param_move_not_uc->grammar = "ucd move_not_uc file_dir save_dir";
    param_move_not_uc->english_explain = "move file when filename not in uc format";
    param_move_not_uc->chinese_explain = "将文件件中所有不符合 uc 命名的移动到指定文件夹中";   
    param_move_not_uc->demo.push_back("ucd move_not_uc ./test ./res             (将 ./test 文件夹下不符合 uc 命名规范的指定后缀的文件移动到 ./res 文件夹下面)");
    UcdParamOpt::add_param(param_move_not_uc);


    // from_crop
    ParamInfo * param_from_crop = new ParamInfo("from_crop");
    param_from_crop->group = "opt";
    param_from_crop->args_info["-o"] = "不使用文件夹的名字作为小图的标签，直接使用小图文件名中包含的标签作为 obj 的 tag";
    param_from_crop->grammar = "ucd from_crop crop_dir save_path";
    param_from_crop->english_explain = "cut img to xml";
    param_from_crop->chinese_explain = "截图生成xml";   
    param_from_crop->demo.push_back("ucd from_crop ./crop res.json                 (将 ./crop 文件夹中的小图文件夹 生成 res.json 文件，使用文件夹的名字作为小图标签)");
    param_from_crop->demo.push_back("ucd from_crop ./crop res.json   -o            (将 ./crop 文件夹中的小图文件夹 生成 res.json 文件，使用小图的名字作为小图标签)");
    UcdParamOpt::add_param(param_from_crop);
    
    // from_assign_crop_xml
    ParamInfo * param_from_assign_crop_xml = new ParamInfo("from_assign_crop_xml");
    param_from_assign_crop_xml->group = "opt";
    param_from_assign_crop_xml->grammar = "ucd param_from_assign_crop_xml xml_dir save_path";
    param_from_assign_crop_xml->chinese_explain = "将裁剪后的小图 xml 转为 json";   
    param_from_assign_crop_xml->demo.push_back("ucd from_assign_crop_xml ./crop_xml res.json         (将 ./crop_xml 文件夹中的小图xml 生成 res.json 文件)");
    UcdParamOpt::add_param(param_from_assign_crop_xml);
    
    // todo
    ParamInfo * param_todo = new ParamInfo("todo");
    param_todo->group = "fun";
    param_todo->args_info["--date"] = "指定要操作的日期，有两种格式 2023-06-14 或者 06-14，不指定的话就使用今天的日期";
    param_todo->args_info["--name"] = "指定要查看用户的 name，如不指定直接使用 ucd meta 中 redis_name 作为用户名";
    param_todo->grammar = "ucd todo {method} {info}";
    param_todo->english_explain = "";
    param_todo->chinese_explain = "计划表，todo 表，目前支持的方法为 [check, add, del, done, undo]";   
    param_todo->demo.push_back("ucd todo                                           (查看四天前和今天的日程表)");
    param_todo->demo.push_back("ucd todo check                                     (查看今天的日程表)");
    param_todo->demo.push_back("ucd todo check --name jokker                       (查看今天 jokker 的日程表)");
    param_todo->demo.push_back("ucd todo check --date  2023-06-14                  (查看2023-06-14的日程表)");
    param_todo->demo.push_back("ucd todo add test1                                 (今天的日程表增加信息 test1)");
    param_todo->demo.push_back("ucd todo add test1 --date  2023-06-14              (2023-06-14的日程表增加信息 test1)");
    param_todo->demo.push_back("ucd todo del 1 --date  2023-06-14                  (2023-06-14的日程表删除第一条信息)");
    param_todo->demo.push_back("ucd todo del all --date  2023-06-14                (2023-06-14的日程表删除所有的信息)");
    param_todo->demo.push_back("ucd todo done 1 --date  2023-06-14                 (2023-06-14的日程表第一条记录已完成)");
    param_todo->demo.push_back("ucd todo done all --date  2023-06-14               (2023-06-14的日程表全部记录已完成)");
    param_todo->demo.push_back("ucd todo undo 1 --date  2023-06-14                 (2023-06-14的日程表第一条记录设置为未完成)");
    param_todo->demo.push_back("ucd todo undo all --date  2023-06-14               (2023-06-14的日程表全部记录设置为未完成)");
    UcdParamOpt::add_param(param_todo);

    // img_server
    ParamInfo * param_img_server = new ParamInfo("img_server");
    param_img_server->group = "server";
    param_img_server->args_info["--port"] = "指定服务提供的端口, 默认端`口为 5001";
    param_img_server->args_info["--img_dir"] = "图片服务提供的图片文件夹，默认使用 ucd 的缓存文件夹";
    param_img_server->grammar = "ucd img_server";
    param_img_server->chinese_explain = "当 80 服务器服务断掉之后可以暂时使用这个图片服务在各个服务器之间进行数据的转移";   
    param_img_server->demo.push_back("ucd img_server                                            (使用默认配置提供图片服务)");
    param_img_server->demo.push_back("ucd img_server --port 11223 --img_dir /home/ldq/img_dir   (使用指定的端口号和图片文件夹提供图片服务)");
    UcdParamOpt::add_param(param_img_server);

    // post_v2
    ParamInfo * param_post_v2 = new ParamInfo("post_v2");
    param_post_v2->group = "server";
    param_post_v2->args_info["--host"] = "检测服务器 host, 默认为 192.168.3.221";
    param_post_v2->args_info["--port"] = "检测服务器的 port, 默认为 11101";
    param_post_v2->args_info["--post_port"] = "指定推送的端口, 默认会找空闲的端口作为推送端口";
    param_post_v2->args_info["--batch_id"] = "批次的 id, 不要重复，默认为 test_ucd_post_v2";
    param_post_v2->args_info["--model_list"] = "需要检测的模型列表, 默认为 nc,kkx";
    param_post_v2->grammar = "ucd post_v2 ucd_path {save_dir}";
    param_post_v2->chinese_explain = "将检测推送给 v2 接口，并将返回结果打印并保存到文件夹中";   
    param_post_v2->demo.push_back("ucd post_v2 test.json                                (将 test.json 中的图片推送给 http://192.168.3.221:111/dete 进行检测)");
    param_post_v2->demo.push_back("ucd post_v2 test.json ./xml_dir                      (将 test.json 中的图片推送给 http://192.168.3.221:111/dete 进行检测，结果保存到 xml_dir 中)");
    param_post_v2->demo.push_back("ucd post_v2 test.json ./xml_dir --model_list test    (将 test.json 中的图片推送给 http://192.168.3.221:111/dete 进行检测，结果保存到 xml_dir 中, 指定检测 test 模型)");
    param_post_v2->demo.push_back("ucd post_v2 test.json ./xml_dir --model_list test --host 192.168.3.33 --port 111 --batch_id test_001   (将 test.json 中的图片推送给 http://192.168.3.221:111/dete 进行检测，结果保存到 xml_dir 中, 指定检测 test 模型)");
    UcdParamOpt::add_param(param_post_v2);


    // from_file
    ParamInfo * param_from_file = new ParamInfo("from_file");
    param_from_file->group = "convert";
    param_from_file->grammar = "ucd from_file file_dir ucd_path";
    param_from_file->english_explain = "";
    param_from_file->chinese_explain = "(递归)只获取指定文件夹下面文件的 uc 组织成一个 ucd, 不去解析具体文件中的内容 ";  
    param_from_file->demo.push_back("ucd from_file ./test test.json                 (将 ./test 文件夹下面名字符合 uc 规范的 uc 组合形成 uc_list 保存到 test.json)"); 
    UcdParamOpt::add_param(param_from_file);

    // uc_analysis
    ParamInfo * param_uc_analysis = new ParamInfo("uc_analysis");
    param_uc_analysis->group = "analysis";
    param_uc_analysis->grammar = "ucd uc_analysis ucd_path";
    param_uc_analysis->english_explain = "";
    param_uc_analysis->chinese_explain = "分析 uc 的组成, 看前三位即可, uc 前三位相同的算作同一天入库的数据，主要查看文件入库的日期的分布";
    param_uc_analysis->demo.push_back("ucd uc_analysis test.json                    (分析 test.json 中的 uc 都是哪一天入库的)");   
    UcdParamOpt::add_param(param_uc_analysis);

    // filter_by_conf
    ParamInfo * param_filter_by_conf = new ParamInfo("filter_by_conf");
    param_filter_by_conf->group = "filter";
    param_filter_by_conf->grammar = "ucd filter_by_conf ucd_path save_ucd_path conf_th";
    param_filter_by_conf->english_explain = "";
    param_filter_by_conf->chinese_explain = "对 ucd 进行指定阈值过滤"; 
    param_filter_by_conf->demo.push_back("ucd filter_by_conf test.json res.json 0.5 (以 0.5 为阈值对 test.json 中的所有 obj 对象进行过滤)");  
    UcdParamOpt::add_param(param_filter_by_conf);
    
    // filter_by_area
    ParamInfo * param_filter_by_area = new ParamInfo("filter_by_area");
    param_filter_by_area->group = "filter";
    param_filter_by_area->grammar = "ucd filter_by_area ucd_path save_ucd_path area_th";
    param_filter_by_area->english_explain = "";
    param_filter_by_area->chinese_explain = "对 ucd 进行指定面积阈值过滤"; 
    param_filter_by_area->demo.push_back("ucd filter_by_area test.json res.json 500 (以 500 为阈值对 test.json 中的所有 obj 对象进行过滤)");  
    UcdParamOpt::add_param(param_filter_by_area);

    // filter_by_nms
    ParamInfo * param_filter_by_nms = new ParamInfo("filter_by_nms");
    param_filter_by_nms->group = "filter";
    param_filter_by_nms->grammar = "ucd filter_by_nms ucd_path save_ucd_path nms_th ignore_tag(true|false)";
    param_filter_by_nms->english_explain = "do nms ";
    param_filter_by_nms->chinese_explain = "对 obj 进行 nms 操作";  
    param_filter_by_nms->demo.push_back("ucd filter_by_nms test.json res.json 0.1 1 (以 nms=0.1 为参数 test.json 中的所有对象进行 nms 操作, 不同标签之间也做 nms)");      
    param_filter_by_nms->demo.push_back("ucd filter_by_nms test.json res.json 0.1 0 (以 nms=0.1 为参数 test.json 中的所有对象进行 nms 操作, 不同标签之间不做 nms)");      
    UcdParamOpt::add_param(param_filter_by_nms);

    // filter_by_uc
    ParamInfo * param_filter_by_uc = new ParamInfo("filter_by_uc");
    param_filter_by_uc->group = "filter";
    param_filter_by_uc->grammar = "ucd filter_by_uc ucd_path save_ucd_path uc1 uc2 uc3";
    param_filter_by_uc->english_explain = "do nms ";
    param_filter_by_uc->chinese_explain = "ucd 中只保留指定的 uc 去掉其他的 uc 信息";  
    param_filter_by_uc->demo.push_back("ucd filter_by_uc test.json res.json Fuc0001 Fuc0002  (test.json 中除了 Fuc0001 Fuc0002 两个 uc 其他所有uc都去掉 保存为 res.json)");      
    UcdParamOpt::add_param(param_filter_by_uc);

    // conf_analysis
    ParamInfo * param_conf_analysis = new ParamInfo("conf_analysis");
    param_conf_analysis->group = "analysis";
    param_conf_analysis->grammar = "ucd conf_analysis ucd_path";
    param_conf_analysis->english_explain = "";
    param_conf_analysis->chinese_explain = "置信度分析，分析 ucd 中存放的置信度信息";
    param_conf_analysis->demo.push_back("ucd conf_analysis test.json      (将置信度 0-1 划分为 10 个区间对 test.json 中的所有 obj 的置信度进行统计)");   
    UcdParamOpt::add_param(param_conf_analysis);

    // area_analysis
    ParamInfo * param_area_analysis = new ParamInfo("area_analysis");
    param_area_analysis->group = "analysis";
    param_area_analysis->grammar = "ucd area_analysis ucd_path seg_count";
    param_area_analysis->english_explain = " ";
    param_area_analysis->chinese_explain = "面积分析，分析所有对象的面积分布"; 
    param_area_analysis->demo.push_back("ucd area_analysis test.json 10     (将面积平均划分为 10 个区间对 test.json 中的所有 obj 的面积进行统计)");   
    UcdParamOpt::add_param(param_area_analysis);
    
    // get
    ParamInfo * param_get = new ParamInfo("get");
    param_get->group = "info";
    param_get->grammar = "ucd get attr_name ucd_path";
    param_get->english_explain = "";
    param_get->chinese_explain = "获取 ucd 中的信息，[tags, dataset_name, uc_count, label_used_count, model_name, model_version, add_time, update_time, describe, label_used, uc_list]";   
    param_get->demo.push_back("ucd get label_used aqm.json                  (获取 aqm.json 数据中的 label_used 属性信息)");
    param_get->demo.push_back("ucd get tags aqm.json                        (获取 aqm.json 数据中的所有标签，用逗号隔开打印出来)");
    UcdParamOpt::add_param(param_get);

    // drop
    ParamInfo * param_drop = new ParamInfo("drop");
    param_drop->group = "opt";
    param_drop->grammar = "ucd drop attr_name ucd_path save_ucd_path";
    param_drop->english_explain = "";
    param_drop->chinese_explain = "删除 ucd 中的信息 dataset_name, object_info, size_info, model_name, model_version, add_time, update_time, describe, label_used, uc_list]";   
    param_drop->demo.push_back("ucd drop object_info aqm.json aqm_drop_object_info.json     (将 aqm.json 中的 object_info 信息给抹去，保存为 aqm_drop_object_info.json)");
    UcdParamOpt::add_param(param_drop);
    
    // has_uc
    ParamInfo * param_has_uc = new ParamInfo("has_uc");
    param_has_uc->group = "info";
    param_has_uc->grammar = "ucd has_uc ucd_path assign_uc_1 assign_uc_2";
    param_has_uc->english_explain = "";
    param_has_uc->chinese_explain = "判断指定的 uc 是否在 ucd 的 uc_list 中";   
    param_has_uc->demo.push_back("ucd has_uc aqm.json Die11mk Die12mk Die13mk Die14mk            (查看 aqm.json 中是否包含 uc Die11mk Die12mk Die13mk Die14mk)");
    UcdParamOpt::add_param(param_has_uc);
    
    // sub
    ParamInfo * param_sub = new ParamInfo("sub");
    param_sub->group = "opt";
    param_sub->grammar = "ucd sub ucd_path save_ucd_path need_count is_random";
    param_sub->english_explain = "";
    param_sub->chinese_explain = "从 ucd 中取出子序列，可以选择是否为随机选取";   
    param_sub->demo.push_back("ucd sub aqm.json aqm_sub.json 10 1           (随机从 aqm.json 中选取 10 个uc 保存为 aqm_sub.json)");
    param_sub->demo.push_back("ucd sub aqm.json aqm_sub.json 10 0           (顺序从 aqm.json 中选取 10 个uc 保存为 aqm_sub.json)");
    UcdParamOpt::add_param(param_sub);

    // split
    ParamInfo * param_split = new ParamInfo("split");
    param_split->group = "opt";
    param_split->grammar = "ucd split ucd_path save_ucd_path_a save_ucd_path_a ratio";
    param_split->english_explain = "";
    param_split->chinese_explain = "将 ucd 按照比例划分为两个部分";   
    param_split->demo.push_back("ucd split aqm.json aqm_a.json aqm_b.json 0.2       (将 aqm.json 数据集按照 0.2 0.8 划分成两个数据集 aqm_a.json aqm_b.json)");
    param_split->demo.push_back("ucd split aqm.json aqm_a.json aqm_b.json 0.5       (将 aqm.json 数据集按照 0.5 0.5 划分成两个数据集 aqm_a.json aqm_b.json)");
    UcdParamOpt::add_param(param_split);

    // split_by_date
    ParamInfo * param_split_by_date = new ParamInfo("split_by_date");
    param_split_by_date->group = "opt";
    param_split_by_date->grammar = "ucd split_by_date ucd_path save_dir";
    param_split_by_date->args_info["--save_name"] = "指定保存的名字，保存格式为 save_name_date.json, 当不指定时，保存名形如 origin_name_date.json";
    param_split_by_date->english_explain = "";
    param_split_by_date->chinese_explain = "将 ucd 按照uc 的前三位（日期）划分为多个 json 文件";   
    param_split_by_date->demo.push_back("ucd split_by_date aqm.json ./res                   (将 aqm.json 根据时间划分为多个 json 保存在 ./res 文件夹)");
    param_split_by_date->demo.push_back("ucd split_by_date aqm.json ./res --save_name test  (将 aqm.json 根据时间划分为多个 json 保存在 ./res 文件夹，指定保存的名字)");
    UcdParamOpt::add_param(param_split_by_date);
   

    // split_by_tags
    ParamInfo * param_split_by_tags = new ParamInfo("split_by_tags");
    param_split_by_tags->group = "opt";
    param_split_by_tags->grammar = "ucd split_by_tags ucd_path save_dir";
    param_split_by_tags->args_info["--save_name"] = "指定保存的名字，保存格式为 save_name_date.json, 当不指定时，保存名形如 origin_name_tag.json";
    param_split_by_tags->english_explain = "";
    param_split_by_tags->chinese_explain = "将 ucd 按照uc 的前三位（日期）划分为多个 json 文件";   
    param_split_by_tags->demo.push_back("ucd split_by_tags aqm.json ./res                   (将 aqm.json 根据标签划分为多个 json 保存在 ./res 文件夹)");
    param_split_by_tags->demo.push_back("ucd split_by_tags aqm.json ./res --save_name test  (将 aqm.json 根据标签划分为多个 json 保存在 ./res 文件夹，指定保存的名字)");
    UcdParamOpt::add_param(param_split_by_tags);
   

    // split_by_conf
    ParamInfo * param_split_by_conf = new ParamInfo("split_by_conf");
    param_split_by_conf->group = "opt";
    param_split_by_conf->grammar = "ucd split_by_conf ucd_path save_path {-t} ";
    param_split_by_conf->args_info["-t"] = "只会改变标签，并按照置信度生成多个 json";
    param_split_by_conf->chinese_explain = "将 ucd 按照置信度进行划分，两种模式（1）不同的置信度分为不同的 json （2）将置信度信息加在 tag 后面，只生成一个文件";   
    param_split_by_conf->demo.push_back("ucd split_by_conf test.json res.json -c             (将 test.json 的标签根据置信度分裂为多个，不理解的话运行一下就知道了)");
    UcdParamOpt::add_param(param_split_by_conf);
   
    // update
    ParamInfo * param_update = new ParamInfo("update");
    param_update->group = "meta_info";
    param_update->grammar = "ucd update {version}";
    param_update->english_explain = "";
    param_update->chinese_explain = "对 ucd 进行更新，只下载并配置库文件，还需要手动去改 ~/.bash_aliases 文件";   
    param_update->demo.push_back("ucd update                                        (将 ucd 更新到最新版本)");
    param_update->demo.push_back("ucd update v1.4.7                                 (将 ucd 版本更新带 ucd_v1.4.7)");
    UcdParamOpt::add_param(param_update);

    // install
    ParamInfo * param_install = new ParamInfo("install");
    param_install->group = "sync";
    param_install->grammar = "ucd install";
    param_install->english_explain = "";
    param_install->chinese_explain = "打印在其他机器上安装 ucd 的命令";   
    param_install->demo.push_back("ucd install                                        (将 ucd 更新到最新版本)");
    UcdParamOpt::add_param(param_install);


    // absorb
    ParamInfo * param_absorb = new ParamInfo("absorb");
    param_absorb->group = "opt";
    param_absorb->grammar = "ucd absorb ucd_path ucd_be_absorb_path ucd_save_path absorb_attr";
    param_absorb->english_explain = "";
    param_absorb->chinese_explain = "ucd 中缺失的数据从其他 ucd 中吸收而来, 只吸收 uc_list 中包含的 uc";   
    param_absorb->demo.push_back("ucd absorb aqm.json all.json aqm_absorb.json object_info      (使用 all.json 为 aqm.json 补充缺少的 object_info 信息)");
    param_absorb->demo.push_back("ucd absorb aqm.json all.json aqm_absorb.json size_info        (使用 all.json 为 aqm.json 补充缺少的 size_info 信息)");
    param_absorb->demo.push_back("ucd absorb aqm.json all.json aqm_absorb.json all              (使用 all.json 为 aqm.json 补充缺少的 size_info object_info 信息)");
    UcdParamOpt::add_param(param_absorb);

    // devide
    ParamInfo * param_devide = new ParamInfo("devide");
    param_devide->group = "opt";
    param_devide->grammar = "ucd devide ucd_path save_dir save_name devide_count";
    param_devide->english_explain = "";
    param_devide->chinese_explain = "将 ucd 随机平均地划分为指定的份数";   
    param_devide->demo.push_back("ucd devide test.json ./ devide 5                            (将 test.json 随机划分为五份, 保存路径形式 devide_1.json, devide_2.json ... )");
    UcdParamOpt::add_param(param_devide);

    // uc_info
    ParamInfo * param_uc_info = new ParamInfo("uc_info");
    param_uc_info->group = "info";
    param_uc_info->grammar = "ucd uc_info ucd_path uc1 uc2 uc3";
    param_uc_info->english_explain = "";
    param_uc_info->chinese_explain = "查看执行 uc 的对应信息";   
    param_uc_info->demo.push_back("ucd uc_info test.json Dsm07qp                                (将 Dsm07qp 相关的信息打印出来)");
    param_uc_info->demo.push_back("ucd uc_info test.json Dsm07qp Fuc0001                        (将 Dsm07qp Fuc0001 相关的信息打印出来)");
    UcdParamOpt::add_param(param_uc_info);

    // exec
    ParamInfo * param_exec = new ParamInfo("exec");
    param_exec->group = "exec";
    param_exec->grammar = "ucd exec ucd_path command_path(.ucd) save_path";
    param_exec->english_explain = "";
    param_exec->chinese_explain = "执行 .ucd 命令文件";   
    param_exec->demo.push_back("ucd exec test.json test.ucd save_path.json                      (对 test.json 执行 test.ucd 命令文件，将结果保存在 save_path.json)");
    param_exec->demo.push_back("----------- test.ucd --------------");
    param_exec->demo.push_back("ADD  UC Dcp08jb");
    param_exec->demo.push_back("ADD  SIZE_INFO Dcp08jb   123 456");
    param_exec->demo.push_back("ADD  OBJECT_INFO Dcp08jb rectangle test_exec 0.5 [(10,10),(20,20)] ");
    param_exec->demo.push_back("DROP ALL UC");
    param_exec->demo.push_back("DROP ALL OBJECT_INFO");
    param_exec->demo.push_back("DROP ALL SIZE_INFO");
    param_exec->demo.push_back("DROP UC Dcp08jb");
    param_exec->demo.push_back("DROP SIZE_INFO Dcp08jb");
    param_exec->demo.push_back("DROP OBJECT_INFO Dcp08jb rectangle test_exec 0.5 [(10,10),(20,20)]");
    param_exec->demo.push_back("SET  DATASET_NAME test_exec");
    param_exec->demo.push_back("SET  MODEL_NAME test_exec_set");
    param_exec->demo.push_back("SET  MODEL_VERSION v0.0.1");
    param_exec->demo.push_back("SET  DESCRIBE just for test");
    param_exec->demo.push_back("SET  LABEL_USED hello nc kkx fnc");
    param_exec->demo.push_back("----------- test.ucd --------------");
    UcdParamOpt::add_param(param_exec);

    // fake_uc
    ParamInfo * param_fake_uc = new ParamInfo("fake_uc");
    param_fake_uc->group = "rename";
    param_fake_uc->grammar = "ucd fake_uc img_folder";
    param_fake_uc->english_explain = "";
    param_fake_uc->chinese_explain = "在离线的情况下将执执行文件夹下面的 文件赋予 假的 uc (Fuc001 开始), 只针对 .jpg .JPG .png .PNG .xml .json 文件";   
    param_fake_uc->demo.push_back("ucd fake_uc img_folder                                (将 img_folder 下面的文件重命名为假的 uc)");
    UcdParamOpt::add_param(param_fake_uc);

    // update_tags
    ParamInfo * param_update_tags = new ParamInfo("update_tags");
    param_update_tags->group = "opt";
    param_update_tags->grammar = "ucd update_tags ucd_path save_path old_tag:new_tag old_tag:new_tag ...";
    param_update_tags->english_explain = "";
    param_update_tags->chinese_explain = "更新 ucd 中的标签";   
    param_update_tags->demo.push_back("ucd update_tags aqm.json res.json anquanmao:aqm cebiandai:cbd       (将 aqm.json 中的 anquanmao 改为 aqm cebiandai 改为 cbd 进行更新)");
    UcdParamOpt::add_param(param_update_tags);

    // filter_by_tags
    ParamInfo * param_filter_by_tags = new ParamInfo("filter_by_tags");
    param_filter_by_tags->group = "filter";
    param_filter_by_tags->args_info["-a"] = "设置 -a 即为 and 模式，当且仅当拥有所有指定标签是才保存 obj（and 模式不支持通配符 * ），否者为 or 模式，只有满足其中的一个条件就保留 obj";
    param_filter_by_tags->grammar = "ucd filter_by_tags ucd_path save_path tag1 tag2 ...";
    param_filter_by_tags->english_explain = "";
    param_filter_by_tags->chinese_explain = "数据集中只保留指定标签（当前版本已经能使用正则表达式进行匹配了）";   
    param_filter_by_tags->demo.push_back("ucd filter_by_tags aqm.json res.json aqm cbd person                   (将 aqm.json 只保留 aqm cbd person 三个标签)");
    param_filter_by_tags->demo.push_back("ucd filter_by_tags aqm.json res.json aqm*                             (将 aqm.json 只保留 aqm 开头的所有标签)");
    param_filter_by_tags->demo.push_back("ucd filter_by_tags aqm.json res.json aqm*ll                           (将 aqm.json 只保留 aqm 开头ll结尾的所有标签)");
    param_filter_by_tags->demo.push_back("ucd filter_by_tags aqm.json res.json aqm cbd -a                       (将 aqm.json 只保留 同时存在 aqm cbd 的 uc)");
    UcdParamOpt::add_param(param_filter_by_tags);

    // filter_by_date
    ParamInfo * param_filter_by_date = new ParamInfo("filter_by_date");
    param_filter_by_date->group = "filter";
    param_filter_by_date->grammar = "ucd filter_by_date ucd_path save_path date1,date2,date3";
    param_filter_by_date->english_explain = "";
    param_filter_by_date->chinese_explain = "数据集中只保留指定日期入库的数据";   
    param_filter_by_date->demo.push_back("ucd filter_by_date aqm.json res.json C                   (将 aqm.json 只保留 2021 年入库的数据)");
    param_filter_by_date->demo.push_back("ucd filter_by_date aqm.json res.json Dad                 (将 aqm.json 只保留 2022.01.04 入库的数据)");
    UcdParamOpt::add_param(param_filter_by_date);

    // filter_volume_by_tags
    ParamInfo * param_filter_volume_by_tags = new ParamInfo("filter_volume_by_tags");
    param_filter_volume_by_tags->group = "filter";
    param_filter_volume_by_tags->grammar = "ucd filter_volume_by_tags uci_path save_path volume_size tag1 tag2 ...";
    param_filter_volume_by_tags->english_explain = "";
    param_filter_volume_by_tags->chinese_explain = "分卷数据集中只保留指定标签";   
    param_filter_volume_by_tags->demo.push_back("ucd filter_volume_by_tags aqm.uci res.uci 10 aqm cbd person                   (将 aqm.uci 只保留 aqm cbd person 三个标签)");
    UcdParamOpt::add_param(param_filter_volume_by_tags);

    // drop_tags
    ParamInfo * param_drop_tags = new ParamInfo("drop_tags");
    param_drop_tags->group = "filter";
    param_drop_tags->grammar = "ucd drop_tags ucd_path save_path tag1 tag2 ...";
    param_drop_tags->english_explain = "";
    param_drop_tags->chinese_explain = "数据集中删除指定标签，当前版本已经能使用正则表达式进行匹配了";   
    param_drop_tags->demo.push_back("ucd drop_tags aqm.json res.json aqm cbd person                     (将 aqm.json 删除 aqm cbd person 三个标签)");
    param_drop_tags->demo.push_back("ucd drop_tags aqm.json res.json aqm*                               (将 aqm.json 删除 aqm 开头的所有标签)");
    UcdParamOpt::add_param(param_drop_tags);

    // drop_extra_info
    ParamInfo * param_drop_extra_info = new ParamInfo("drop_extra_info");
    param_drop_extra_info->group = "filter";
    param_drop_extra_info->grammar = "ucd drop_extra_info ucd_path save_path";
    param_drop_extra_info->english_explain = "";
    param_drop_extra_info->chinese_explain = "去除多余的 size_info, object_info, 叫做 drop_extra_info, 没有对应的 uc 所有的其他的信息都要删除";   
    param_drop_extra_info->demo.push_back("ucd param_drop_extra_info aqm.json res.json                     (将 aqm.json 中不应该有的 size_info 和 object_info 信息删除)");
    UcdParamOpt::add_param(param_drop_extra_info);

    // keep_only_uc
    ParamInfo * param_keep_only_uc = new ParamInfo("keep_only_uc");
    param_keep_only_uc->group = "filter";
    param_keep_only_uc->grammar = "ucd keep_only_uc ucd_path save_path";
    param_keep_only_uc->english_explain = "";
    param_keep_only_uc->chinese_explain = "数据集中只保留 uc 信息删除其他所有信息";   
    param_keep_only_uc->demo.push_back("ucd keep_only_uc aqm.json res.json                              (只保留 aqm.json 中的 uc 信息生成 res.json)");
    UcdParamOpt::add_param(param_keep_only_uc);

    // zen
    ParamInfo * param_zen = new ParamInfo("zen");
    param_zen->group = "fun";
    param_zen->grammar = "ucd zen";
    param_zen->english_explain = "";
    param_zen->chinese_explain = "ucd 的目的和宗旨";   
    param_zen->demo.push_back("ucd zen                                                                (查看 ucd 的目的)");
    UcdParamOpt::add_param(param_zen);

    // drop_empty_uc
    ParamInfo * param_drop_empty_uc = new ParamInfo("drop_empty_uc");
    param_drop_empty_uc->group = "filter";
    param_drop_empty_uc->grammar = "ucd drop_empty_uc ucd_path save_path";
    param_drop_empty_uc->english_explain = "";
    param_drop_empty_uc->chinese_explain = "删除没有 obj 元素的 uc";   
    param_drop_empty_uc->demo.push_back("ucd drop_empty_uc test.json test.json                          (从 test.json 中删除没有 obj 元素的 uc)");
    UcdParamOpt::add_param(param_drop_empty_uc);

    // draw
    ParamInfo * param_draw = new ParamInfo("draw");
    param_draw->group = "opt";
    param_draw->grammar = "ucd draw ucd_path save_dir {uc} {uc} {uc} ...";
    param_draw->args_info["-f"] = "force, 强制重画，存储路径有相同的图片，也会强制重新画一遍";
    param_draw->english_explain = "";
    param_draw->chinese_explain = "将 uc 中的 obj 画出来，可以在配置文件中指定各个标签的颜色，标签以 correct_, mistake_, miss_, extra_ 开头 当color.txt 未指定标签颜色时，会给指定的颜色";   
    param_draw->demo.push_back("ucd draw test.json ./draw                                       (将 test.json 中的所有 uc 中的 obj 画出并保存在 ./draw 文件夹下)");
    param_draw->demo.push_back("ucd draw test.json ./draw  -f                                   (将 test.json 中的所有 uc 中的 obj 画出并保存在 ./draw 文件夹下, 本地有画好的图片强制重画)");
    param_draw->demo.push_back("ucd draw test.json ./draw  Dem1iwe Dem1iwk Dem1iwy              (将 test.json 中的 Dem1iwe Dem1iwk Dem1iwy 三个 uc 画出并保存在 ./draw 文件夹下)");
    UcdParamOpt::add_param(param_draw);
    
    // random color
    ParamInfo * param_random_color = new ParamInfo("random_color");
    param_random_color->group = "opt";
    param_random_color->grammar = "ucd random_color ucd_path";
    param_random_color->english_explain = "";
    param_random_color->chinese_explain = "对 ucd 中的标签赋予随机的颜色，在配置文件中已有颜色的标签，颜色不变";   
    param_random_color->demo.push_back("ucd random_color test.json                                       (将 test.json 中的标签赋予随机的颜色)");
    UcdParamOpt::add_param(param_random_color);
    
    // random color
    ParamInfo * param_merge_all = new ParamInfo("merge_all");
    param_merge_all->group = "opt";
    param_merge_all->grammar = "ucd merge_all res.json ucd_dir";
    param_merge_all->english_explain = "";
    param_merge_all->chinese_explain = "将一个文件夹中的所有 ucd 进行合并";   
    param_merge_all->demo.push_back("ucd merge_all res.json ucd_dir                                       (将 ucd_dir 中的所有 ucd 文件进行合并生成 res.json)");
    UcdParamOpt::add_param(param_merge_all);
    
    // ls
    ParamInfo * param_ls = new ParamInfo("ls");
    param_ls->group = "opt_uci";
    param_ls->grammar = "ucd ls folder_dir";
    param_ls->english_explain = "";
    param_ls->chinese_explain = "查看指定文件夹下的 uci 数据集情况";   
    param_ls->demo.push_back("ucd ls ./uci_dir                                       (查看 ./uci_dir 文件夹下面有多少 .uci 数据集，以及数据集信息)");
    UcdParamOpt::add_param(param_ls);
    
    // mv
    ParamInfo * param_mv = new ParamInfo("mv");
    param_mv->group = "opt_uci";
    param_mv->grammar = "ucd mv uci_path save_path";
    param_mv->english_explain = "";
    param_mv->chinese_explain = "将 uci 数据集移动到指定位置";   
    param_mv->demo.push_back("ucd mv ./uci_dir/test.uci  ./res/res.uci                      (查看 ./uci_dir/test.uci 移动到 ./res/res.uci 位置)");
    UcdParamOpt::add_param(param_mv);

    // cp
    ParamInfo * param_cp = new ParamInfo("cp");
    param_cp->group = "opt_uci";
    param_cp->grammar = "ucd cp uci_path save_path";
    param_cp->english_explain = "";
    param_cp->chinese_explain = "将 uci 数据集移动到指定位置";   
    param_cp->demo.push_back("ucd cp ./uci_dir/test.uci  ./res/res.uci                      (查看 ./uci_dir/test.uci 拷贝到 ./res/res.uci 位置)");
    UcdParamOpt::add_param(param_cp);

    // rm
    ParamInfo * param_rm = new ParamInfo("rm");
    param_rm->group = "opt_uci";
    param_rm->grammar = "ucd rm uci_path";
    param_rm->english_explain = "";
    param_rm->chinese_explain = "删除 uci 数据集";   
    param_rm->demo.push_back("ucd rm ./uci_dir/test.uci                                     (删除 ./uci_dir/test.uci 和其对应的卷文件)");
    UcdParamOpt::add_param(param_rm);

    // json_to_uci
    ParamInfo * param_json_to_uci = new ParamInfo("json_to_uci");
    param_json_to_uci->group = "convert";
    param_json_to_uci->grammar = "ucd json_to_uci json_path uci_path {volume_size}";
    param_json_to_uci->english_explain = "";
    param_json_to_uci->chinese_explain = "将 ucd 从 .json 格式转为 .uci 格式";   
    param_json_to_uci->demo.push_back("ucd json_to_uci test.json test.uci                   (将 test.json 文件转为 test.uci 分卷文件)");
    param_json_to_uci->demo.push_back("ucd json_to_uci test.json test.uci   30              (将 test.json 文件转为 test.uci 分卷文件，指定每一卷大小为 30 大概 280M)");
    UcdParamOpt::add_param(param_json_to_uci);

    // uci_to_json
    ParamInfo * param_uci_to_json = new ParamInfo("uci_to_json");
    param_uci_to_json->group = "convert";
    param_uci_to_json->grammar = "ucd uci_to_json uci_path json_path {volume_size:30}";
    param_uci_to_json->english_explain = "";
    param_uci_to_json->chinese_explain = "将 ucd 从 .uci 格式转为 .json 格式";   
    param_uci_to_json->demo.push_back("ucd json_to_uci test.uci test.json                   (将 test.uci 文件转为 test.json 文件, 分卷大小默认为 30)");
    param_uci_to_json->demo.push_back("ucd json_to_uci test.uci test.json  20                (将 test.uci 文件转为 test.json 文件，分卷大小为 20)");
    UcdParamOpt::add_param(param_uci_to_json);
    
    // interset
    ParamInfo * param_interset = new ParamInfo("interset");
    param_interset->group = "opt";
    param_interset->grammar = "ucd interset save_path ucd_path_a ucd_path_b";
    param_interset->english_explain = "";
    param_interset->chinese_explain = "找到两个 ucd 的 uc_list 交集";   
    param_interset->demo.push_back("ucd interset res.json ucd_a.json ucd_b.json             (将 ucd_a.json 和 ucd_b.json 的交集保存在 res.json)");
    UcdParamOpt::add_param(param_interset);

    // foretell
    ParamInfo * param_foretell = new ParamInfo("foretell");
    param_foretell->group = "fun";
    param_foretell->grammar = "ucd foretell context";
    param_foretell->chinese_explain = "输入一段文字，输出一个预言";   
    param_foretell->demo.push_back("ucd foretell hello             (对 hello 进行预言)");
    UcdParamOpt::add_param(param_foretell);
    
    // grammar
    ParamInfo * param_grammar = new ParamInfo("grammar");
    param_grammar->group = "info";
    param_grammar->grammar = "ucd grammar";
    param_grammar->chinese_explain = "输出所有关键字的语法";   
    UcdParamOpt::add_param(param_grammar);

    // augment
    ParamInfo * param_augment = new ParamInfo("augment");
    param_augment->group = "opt";
    param_augment->grammar = "ucd augment ucd_path save_path x1 x2 y1 y2 (左右上下）{is_relative，默认相对缩放}";
    param_augment->chinese_explain = "对检测框进行缩放(相对|绝对)";   
    param_augment->demo.push_back("ucd augment test.json save.json 0.5 0.5 -0.1 -0.1 true  (使用相对的方式对 test.json 中的所有检测框的往左往右分别扩展 宽度 * 0.5, 往上往下分别缩小高度 * 0.1)");
    UcdParamOpt::add_param(param_augment);
    
    // fix_size_info
    ParamInfo * param_fix_size_info = new ParamInfo("fix_size_info");
    param_fix_size_info->group = "opt";
    param_fix_size_info->grammar = "ucd fix_size_info ucd_path save_path";
    param_fix_size_info->args_info["--no_cache"] = "1|True|true 低缓存模式，使用完下载的图片之后会删除，本地已有缓存的不进行删除";
    param_fix_size_info->args_info["--size_ucd"] = "用于辅助修复的 ucd 的路径，当辅助 ucd 中存在需要的 size_info 直接从辅助 ucd 中取而不是从缓存中获取";
    param_fix_size_info->chinese_explain = "对 size_info 信息进行修复";   
    param_fix_size_info->demo.push_back("ucd fix_size_info test.json save.json               (对 test.json 中的 size_info 信息进行修复)");
    param_fix_size_info->demo.push_back("ucd fix_size_info test.json save.json --no_cache 1   (使用低缓存模式对 test.json 中的 size_info 信息进行修复)");
    UcdParamOpt::add_param(param_fix_size_info);

}

void UcdParamOpt::not_ready(std::string method_name)
{
    if(method_name == "")
    {
        std::cout << "----------------------------" << std::endl;
        std::cout << "* this method is not ready" << std::endl;
        std::cout << "----------------------------" << std::endl;
    }
    else
    {
        std::cout << "----------------------------" << std::endl;
        std::cout << "* method " << method_name << " is not ready" << std::endl;
        std::cout << "----------------------------" << std::endl;
    }

}

