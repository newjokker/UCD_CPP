
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <cstdlib>
#include "../include/deteRes.hpp"
#include "../include/pystring.h"

namespace jotools
{

const double eps = 1e-8; // 用于判断浮点数是否相等的小数

void DeteObj::do_offset(int offset_x, int offset_y)
{
    DeteObj::x1 += offset_x;
    DeteObj::x2 += offset_x;
    DeteObj::y1 += offset_y;
    DeteObj::y2 += offset_y;
}

void DeteObj::print_format()
{
    std::cout << DeteObj::x1 << ", " << DeteObj::y1 << ", " << DeteObj::x2 << ", " << DeteObj::y2 << ", " << DeteObj::conf << ", " << DeteObj::tag.c_str() << std::endl;
}

std::vector<int> DeteObj::get_rectangle()
{
    std::vector<int> rect;
    rect.push_back(DeteObj::x1); 
    rect.push_back(DeteObj::y1); 
    rect.push_back(DeteObj::x2); 
    rect.push_back(DeteObj::y2);
    return rect; 
}

Point DeteObj::get_center_point()
{
    Point center_point;
    center_point.x = (float)((DeteObj::x2 - DeteObj::x1)/2.0);
    center_point.y = (float)((DeteObj::y2 - DeteObj::y1)/2.0);
    return center_point;
}

int DeteObj::get_area()
{
    return (DeteObj::x2 - DeteObj::x1) * (DeteObj::y2 - DeteObj::y1);
}

std::string DeteObj::get_name_str()
{
    std::ostringstream oss;
    // 最后一个是 id 是历史遗留下来的问题，设置为默认值 -1 即可
    oss << "[" << DeteObj::x1 << "," << DeteObj::y1 << "," << DeteObj::x2 << "," << DeteObj::y2 << ","  << "'" << DeteObj::tag << "'" << "," << DeteObj::conf << "," << "-1" << "]";
    return oss.str();
}

void DeteObj::load_from_name_str(std::string name_str)
{
    std::string loc_str_pure = pystring::slice(name_str, 1, -1);
    std::vector<std::string> obj_info = pystring::split(loc_str_pure, ",");
    // 
    DeteObj::x1 = std::stoi(obj_info[0]);
    DeteObj::y1 = std::stoi(obj_info[1]);
    DeteObj::x2 = std::stoi(obj_info[2]);
    DeteObj::y2 = std::stoi(obj_info[3]);
    DeteObj::conf = std::stof(obj_info[5]);
    DeteObj::tag = pystring::slice(obj_info[4], 1, -1) ;
}

bool DeteObj::operator==(const DeteObj other)
{
    if(DeteObj::x1 != other.x1){return false;}
    if(DeteObj::x2 != other.x2){return false;}
    if(DeteObj::y1 != other.y1){return false;}
    if(DeteObj::y2 != other.y2){return false;}
    if(DeteObj::tag != other.tag){return false;}
    return true;
}


}