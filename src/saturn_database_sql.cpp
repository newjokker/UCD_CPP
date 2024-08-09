
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mysql/mysql.h>
#include <stdlib.h>
#include "include/saturn_database_sql.hpp"
#include "include/md5.hpp"
#include "include/fileOperateUtil.hpp"
#include "include/tqdm.h"


#define ERROR_COLOR         "\x1b[1;31m"    // 红色
#define HIGHTLIGHT_COLOR    "\033[1;35m"    // 品红
#define WARNNING_COLOR      "\033[1;33m"    // 橙色
#define STOP_COLOR          "\033[0m"       // 复原



static bool is_uc(std::string uc)
{
    if(uc.size() != 7){ return false; }
    if(((int)uc[0] < (int)'C') || ((int)uc[0] > int('K'))) { return false; }
    if(((int)uc[1] < (int)'a') || ((int)uc[1] > int('z'))) { return false; }
    if(((int)uc[2] < (int)'a') || ((int)uc[2] > int('z'))) { return false; }
    return true;
}

static bool is_uc_ignore_fake(std::string uc)
{
    if(uc.size() != 7){ return false; }
    if(((int)uc[0] < (int)'C') || ((int)uc[0] > int('K'))) { return false; }
    if(((int)uc[1] < (int)'a') || ((int)uc[1] > int('z'))) { return false; }
    if(((int)uc[2] < (int)'a') || ((int)uc[2] > int('z'))) { return false; }
    // 将 fake uc 视作非规范化的数据
    if(uc[0] == 'F' && uc[1] == 'u' && uc[2] == 'c') { return false; }
    
    return true;
}

std::map<std::string, bool> SaturnDatabaseSQL::check_uc_by_mysql(std::vector<std::string> uc_vector)
{
    MYSQL *conn;
    MYSQL_RES *res;  
    conn = mysql_init(NULL);  
    std::map<std::string, bool> is_uc_map;

    // connect
    if (!mysql_real_connect(conn, SaturnDatabaseSQL::host.c_str(), SaturnDatabaseSQL::user.c_str(), SaturnDatabaseSQL::pwd.c_str(), SaturnDatabaseSQL::db.c_str(),0, NULL, 0))
    {
        std::cout << "saturndatabase sql connect error" << std::endl;
        SaturnDatabaseSQL::print_sql_info();
        throw "saturndatabase sql connect error";
    }
    else
    {
        std::cout << "saturndatabase sql connect success" << std::endl;
    }     
    
    // search
    for(int i=0; i<uc_vector.size(); i++)
    {
        std::string uc = uc_vector[i];
        std::string serarch_str = "SELECT md5 FROM Md5ToUc WHERE uc = '" + uc + "';";
        mysql_query(conn, serarch_str.c_str());       
        res = mysql_store_result(conn);      
        if(!res)                                
        {
            std::cout << "saturndatabase sql query error" << std::endl;
            throw "saturndatabase sql query error";
        }   
        int cols = mysql_num_fields(res);  
        int rows = mysql_num_rows(res);            

        if(rows > 0)
        {
            is_uc_map[uc] = true;
        }
        else
        {
            is_uc_map[uc] = false;
        }
        mysql_free_result(res); 
    }

    mysql_close(conn);  
    return is_uc_map;
}

SaturnDatabaseSQL::SaturnDatabaseSQL(std::string host, int port, std::string user, std::string pwd, std::string db)
{
    SaturnDatabaseSQL::host = host;
    SaturnDatabaseSQL::port = port;
    SaturnDatabaseSQL::user = user;
    SaturnDatabaseSQL::pwd = pwd;
    SaturnDatabaseSQL::db = db;
}

