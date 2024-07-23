#ifndef _MD5_HPP_
#define _MD5_HPP_

#include <iostream>
#include <openssl/md5.h>
#include <fstream>
#include <string>
#include <iomanip>
#include <string.h>

std::string get_file_md5(std::string lpcstrFilePath);

std::string get_str_md5(std::string assign_str);

#endif