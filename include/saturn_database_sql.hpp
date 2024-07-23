
#ifndef _SATURN_DATABASE_SQL_HPP_
#define _SATURN_DATABASE_SQL_HPP_

#include <string>
#include <iostream>
#include <vector>
#include <map>

using namespace std;


class SaturnDatabaseSQL
{
    public:
        std::string host;
        int port;
        std::string user;
        std::string pwd;
        std::string db;
        //
        SaturnDatabaseSQL(std::string host, int port, std::string user, std::string pwd, std::string db);

        // // 获取文件的 md5 值
        // std::string get_file_md5(std::string file_path);

        // rename image
        void rename_img_dir(std::string img_dir, int buffer_img_size=100, bool check_uc=true);

        // rename image xml
        void rename_img_xml_dir(std::string img_dir, std::string xml_dir, int buffer_img_size=100, bool check_uc=true);
        
        // rename image json
        void rename_img_json_dir(std::string img_dir, std::string json_dir, int buffer_img_size=100, bool check_uc=true);

        // 根据 md5 获取 uc
        std::map<std::string, std::string> get_md5_uc_map_from_md5_vector(std::vector<std::string> md5_vector);

        // 根据 uc 获取 md5
        std::map<std::string, std::string> get_uc_md5_map_from_uc_vector(std::vector<std::string> md5_vector);

        // 检查 sql 配置信息
        void print_sql_info();

        std::map<std::string, bool> check_uc_by_mysql(std::vector<std::string> uc_vector);
};


#endif