void SaturnDatabaseSQL::rename_img_dir(std::string img_dir, int buffer_img_size, bool check_uc)
{

    if(img_dir == "./")
    {
        std::cout << ERROR_COLOR << "因为未知的原因现在还不支持在当前路径下运行 " << img_dir << STOP_COLOR << std::endl;
        return;
    }


    // fixme 本身是 uc 名字的就不要操作了，直接拷贝或者复制就行，有 uc 格式的看在不在数据库就完事了

    std::set<std::string> suffixs {".jpg", ".JPG", ".png", ".PNG", ".jpeg"};
    std::vector<std::string> img_path_vector = get_all_file_path_recursive(img_dir, suffixs);
    std::vector<std::string> md5_vector;
    std::string md5_str;
    std::map<std::string, std::string> img_path_md5_map;

    tqdm bar;
    int N = img_path_vector.size();

    for(int i=0; i<img_path_vector.size(); i++)
    {
        std::string img_name = get_file_name(img_path_vector[i]);

        if((! is_uc_ignore_fake(img_name)) || (check_uc == true))
        {
            md5_str = get_file_md5(img_path_vector[i]);
            md5_vector.push_back(md5_str);
            // std::cout << i << ", " << "get md5 : " << img_path_vector[i] << ", " << md5_str << std::endl;
            img_path_md5_map[img_path_vector[i]] = md5_str;
            bar.progress(i, N);
        }
    }
    bar.finish();

    // get uc by md5
    std::map<std::string, std::string> md5_uc_map = SaturnDatabaseSQL::get_md5_uc_map_from_md5_vector(md5_vector);

    // rename
    std::string img_path;
    std::string img_folder;
    std::string img_suffix;
    std::string new_img_path;
    int rename_count = 0;
    auto iter = img_path_md5_map.begin();
    while(iter != img_path_md5_map.end())
    {
        img_path = iter->first;
        string uc = md5_uc_map[iter->second];

        if(uc != "")
        {
            img_folder = get_file_folder(img_path);
            img_suffix = get_file_suffix(img_path);
            new_img_path = img_folder + "/" + uc + img_suffix;
            // rename 
            rename(img_path.c_str(), new_img_path.c_str());
            rename_count ++;
        }
        iter ++;
    }

    std::cout << HIGHTLIGHT_COLOR << "rename count : " << rename_count << STOP_COLOR << std::endl;

}

void SaturnDatabaseSQL::rename_img_xml_dir(std::string img_dir, std::string xml_dir, int buffer_img_size, bool check_uc)
{

    if((! is_dir(img_dir)) || (! is_dir(xml_dir)))
    {
        std::cout << ERROR_COLOR << "image dir or xml dir not exists : " << img_dir << STOP_COLOR << std::endl;
    }

    std::set<std::string> suffixs {".jpg", ".JPG", ".png", ".PNG", ".jpeg"};
    std::vector<std::string> img_path_vector = get_all_file_path(img_dir, suffixs);
    std::vector<std::string> md5_vector;
    std::string md5_str;
    std::map<std::string, std::string> img_path_md5_map;
    std::string img_path, xml_path, img_name, img_suffix;
    std::string new_img_path, new_xml_path;

    tqdm bar;
    int N = img_path_vector.size();

    // 
    for(int i=0; i<img_path_vector.size(); i++)
    {
        // todo 如果存在对应的 xml path
        img_name = get_file_name(img_path_vector[i]);
        xml_path = xml_dir + "/" + img_name + ".xml";
        if((! is_uc_ignore_fake(img_name)) || (check_uc == true))
        {
            if(is_file(xml_path))
            {
                md5_str = get_file_md5(img_path_vector[i]);
                md5_vector.push_back(md5_str);
                // std::cout << i << ", " << "get md5 : " << img_path_vector[i] << ", " << md5_str << std::endl;
                img_path_md5_map[img_path_vector[i]] = md5_str;
            }
            else
            {
                std::cout << HIGHTLIGHT_COLOR << "can't find xml_path : " << xml_path << STOP_COLOR << std::endl;
            }
            bar.progress(i, N);
        }
    }
    bar.finish();

    // get uc by md5
    std::map<std::string, std::string> md5_uc_map = SaturnDatabaseSQL::get_md5_uc_map_from_md5_vector(md5_vector);

    // rename
    int rename_count = 0; 
    auto iter = img_path_md5_map.begin();
    while(iter != img_path_md5_map.end())
    {
        string uc = md5_uc_map[iter->second];

        if(uc != "")
        {
            img_path = iter->first;
            img_name = get_file_name(img_path);
            xml_path = xml_dir + "/" + img_name + ".xml";
            img_suffix = get_file_suffix(img_path);
            new_img_path = img_dir + "/" + uc + img_suffix;
            new_xml_path = xml_dir + "/" + uc + ".xml";

            rename(img_path.c_str(), new_img_path.c_str());
            rename(xml_path.c_str(), new_xml_path.c_str());
            rename_count++;
        }
        iter ++;
    }
    
    std::cout << HIGHTLIGHT_COLOR << "rename count : " << rename_count << STOP_COLOR << std::endl;

}

