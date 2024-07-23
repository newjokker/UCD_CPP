#ifndef _UCD_TOOLS_HPP
#define _UCD_TOOLS_HPP

#include <vector>

std::string get_ip_address();

int post_v2(std::string host, int port, std::string ucd_path, std::string url, std::vector<std::string> model_list, std::string batch_id, std::string save_dir, int assign_receive_port=-1);

int receive_v2();





#endif