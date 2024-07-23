#ifndef _DETERES_HPP_
#define _DETERES_HPP_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>


namespace jotools
{

struct Point
{
    float x;
    float y;
};

struct Color
{
    int r;
    int g;
    int b;
};

class DeteObj
{
    public:
        //
        int x1;
        int x2;
        int y1;
        int y2;
        int id;
        std::string tag;
        float conf;
        // 
        void do_offset(int, int);
        void print_format();
        std::vector<int> get_rectangle();
        Point get_center_point();
        int get_area();
        std::string get_name_str();
        void load_from_name_str(std::string);
        void test();
        //
        bool operator==(const DeteObj other);
};

class DeteRes
{
    public:
        //
        DeteRes(std::string xml_path="", std::string img_path="");
        // ~DeteRes();
        // variable
        int height;
        int width;
        int depth;
        std::string folder;
        std::string file_name;
        std::string img_path;
        std::string xml_path;
        std::vector<DeteObj> alarms;
        // todo img_ndarry 需要 opencv 这个库
        cv::Mat img_ndarry;
        void parse_img_info(std::string img_path);
        // func
        void parse_json(std::string json_path);
        void save_to_xml(std::string xml_path);
        void save_to_yolo_txt(std::string txt_path, std::map<std::string, int>tag_map);
        void save_to_json(std::string json_path);
        void crop_dete_obj(std::string save_dir, bool split_by_tag=true, std::string save_name="");
        cv::Mat get_sub_img_by_dete_obj(DeteObj assign_dete_obj, bool RGB=true);
        cv::Mat get_img_array(bool RGB);
        void add_dete_obj(DeteObj dete_obj);
        void add_dete_obj(int x1, int y1, int x2, int y2, float conf, std::string tag);
        void draw_dete_res(std::string save_path, std::map<std::string, Color> color_dirt);
        void save_to_assign_range(std::string tag, std::string save_img_dir, std::string save_label_dir, std::map<std::string, int>tag_map, float iou_1=0.85, std::string mode="txt");

        // 根据置信度对 dete_obj 进行排序
        void sort_by_conf(bool reverse=false);
        
        // 内容进行偏移
        void offset(int x, int y);

        // 结果进行 nms 处理
        void do_nms(float threshold, bool ignore_tag);
        
        void do_nms_center_point(bool ignore_tag);
        void filter_by_area(float area_th, std::string mode, bool update);
        void filter_by_tags(std::vector<std::string> tags);
        void filter_by_conf();
        void filter_by_mask();
        void filter_by_dete_res();
        void filter_by_topn();
        void do_augment();
        void del_dete_obj(DeteObj dete_obj);
        bool has_tag(std::string tag);
        bool has_dete_obj(DeteObj dete_obj);
        std::map<std::string, int> count_tags();
        int size();
        DeteRes update(DeteRes other);
        //
        void print_format();
        //
        bool operator+(const DeteRes other);
        DeteObj& operator[](const int i);
        bool parse_xml_info(std::string xml_path);

    private:

};

}

#endif