void SaturnDatabaseSQL::rename_img_json_dir(std::string img_dir, std::string json_dir, int buffer_img_size, bool check_uc)
{

    if((! is_dir(img_dir)) || (! is_dir(json_dir)))
    {
        std::cout << ERROR_COLOR << "image dir or xml dir not exists : " << img_dir << STOP_COLOR << std::endl;
    }

    std::set<std::string> suffixs {".jpg", ".JPG", ".png", ".PNG", ".jpeg"};
    std::vector<std::string> img_path_vector = get_all_file_path(img_dir, suffixs);
    std::vector<std::string> md5_vector;
    std::string md5_str;
    std::map<std::string, std::string> img_path_md5_map;
    std::string img_path, xml_path, img_name, img_suffix;
    std::string new_img_path, new_xml_path;

    tqdm bar;
    int N = img_path_vector.size();

    // 
    for(int i=0; i<img_path_vector.size(); i++)
    {
        // todo 如果存在对应的 xml path
        img_name = get_file_name(img_path_vector[i]);
        xml_path = json_dir + "/" + img_name + ".json";
        if((! is_uc_ignore_fake(img_name)) || (check_uc == true))
        {
            if(is_file(xml_path))
            {
                md5_str = get_file_md5(img_path_vector[i]);
                md5_vector.push_back(md5_str);
                std::cout << i << ", " << "get md5 : " << img_path_vector[i] << ", " << md5_str << std::endl;
                img_path_md5_map[img_path_vector[i]] = md5_str;
            }
            else
            {
                std::cout << "can't find xml_path : " << xml_path << std::endl;
            }
        }
        bar.progress(i, N);
    }
    bar.finish();

    // get uc by md5
    std::map<std::string, std::string> md5_uc_map = SaturnDatabaseSQL::get_md5_uc_map_from_md5_vector(md5_vector);

    // rename
    int rename_count = 0; 
    auto iter = img_path_md5_map.begin();
    while(iter != img_path_md5_map.end())
    {
        string uc = md5_uc_map[iter->second];

        if(uc != "")
        {
            img_path = iter->first;
            img_name = get_file_name(img_path);
            xml_path = json_dir + "/" + img_name + ".json";
            img_suffix = get_file_suffix(img_path);
            new_img_path = img_dir + "/" + uc + img_suffix;
            new_xml_path = json_dir + "/" + uc + ".json";

            rename(img_path.c_str(), new_img_path.c_str());
            rename(xml_path.c_str(), new_xml_path.c_str());
            rename_count++;
        }

        // std::cout << img_path << " : " << new_img_path << std::endl;
        iter ++;
    }
    std::cout << HIGHTLIGHT_COLOR << "rename count : " << rename_count << STOP_COLOR << std::endl;

}

