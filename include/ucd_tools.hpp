#ifndef _UCD_TOOLS_HPP
#define _UCD_TOOLS_HPP

#include <vector>


std::string get_ip_address();

int post_v2(std::string host, int port, std::string ucd_path, std::string url, std::vector<std::string> model_list, std::string batch_id, std::string save_dir, int assign_receive_port=-1);

int receive_v2();

// 获取时间戳
double getPythonStyleTimestamp();

// 时间戳转为字符串
std::string timestampToString(double timestamp);

// 删除图片的元数据
bool remove_image_meta_data(std::string img_path, std::string save_path, int quality=90);



#endif