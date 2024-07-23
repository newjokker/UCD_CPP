
#ifndef _PARSEARGS_
#define _PARSEARGS_HPP_


#include <iostream>
#include <vector>
#include <map>
#include <set>




// long，       长参数，是一个字典 {str::str}
// short,       短参数，是一个 set{str}
// new_argv:    常数参数vector
bool parse_args(int argc, char** argv, std::vector<std::string> &new_argv, std::set<std::string> &short_args, std::map< std::string, std::string > &long_args);






#endif