std::map<std::string, std::string> SaturnDatabaseSQL::get_md5_uc_map_from_md5_vector(std::vector<std::string> md5_vector)
{
    MYSQL *conn;
    MYSQL_RES *res;  
    conn = mysql_init(NULL);  
    std::map<std::string, std::string> md5_uc_map;

    // connect
    if (!mysql_real_connect(conn, SaturnDatabaseSQL::host.c_str(), SaturnDatabaseSQL::user.c_str(), SaturnDatabaseSQL::pwd.c_str(), SaturnDatabaseSQL::db.c_str(),0, NULL, 0))
    {
        std::cout << "saturndatabase sql connect error" << std::endl;
        SaturnDatabaseSQL::print_sql_info();
        throw "saturndatabase sql connect error";
    }     
    // search
    int no_uc_index = 0;
    std::string serarch_str;
    for(int i=0; i<md5_vector.size(); i++)
    {
        serarch_str = "SELECT uc FROM Md5ToUc WHERE md5 = '" + md5_vector[i] + "';";
        mysql_query(conn, serarch_str.c_str());       
        res = mysql_store_result(conn);      
        if(!res)                                
        {
            std::cout << "saturndatabase sql query error" << std::endl;
            throw "saturndatabase sql query error";
        }
        int rows = mysql_num_rows(res);               
        int cols = mysql_num_fields(res);  

        // 结果只有一行数据
        if(rows > 0)
        {
            MYSQL_ROW row = mysql_fetch_row(res); 
            std::string uc = row[0];
            md5_uc_map[md5_vector[i]] = uc;
        }
        else
        {
            no_uc_index += 1;
            // std::cout << no_uc_index << ", " << "can't find uc by md5 : " << md5_vector[i] << std::endl;
        }
        mysql_free_result(res); 
    }
    mysql_close(conn);  
    return md5_uc_map;
}

std::map<std::string, std::string> SaturnDatabaseSQL::get_uc_md5_map_from_uc_vector(std::vector<std::string> uc_vector)
{
    MYSQL *conn;
    MYSQL_RES *res;  
    conn = mysql_init(NULL);  
    std::map<std::string, std::string> uc_md5_map;

    // connect
    if (!mysql_real_connect(conn, SaturnDatabaseSQL::host.c_str(), SaturnDatabaseSQL::user.c_str(), SaturnDatabaseSQL::pwd.c_str(), SaturnDatabaseSQL::db.c_str(),0, NULL, 0))
    {
        std::cout << "saturndatabase sql connect error" << std::endl;
        SaturnDatabaseSQL::print_sql_info();
        throw "saturndatabase sql connect error";
    }     
    // search
    std::string serarch_str;
    for(int i=0; i<uc_vector.size(); i++)
    {
        serarch_str = "SELECT uc FROM Md5ToUc WHERE uc = '" + uc_vector[i] + "';";
        mysql_query(conn, serarch_str.c_str());       
        res = mysql_store_result(conn);      
        if(!res)                                
        {
            std::cout << "saturndatabase sql query error" << std::endl;
            throw "saturndatabase sql query error";
        }
        int rows = mysql_num_rows(res);               
        int cols = mysql_num_fields(res);  

        // 结果只有一行数据
        if(rows > 0)
        {
            MYSQL_ROW row = mysql_fetch_row(res); 
            std::string md5 = row[0];
            std::cout << md5 << std::endl;
            uc_md5_map[uc_vector[i]] = md5;
        }
        else
        {
            std::cout << "can't find md5 by uc : " << uc_vector[i] << std::endl;
        }
        mysql_free_result(res); 
    }
    mysql_close(conn);  
    return uc_md5_map;
}

void SaturnDatabaseSQL::print_sql_info()
{
        std::cout << "sql_host      : " << SaturnDatabaseSQL::host << std::endl;
        std::cout << "sql_port      : " << SaturnDatabaseSQL::port << std::endl;
        std::cout << "sql_user      : " << SaturnDatabaseSQL::user << std::endl;
        std::cout << "sql_pwd       : " << SaturnDatabaseSQL::pwd << std::endl;
        std::cout << "sql_db        : " << SaturnDatabaseSQL::db << std::endl;
}

