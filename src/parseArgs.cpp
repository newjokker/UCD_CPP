
#ifndef _PARSE_ARG_
#define _PARSE_ARG_HPP_


#include <include/parseArgs.hpp>
#include <include/pystring.h>
#include <iostream>
#include <fstream>
#include <string>

// long，是一个字典 {str::str}
// short, 是一个 set{str}
// argc: 常数参数的个数
// argv: 常数参数vector


#define ERROR_COLOR         "\x1b[1;31m"    // 红色
#define HIGHTLIGHT_COLOR    "\033[1;35m"    // 品红
#define WARNNING_COLOR      "\033[1;33m"    // 橙色
#define STOP_COLOR          "\033[0m"       // 复原






bool parse_args(int argc, char** argv, std::vector<std::string> &new_argv, std::set<std::string> &short_args, std::map< std::string, std::string > &long_args)
{
    for(int i=0; i<argc; i++)
    {
        std::string each_argv = argv[i];
        if(pystring::startswith(each_argv, "--"))
        {
            if(i+1 < argc)
            {
                std::string next_argv = argv[i+1];
                long_args[each_argv.substr(2)] = next_argv;
            }
            else
            {
                std::cout << ERROR_COLOR << "error : need args after " << each_argv << STOP_COLOR << std::endl;
                throw "need args after " + each_argv;
                // return false;
            }
            i++;
        }
        else if(pystring::startswith(each_argv, "-"))
        {
            short_args.insert(each_argv.substr(1));
        }
        else
        {
            new_argv.push_back(argv[i]);
        }
    }
    return true;
}




#endif