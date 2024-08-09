# -*- coding: utf-8  -*-
# -*- author: jokker -*-


import redis


r = redis.Redis(host='192.168.3.221', port=6379, db=0)


def set_menu():
    # 插入信息
    r.hset("menu", "cmd",       "常用命令")
    r.hset("menu", "joke",      "笑话")
    r.hset("menu", "customer",  "自定义")
    r.hset("menu", "txkj",      "公司文化")
    r.hset("menu", "svn",       "svn")
    r.hset("menu", "official",  "官方")
    r.hset("menu", "pingcode",  "pingcode")
    r.hset("menu", "document",  "文档")
    r.hset("menu", "docker",    "docker")
    r.hset("menu", "pdb",       "pdb")

def print_todo_list():
    # todo_list_jokker
    res = r.hgetall("todo_list_jokker")

    for each in res:
        print(each.decode("utf-8"), res[each].decode("utf-8"))



if __name__ == "__main__":

    # set_menu()

    print_todo_list()





