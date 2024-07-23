#ifndef _REDISBOOK_HPP
#define _REDISBOOK_HPP

#include <iostream>
#include <vector>
#include <hiredis/hiredis.h> 
#include <map>
#include <set>

class Joke 
{
    public:
        std::string id;
        std::string content;
        std::set<std::string > tags;
        std::vector<std::string>command;    // 评论信息，评论时间-+-评论id-+-评论内容
        
        // 打印笑话
        void print_info();

        int add_tag(std::string tag);

        int add_command(std::string command, std::string writer);

};

class RedisBook
{
    
    public:

        // 客户端
        redisContext *client;
        std::string name    = "";
        std::string host    = "192.168.3.221";
        int port            = 6379;

        // 菜单
        std::vector<std::string> menu;

        // 构造函数
        RedisBook(std::string host, int port, std::string name="anonymity");

        // 连接 redis
        int connect();

        // 关闭链接
        int close();

        // 获取菜单信息
        std::map<std::string, std::string> get_menu_info(bool print_info=true);

        // 插入主菜单，显示插入的 python 代码，不直接插入
        int insert_menu_info();

        // 主菜单循环
        int menu_loop(std::string book_name);

        // txkj 文化学习
        int learn_tx_cluture();

        // 加载指定 id 的笑话
        int load_assign_joke(std::string joker_id, Joke *each_joke);

        // 更新笑话的信息
        int update_assign_joke(std::string joke_id, Joke *each_joke);

        // 开始笑话
        int tell_joke();
        
        // 获取所有笑话的 id
        std::vector<std::string> get_joke_id_vector();

        // 打印 docker 相关的信息
        int print_docker_info();

        // 打印 pdb 使用的相关信息
        int print_pdb_info();

        // 打印官方通告
        int print_official_info();

        // 打印常见的 CMD 命令
        int print_cmd_info();

};


class TodoList
{
    public:

        std::string name;
        std::string book_name;
        std::string host;
        int port;

        TodoList(std::string host, int port, std::string name="");

        // 获取指定日期的 todo 信息
        std::vector<std::string> get_todo_info(std::string date);

        // 打印 todo 信息
        int print_todo_info(std::string assign_date);

        // 打印指定日期到前面一周的 todo 信息
        int print_todo_info_assign_date_range(std::string assign_date, int data_range=7);

        // 删除 todo 信息
        int delete_todo_info(std::string assign_date, int assign_index);

        // 增加 todo 信息
        int add_todo_info(std::string assign_date, std::string todo_info);

        // 更新 todo 信息
        int update_todo_info(std::string assign_date, std::vector<std::string> todo_vector);

        // 获得当前的日期
        std::string get_date(std::string assign_date="", int offset_day=0);

        // 验证输入的日期是否合法
        bool is_valid_date(std::string assign_date);

        // 完成某条信息
        int finish_todo(std::string assign_date, int index);

        // 恢复某条记录为 todo
        int undo_todo(std::string assign_date, int index);

};



#endif