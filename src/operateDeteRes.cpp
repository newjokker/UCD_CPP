

#include <iostream>
#include <vector>
#include <map>
#include "../include/deteRes.hpp"
#include "../include/fileOperateUtil.hpp"
#include "../include/pystring.h"
#include "../include/operateDeteRes.hpp"
#include <stdio.h>
#include <sys/stat.h> 
// #include "./parse_img_exif_info.hpp"
#include "include/easyexif.h"
#include "include/imageinfo.hpp"
#include "include/md5.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>
#include <algorithm>
#include "ucDatasetUtil.hpp"
#include "include/tqdm.h"
#include "include/ucd_tools.hpp"

#define ERROR_COLOR         "\x1b[1;31m"    // 红色
#define HIGHTLIGHT_COLOR    "\033[1;35m"    // 品红
#define WARNNING_COLOR      "\033[1;33m"    // 橙色
#define STOP_COLOR          "\033[0m"       // 复原



namespace jotools
{


std::map<std::string, int> count_tags(std::string floder_path)
{
    std::map<std::string, int> count_res;
    std::vector<std::string> file_names = get_all_file_path_recursive(floder_path);
    std::set<std::string> suffixs;
    suffixs.insert(".xml");
    std::vector<std::string> file_name_xml = filter_by_suffix(file_names, suffixs);

    tqdm bar;
    int N = file_name_xml.size();

    // 
    for(int i; i<file_name_xml.size(); i++)
    {
        DeteRes *dete_res = new DeteRes(file_name_xml[i]);
        std::map<std::string, int> each_res = dete_res->count_tags();
        std::map<std::string, int>::iterator iter;

        iter = each_res.begin();
        while(iter != each_res.end()) 
        {
            if(count_res.count(iter->first) == 0)
            {
                count_res[iter->first] = iter->second;
            }
            else
            {
                count_res[iter->first] += iter->second;
            }
            iter++;
        }
        bar.progress(i, N);
        delete dete_res;
    }
    bar.finish();
    // print res
    std::cout << "------------------------------------------" << std::endl;
    std::cout << std::left << std::setfill(' ') << std::setw(25) << "tags" << " :    " << "count" << std::left << std::setfill(' ') << std::setw(25) << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    int tags_numb = 0; 
    std::map<std::string, int>::iterator iter;
    iter = count_res.begin();
    while(iter != count_res.end()) 
    {
        std::cout << std::left << std::setfill(' ') << std::setw(25) << iter->first << " :    " << iter->second << std::left << std::setfill(' ') << std::setw(25) << std::endl;
        tags_numb += iter->second;
        iter++;
    }
    // extra info
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "number of xml has obj     :    " << file_name_xml.size() << std::endl;
    std::cout << "number of tag             :    " << tags_numb << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    return count_res;
}

void cut_small_img(std::string img_dir, std::string xml_dir, std::string save_dir, bool split_by_tag)
{
    std::vector<std::string> file_path_list = get_all_file_path(img_dir);
    std::vector<std::string> img_path_list;
    std::set<std::string> suffixs {".jpg", ".JPG", ".png", ".PNG"};
    img_path_list = filter_by_suffix(file_path_list, suffixs);

    for(int i=0; i<file_path_list.size(); i++)
    {
        std::cout << file_path_list[i] << std::endl;
    }

    std::cout << "here" << std::endl;
    std::cout << file_path_list.size() << std::endl;
    std::cout << img_path_list.size() << std::endl;

    for(int i=0; i<img_path_list.size();i++)
    {
        std::string each_xml_path = xml_dir + "/" + get_file_name(img_path_list[i]) + ".xml";
        
        std::cout << each_xml_path << std::endl;
        
        if (is_file(each_xml_path))
        {
            std::cout << i << " : " << each_xml_path << " , " << img_path_list[i] << std::endl;
            DeteRes a(each_xml_path, img_path_list[i]);
            a.crop_dete_obj(save_dir, split_by_tag, "");
        }
    }
}

void get_ucd_from_crop_img(std::string crop_dir, std::string save_path, bool origin_tag)
{

    std::map<std::string, std::vector<std::string>> xml_info_map;
    std::vector<std::string> folder_path_list = get_all_folder_path(crop_dir);
    DeteObj obj;

    tqdm bar;
    int N = folder_path_list.size();
    for(int i=0; i<folder_path_list.size(); i++)
    {
        std::string folder_name = get_folder_name(folder_path_list[i]);
        std::vector<std::string> file_path_vector = get_all_file_path(folder_path_list[i]);
        std::set<std::string> suffixs {".jpg", ".png", ".JPG", ".PNG"};
        std::vector<std::string> img_path_vector = filter_by_suffix(file_path_vector, suffixs);

        for(int j=0; j<img_path_vector.size(); j++)
        {
            std::string file_name = get_file_name(img_path_vector[j]);
            std::vector<std::string> loc_str_list = pystring::split(file_name, "-+-");
            std::string loc_str = loc_str_list[loc_str_list.size()-1];

            if(! origin_tag)
            {
                // 替换 loc_str 中的 tag 信息
                obj.load_from_name_str(loc_str);
                obj.tag = folder_name;
                loc_str = obj.get_name_str();
            }

            std::string uc = file_name.substr(0, pystring::rfind(file_name, "-+-"));
            xml_info_map[uc].push_back(loc_str);
        }
        bar.progress(i, N);
    }
    bar.finish();

    UCDataset *ucd = new UCDataset(save_path);

    // get xml
    tqdm bar2;
    int N2 = xml_info_map.size();
    int i2 = 0;
    auto iter = xml_info_map.begin();
    while (iter != xml_info_map.end())
    {
        DeteRes* dete_res = new DeteRes();
        for(int i=0; i<iter->second.size(); i++)
        {
            DeteObj* dete_obj = new DeteObj();
            dete_obj->load_from_name_str(iter->second[i]);
            // 这边修改名字

            dete_res->add_dete_obj(*dete_obj);
            delete dete_obj;
        }
        ucd->add_dete_res_info(iter->first, *dete_res);
        delete dete_res;
        iter++;
        i2++;
        bar2.progress(i2, N2);
    }
    bar2.finish();

    // save ucd
    ucd->add_time       = getPythonStyleTimestamp();
    ucd->update_time    = getPythonStyleTimestamp();
    ucd->save_to_ucd(save_path);
    delete ucd;
}

void get_xml_from_crop_img(std::string crop_dir, std::string save_xml_dir)
{
    std::map<std::string, std::vector<std::string>> xml_info_map;
    std::vector<std::string> folder_path_list = get_all_folder_path(crop_dir);
    DeteObj obj;

    for(int i=0; i<folder_path_list.size(); i++)
    {
        std::vector<std::string> file_path_vector = get_all_file_path(folder_path_list[i]);
        std::set<std::string> suffixs {".jpg", ".png", ".JPG", ".PNG"};
        std::vector<std::string> img_path_vector = filter_by_suffix(file_path_vector, suffixs);

        std::string folder_name = get_folder_name(folder_path_list[i]);
        for(int j=0; j<img_path_vector.size(); j++)
        {
            std::string file_name = get_file_name(img_path_vector[j]);
            std::vector<std::string> loc_str_list = pystring::split(file_name, "-+-");
            std::string loc_str = loc_str_list[loc_str_list.size()-1];

            // 替换 loc_str 中的 tag 信息
            obj.load_from_name_str(loc_str);
            obj.tag = folder_name;
            loc_str = obj.get_name_str();

            std::string uc = file_name.substr(0, pystring::rfind(file_name, "-+-"));
            xml_info_map[uc].push_back(loc_str);
            // std::cout << uc << ", " << folder_name << ", " << loc_str << std::endl;
        }
    }

    // get xml
    auto iter = xml_info_map.begin();
    while (iter != xml_info_map.end())
    {
        DeteRes* dete_res = new DeteRes();
        for(int i=0; i<iter->second.size(); i++)
        {
            DeteObj* dete_obj = new DeteObj();
            dete_obj->load_from_name_str(iter->second[i]);
            // 这边修改名字

            dete_res->add_dete_obj(*dete_obj);
            delete dete_obj;
            // std::cout << iter->first << " : " << iter->second.size() << std::endl;
        }
        std::string save_xml_path = save_xml_dir + "/" + iter->first + ".xml"; 
        dete_res->save_to_xml(save_xml_path);
        delete dete_res;
        iter++;
    }
}

void get_xml_from_crop_img_use_origin_tag(std::string crop_dir,  std::string save_xml_dir)
{
    std::set<std::string> suffixs {".jpg", ".png", ".JPG", ".PNG"};
    std::vector<std::string> img_path_vector = get_all_file_path_recursive(crop_dir, suffixs);
    std::map<std::string, std::vector<std::string>> xml_info_map;
    DeteObj obj;

    for(int i=0; i<img_path_vector.size(); i++)
    {
        std::string file_name = get_file_name(img_path_vector[i]);
        std::vector<std::string> loc_str_list = pystring::split(file_name, "-+-");
        std::string loc_str = loc_str_list[loc_str_list.size()-1];

        // // 替换 loc_str 中的 tag 信息
        // obj.load_from_name_str(loc_str);
        // loc_str = obj.get_name_str();

        std::string uc = file_name.substr(0, pystring::rfind(file_name, "-+-"));
        xml_info_map[uc].push_back(loc_str);
    }

    // get xml
    auto iter = xml_info_map.begin();
    while (iter != xml_info_map.end())
    {
        DeteRes* dete_res = new DeteRes();
        for(int i=0; i<iter->second.size(); i++)
        {
            DeteObj* dete_obj = new DeteObj();
            dete_obj->load_from_name_str(iter->second[i]);
            // 这边修改名字

            dete_res->add_dete_obj(*dete_obj);
            delete dete_obj;
            // std::cout << iter->first << " : " << iter->second.size() << std::endl;
        }
        std::string save_xml_path = save_xml_dir + "/" + iter->first + ".xml"; 
        dete_res->save_to_xml(save_xml_path);
        delete dete_res;
        iter++;
    }
}

void count_files(std::string folder_path, bool recursive)
    {
        std::map<std::string, int> file_count_map;
        std::vector<std::string> file_path_vector;
        if(recursive)
        {
            file_path_vector = get_all_file_path_recursive(folder_path);
        }
        else
        {
            file_path_vector = get_all_file_path(folder_path);
        }

        for(int i=0; i<file_path_vector.size(); i++)
        {

            std::string suffix = get_file_suffix(file_path_vector[i]);

            if(file_count_map.count(suffix) == 0)
            {
                file_count_map[suffix] = 1;
            }
            else
            {
                file_count_map[suffix] +=1;
            }
        }

        auto iter = file_count_map.begin();
        std::cout << "----------------------" << std::endl;
        std::cout << std::setw(10) << std::left << "suffix" << " : " << "count" << std::endl;
        std::cout << "----------------------" << std::endl;
        while(iter != file_count_map.end())
        {
            std::cout << std::setw(10) << std::left << iter->first << " : " << iter->second << std::endl;
            iter++;
        }
        std::cout << "----------------------" << std::endl;
    }

void xml_check(std::string xml_dir, std::string img_dir, int size_th, bool remove_error_path)
{

    // if folder exists
    if(! is_dir(xml_dir))
    {
        std::cout << "xml dir not exist, " << xml_dir << std::endl;
        throw "xml dir not exist";
    }

    if(! is_dir(img_dir))
    {
        std::cout << "img dir not exist, " << img_dir << std::endl;
        throw "img dir not exist";
    }

    std::vector<std::string> error_file_vector;

    // suffixs
    std::set<std::string> suffixs_xml {".xml"};
    std::set<std::string> suffixs_img {".jpg", ".png", ".JPG", ".PNG"};
    
    // xml
    std::vector<std::string> all_xml_vector = get_all_file_path(xml_dir, suffixs_xml);
    for(int i=0; i<all_xml_vector.size(); i++)
    {
        std::string xml_name = get_file_name(all_xml_vector[i]);
        std::string img_path = get_file_by_suffix_set(img_dir, xml_name, suffixs_img);
        if(img_path == "")
        {
            std::cout << "extra_xml: " << all_xml_vector[i] << std::endl;
            error_file_vector.push_back(all_xml_vector[i]);
        }
    }
    
    // img
    std::vector<std::string> all_img_vector = get_all_file_path(img_dir, suffixs_img);
    std::vector<std::string> img_vector;
    for(int i=0; i<all_img_vector.size(); i++)
    {
        std::string img_name = get_file_name(all_img_vector[i]);
        std::string xml_path = get_file_by_suffix_set(xml_dir, img_name, suffixs_xml);
        if(xml_path == "")
        {
            std::cout << "extra_img: " << all_img_vector[i] << std::endl;
            error_file_vector.push_back(all_img_vector[i]);
        }
        else
        {
            img_vector.push_back(all_img_vector[i]);
        }
    }

    // compare 
    for(int i=0; i<img_vector.size(); i++)
    {
        std::string xml_path = xml_dir + "/" + get_file_name(img_vector[i]) + ".xml";
        
        DeteRes* dete_res = new DeteRes(xml_path); 

        // compare height, width
        int height, width;

        FILE *file = fopen(img_vector[i].c_str(), "rb");
        auto imageInfo = getImageInfo<IIFileReader>(file);
        fclose(file);

        height = imageInfo.getHeight();
        width =  imageInfo.getWidth();

        if(dete_res->height != height)
        {
            std::cout << "error_size: " << xml_path << std::endl;
            std::cout << "error_size: " << img_vector[i] << std::endl;
            error_file_vector.push_back(xml_path);
            error_file_vector.push_back(img_vector[i]);
            continue;
        }
        
        if(dete_res->width != width)
        {
            std::cout << "error_size: " << xml_path << std::endl;
            std::cout << "error_size: " << img_vector[i] << std::endl;
            error_file_vector.push_back(xml_path);
            error_file_vector.push_back(img_vector[i]);
            continue;
        }

        // compare size_th
        for(int j=0; j<dete_res->size(); j++)
        {
            DeteObj dete_obj = dete_res->alarms[j];
            if((dete_obj.x2 < dete_obj.x1) || ((dete_obj.x2 - dete_obj.x1) < size_th) || (dete_obj.y2 < dete_obj.y1) || ((dete_obj.y2 - dete_obj.y1) < size_th))
            {
                std::cout << "error_size_th: " << xml_path << std::endl;
                std::cout << "error_size_th: " << img_vector[i] << std::endl;
                error_file_vector.push_back(xml_path);
                error_file_vector.push_back(img_vector[i]);
                continue;
            }
        }

        // delete
        if(remove_error_path)
        {
            for(int i=0; i<error_file_vector.size(); i++)
            {
                remove_file(error_file_vector[i]);
            }
        }

        delete dete_res;
    }
}

void rename_all_files_by_md5(std::string folder_path)
{
    std::vector<std::string> file_path_vector = get_all_file_path_recursive(folder_path);

    std::string md5_str;
    std::string new_file_path;
    for(int i=0; i<file_path_vector.size(); i++)
    {
        md5_str = get_file_md5(file_path_vector[i]);
        new_file_path = get_file_folder(file_path_vector[i]) + "/" + md5_str + get_file_suffix(file_path_vector[i]);
        // std::cout << "md5 : " << md5_str << std::endl;
        // std::cout << "region path : " << file_path_vector[i] << std::endl;
        // std::cout << "rename path : " << new_file_path << std::endl;
        rename(file_path_vector[i].c_str(), new_file_path.c_str());
    }
}

void rename_xml_img_by_md5(std::string xml_folder, std::string img_folder)
{

    // 在使用这个代码的时候先要运行 xml_check 确保 xml 和 img 是一一对应的
    // 找到 xml 文件夹下面的所有 xml
    // 找到 img 文件夹中的所有 img

    if(!is_dir(xml_folder))
    {
        std::cout << "error, xml dir not exists : " << xml_folder << std::endl;
        throw "error, xml dir not exists : " + xml_folder;
    }

    if(!is_dir(img_folder))
    {
        std::cout << "error, img dir not exists : " << img_folder << std::endl;
        throw "error, xml dir not exists : " + xml_folder;
    }

    std::set<std::string> suffix {".jpg", ".png", ".JPG", ".PNG"};
    std::vector<std::string> img_path_vector = get_all_file_path(img_folder, suffix);

    std::string each_xml_path;
    std::string each_img_path;
    std::string md5_str;
    std::string new_xml_path, new_img_path;
    for(int i=0; i<img_path_vector.size(); i++)
    {
        each_img_path = img_path_vector[i];
        each_xml_path = xml_folder + "/" + get_file_name(each_img_path) + ".xml";
        if(! is_file(each_xml_path))
        {
            std::cout << "error, xml_path not exists : " << each_xml_path;
            throw "error, xml_path not exists : " + each_xml_path;
        }
        md5_str = get_file_md5(each_img_path);
        new_img_path = img_folder + "/" + md5_str + get_file_suffix(each_img_path);
        new_xml_path = xml_folder + "/" + md5_str + ".xml";
        rename(each_img_path.c_str(), new_img_path.c_str());
        rename(each_xml_path.c_str(), new_xml_path.c_str());
    }


}

float rect_iou(int x1, int y1, int h1, int w1, int x2, int y2, int h2, int w2)
{
    // 计算两个矩形的 iou
    int endx = std::max(x1 + w1, x2 + w2);
    int startx = std::min(x1, x2);
    int width = w1 + w2 - (endx - startx);
    int endy = std::max(y1 + h1, y2 + h2);
    int starty = std::min(y1, y2);
    int height = h1 + h2 - (endy - starty);
    if (width > 0 && height > 0) {
        int area = width * height;
        int area1 = w1 * h1;
        int area2 = w2 * h2;
        float ratio = (float)area / (area1 + area2 - area);
        return ratio;
    } else {
        return 0.0;
    }
}

float dete_obj_iou(DeteObj a, DeteObj b)
{
    int x1 = a.x1;
    int y1 = a.y1;
    int h1 = a.y2 - a.y1;
    int w1 = a.x2 - a.x1;

    int x2 = b.x1;
    int y2 = b.y1;
    int h2 = b.y2 - b.y1;
    int w2 = b.x2 - b.x1;

    return rect_iou(x1, y1, h1, w1, x2, y2, h2, w2);
}


//


DeteAcc::DeteAcc()
{
    DeteAcc::iou = 0.5;
}

std::map<std::string, std::map<std::string, int> > DeteAcc::compare_customer_and_standard(DeteRes dete, DeteRes gt, std::string uc, UCDataset* compare_res_ucd)
{

    // sort by conf 
    dete.sort_by_conf();
    gt.sort_by_conf();

    DeteRes* compare_dete_res = new DeteRes();

    float iou_th = 0.5;
    std::map<int, bool> has_obj_map_dete;
    std::map<int, bool> has_obj_map_gt;
    std::map<int, bool> has_mis_map;                                    // 是否存在标签被标记为 mistake 
    std::map<std::string, std::map<std::string, int> > acc_res;
    acc_res["correct"]  = {};
    acc_res["miss"]     = {};
    acc_res["extra"]    = {};
    acc_res["mistake"]  = {};


    // 第一遍找到所有能匹配上的标签
    for(int i=0; i<dete.alarms.size(); i++)
    {
        has_obj_map_dete[i] = false;
    }

    for(int i=0; i<gt.alarms.size(); i++)
    {
        has_obj_map_gt[i] = false;
    }

    for(int i=0; i<gt.alarms.size(); i++)
    {
        DeteObj max_iou_obj;
        float max_iou           = -1;
        int max_iou_index       = -1;

        // 找到单个元素的最匹配标签
        for(int j=0; j<dete.alarms.size(); j++)
        {
            bool is_correct_tag     = false;                                        // 当前是相同的标签
            float each_iou          = dete_obj_iou(gt.alarms[i], dete.alarms[j]);

            if(dete.alarms[j].tag != gt.alarms[i].tag)
            {
                continue;;
            }
            else if((dete.alarms[j].tag == gt.alarms[i].tag) && (has_obj_map_dete[j] == false) && (each_iou > max_iou))
            {
                max_iou         = each_iou;
                max_iou_obj     = gt.alarms[i];
                max_iou_index   = j; 
            }
        }

        // 决定是否匹配
        if(max_iou > DeteAcc::iou)
        {
            has_obj_map_gt[i] = true;
            std::string correct_tag = gt.alarms[i].tag;
            has_obj_map_dete[max_iou_index] = true;
            DeteObj correct_dete_obj = dete.alarms[max_iou_index];
            correct_dete_obj.tag = "correct_" + correct_tag;
            compare_dete_res->add_dete_obj(correct_dete_obj);

            if(acc_res["correct"].count(correct_tag) == 0)
            {
                acc_res["correct"][correct_tag] = 1;
            }
            else
            {
                acc_res["correct"][correct_tag] += 1;
            }
        } 
    }

    // 第二遍找 mistake 数据
    for(int i=0; i<gt.alarms.size(); i++)
    {
        if(has_obj_map_gt[i] == true)
        {
            continue;
        }

        DeteObj max_iou_obj;
        float max_iou           = -1;
        int max_iou_index       = -1;

        // 找到单个元素的最匹配标签
        for(int j=0; j<dete.alarms.size(); j++)
        {
            if(has_obj_map_dete[j] == false)
            {
                bool is_correct_tag     = false;                                        
                float each_iou          = dete_obj_iou(gt.alarms[i], dete.alarms[j]);

                if(each_iou > max_iou)
                {
                    max_iou         = each_iou;
                    max_iou_obj     = gt.alarms[i];
                    max_iou_index   = j; 
                }
            }
        }

        // 决定是否匹配
        if(max_iou > DeteAcc::iou)
        {
            has_obj_map_gt[i] = true;
            std::string mistake_tag = gt.alarms[i].tag;
            has_obj_map_dete[max_iou_index] = true;
            DeteObj mistake_dete_obj = dete.alarms[max_iou_index];
            mistake_dete_obj.tag = "mistake_" + gt.alarms[i].tag + ":" + dete.alarms[max_iou_index].tag;
            compare_dete_res->add_dete_obj(mistake_dete_obj);

            if(acc_res["mistake"].count(mistake_tag) == 0)
            {
                acc_res["mistake"][mistake_tag] = 1;
            }
            else
            {
                acc_res["mistake"][mistake_tag] += 1;
            }
        }
        else
        {
            // miss 掉了数据
            std::string miss_tag = gt.alarms[i].tag;
            has_obj_map_gt[i] = true;
            DeteObj miss_dete_obj = gt.alarms[i];
            miss_dete_obj.tag = "miss_" +  miss_tag;
            compare_dete_res->add_dete_obj(miss_dete_obj);

            if(acc_res["miss"].count(miss_tag) == 0)
            {
                acc_res["miss"][miss_tag] = 1;
            }
            else
            {
                acc_res["miss"][miss_tag] += 1;
            }
        } 
    }

    // 第三遍找 extra 的数据
    for(int i=0; i<dete.alarms.size(); i++)
    {
        if(has_obj_map_dete[i] == false)
        {
            // miss 掉了数据
            std::string extra_tag = dete.alarms[i].tag;
            has_obj_map_dete[i] = true;
            DeteObj dete_dete_obj = dete.alarms[i];
            dete_dete_obj.tag = "extra_" + extra_tag;
            compare_dete_res->add_dete_obj(dete_dete_obj);

            if(acc_res["extra"].count(extra_tag) == 0)
            {
                acc_res["extra"][extra_tag] = 1;
            }
            else
            {
                acc_res["extra"][extra_tag] += 1;
            }
        }            
    }

    compare_res_ucd->add_dete_res_info(uc, *compare_dete_res);
    return acc_res;
}

std::map<std::string, std::map<std::string, int> > merge_compare_res(std::map<std::string, std::map<std::string, int> > b, std::map<std::string, std::map<std::string, int> > a)
{
    // 将两个检测结果进行合并
    auto iter_a = a.begin();
    while(iter_a != a.end())
    {
        std::map<std::string, int> each_res;
        each_res = iter_a->second;
        
        if(b.count(iter_a->first)==0)
        {
            b[iter_a->first] = {};
        }
        
        auto iter_a2 = each_res.begin();
        while(iter_a2 != each_res.end())
        {
            if(b[iter_a->first].count(iter_a2->first) == 0)
            {
                b[iter_a->first][iter_a2->first] = 0;
            }

            b[iter_a->first][iter_a2->first] += iter_a2->second;

            iter_a2++;
        }
        iter_a++;
    }
    return b;
}

std::map<std::string, std::vector<float> > DeteAcc::cal_acc_rec(UCDataset* ucd_a, UCDataset* ucd_b, std::string save_ucd_path, bool print)
{
    std::map<std::string, std::vector<float> > res;
    std::map<std::string, std::map<std::string, int> > compare_res;
    std::map<std::string, int> gt_count;
    UCDataset* compare_res_ucd = new UCDataset();

    // 找到 a b uc 的合集
    std::set<std::string> uc_set;
    for(int i=0; i<ucd_a->uc_list.size(); i++)
    {
        uc_set.insert(ucd_a->uc_list[i]);
    }

    for(int i=0; i<ucd_b->uc_list.size(); i++)
    {
        uc_set.insert(ucd_b->uc_list[i]);
    }

    if(print)
    {
        tqdm bar;
        int N = uc_set.size();

        int i =0;
        auto iter_uc = uc_set.begin();
        while(iter_uc != uc_set.end())
        {
            std::string uc = iter_uc->data();
            DeteRes *each_b = new DeteRes();
            DeteRes *each_a = new DeteRes();
            ucd_b->get_dete_res_with_assign_uc(each_b, uc);
            ucd_a->get_dete_res_with_assign_uc(each_a, uc);

            // add gt_count
            std::map<std::string, int> each_count_res = each_b->count_tags();
            auto iter_dete_res = each_count_res.begin();
            while(iter_dete_res != each_count_res.end())
            {
                if(gt_count.count(iter_dete_res->first) == 0)
                {
                    gt_count[iter_dete_res->first] = 0;
                }
                gt_count[iter_dete_res->first] += iter_dete_res->second;
                iter_dete_res++;
            }

            std::map<std::string, std::map<std::string, int> > each_compare_res = DeteAcc::compare_customer_and_standard(*each_a, *each_b, uc, compare_res_ucd);
            compare_res = merge_compare_res(compare_res, each_compare_res);
            bar.progress(i, N);
            iter_uc ++;
            i++;
        }
        bar.finish();
    }
    else
    {
        int i =0;
        auto iter_uc = uc_set.begin();
        while(iter_uc != uc_set.end())
        {
            std::string uc = iter_uc->data();
            DeteRes *each_b = new DeteRes();
            DeteRes *each_a = new DeteRes();            
            ucd_b->get_dete_res_with_assign_uc(each_b, uc);
            ucd_a->get_dete_res_with_assign_uc(each_a, uc);
            
            // add gt_count
            std::map<std::string, int> each_count_res = each_b->count_tags();
            auto iter_dete_res = each_count_res.begin();
            while(iter_dete_res != each_count_res.end())
            {
                if(gt_count.count(iter_dete_res->first) == 0)
                {
                    gt_count[iter_dete_res->first] = 0;
                }
                gt_count[iter_dete_res->first] += iter_dete_res->second;
                iter_dete_res++;
            }
            
            std::map<std::string, std::map<std::string, int> > each_compare_res = DeteAcc::compare_customer_and_standard(*each_a, *each_b, uc, compare_res_ucd);
            compare_res = merge_compare_res(compare_res, each_compare_res);            
            iter_uc ++;
            i++;
        }
    }

    // stastic
    std::set<std::string> all_tags      = ucd_a->get_tags();
    std::set<std::string> all_tags_b    = ucd_b->get_tags();
    all_tags.insert(all_tags_b.begin(), all_tags_b.end());
    
    if(print)
    {
        std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
        std::cout << "                                             STASTIC" << std::endl;
        std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
        std::cout << std::setw(20) << std::left << "TAG" << std::setw(10) << std::left << "CORRECT" << std::setw(10) << std::left << "MISS" << std::setw(10) << std::left << "EXTRA" 
                    << std::setw(10) << std::left << "MISTAKE" << std::setw(15) << std::left << "GT_COUNT" << std::setw(15) << std::left << "P(100%)" << std::setw(15) << std::left << "R(100%)" << std::endl;
        std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
    }

    int all_correct_num = 0;
    int all_miss_num    = 0;
    int all_extra_num   = 0;
    int all_mistake_num = 0;
    int all_gt_num      = 0;        // 所有标签的数目，之前计算正确的数目的方式是有点问题的
    float all_p         = 0;
    float all_r         = 0;

    auto iter = all_tags.begin();
    while(iter != all_tags.end())
    {
        std::string tag = iter->data();
        int correct_num = 0;
        int miss_num    = 0;
        int extra_num   = 0;
        int mistake_num = 0;
        int gt_num      = 0;
        float each_p    = 0;
        float each_r    = 0;

        if(gt_count.count(tag) == 0)
        {
            gt_num = 0;
        }
        else
        {
            gt_num = gt_count[tag];
        }
        all_gt_num += gt_num;

        if(compare_res["correct"].count(tag) > 0)
        {
            correct_num = compare_res["correct"][tag];
        } 

        if(compare_res["extra"].count(tag) > 0)
        {
            extra_num   = compare_res["extra"][tag];
        } 

        if(compare_res["miss"].count(tag) > 0)
        {
            miss_num    = compare_res["miss"][tag];
        } 

        if(compare_res["mistake"].count(tag) > 0)
        {
            mistake_num = compare_res["mistake"][tag];
        } 

        if((correct_num + mistake_num + extra_num) > 0)
        {
            each_p = 100 * (correct_num * 1.0) / (correct_num + mistake_num + extra_num);
        }

        if((correct_num > 0) && (gt_num > 0))
        {
            each_r = 100 * (correct_num * 1.0) / gt_num;
        } 

        all_correct_num += correct_num;
        all_extra_num   += extra_num;
        all_miss_num    += miss_num;
        all_mistake_num += mistake_num;
        

        res[tag].push_back(each_p);
        res[tag].push_back(each_r);

        if(print)
        {
            std::cout << std::setw(20) << std::left << tag << std::setw(10) << std::left << correct_num << std::setw(10) << std::left << miss_num << std::setw(10) << std::left << extra_num 
                    << std::setw(10) << std::left << mistake_num << std::setw(15) << std::left << gt_num << std::setw(15) << std::left << each_p << std::setw(15) << std::left << each_r << std::endl;
        }
        iter++;
    } 


    if((all_correct_num + all_mistake_num + all_extra_num) > 0)
    {
        all_p = 100 * (all_correct_num * 1.0) / (all_correct_num + all_mistake_num + all_extra_num);
    }

    if((all_correct_num + all_miss_num) > 0)
    {
        all_r = 100 * (all_correct_num * 1.0) / (all_gt_num);
    } 

    res["TOTAL"].push_back(all_p); 
    res["TOTAL"].push_back(all_r); 

    if(print)
    {
        std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
        std::cout << std::setw(20) << std::left << "TOTAL" << std::setw(10) << std::left << all_correct_num << std::setw(10) << std::left << all_miss_num << std::setw(10) << std::left << all_extra_num 
                << std::setw(10) << std::left << all_mistake_num << std::setw(15) << std::left << all_gt_num << std::setw(15) << std::left << all_p << std::setw(15) << std::left << all_r << std::endl;
        std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
        std::cout << "IOU : " << DeteAcc::iou << std::endl;
        std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
        std::cout << "" << std::endl;
        // std::cout << WARNNING_COLOR << "* 一横排全部统计为 0 是没有检测到也没有遗漏，是被误检了其他类型了，不是误检到当前类型，所以误检不统计" << STOP_COLOR << std::endl;
        // std::cout << WARNNING_COLOR << "* 这个函数还需要再测测，看看否有遗漏的问题" << STOP_COLOR << std::endl;
    }

    compare_res_ucd->size_info = ucd_b->size_info;
    if(save_ucd_path.size() > 0)
    {
        compare_res_ucd->save_to_ucd(save_ucd_path);
    }
    delete compare_res_ucd;
    return res;
}

static float vector_means(std::vector<float> res)
{
    if(res.size() == 0)
    {
        return 0;
    }

    float res_sum = 0;
    for(int i=0; i<res.size(); i++)
    {
        res_sum += res[i];
    }
    return res_sum / res.size();
}

void DeteAcc::cal_map(UCDataset* ucd_a, UCDataset* ucd_b, std::string save_path)
{
    std::cout << "---------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << std::setw(20) << std::left << "TAG         (P/R)";
    std::vector<std::string> ap_list;
    
    // 各个类别的 ap 的均值就是 map

    std::map< std::string, std::vector<float> > tag_ap_dict; 
    std::map< std::string, std::vector<float> > tag_ar_dict; 

    // start:stop:step, 5:100:10
    for(int i=20; i<100; i+=10)
    {
        ucd_a->filter_by_conf(i * 0.01, false);
        std::map<std::string, std::vector<float> > each_res = DeteAcc::cal_acc_rec(ucd_a, ucd_b, "", false);
        auto iter = each_res.begin();
        while(iter != each_res.end())
        {
            std::string tag = iter->first;
            tag_ap_dict[tag].push_back(iter->second[0]);
            tag_ar_dict[tag].push_back(iter->second[1]);
            iter++;
        }
        std::cout << std::setw(8) << std::left << i;
    }
    std::cout << std::setw(8) << std::left << "AP (100%)" << std::endl;
    std::cout << "---------------------------------------------------------------------------------------------------------------" << std::endl;

    auto iter_ap = tag_ap_dict.begin();
    while(iter_ap != tag_ap_dict.end())
    {
        // float each_ap = vector_means(iter_ap->second);
        // percion

        if(iter_ap->first != "TOTAL")
        {
            std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2);
            std::cout << std::setw(20) << std::left << iter_ap->first;
            for(int j=0; j<iter_ap->second.size(); j++)
            {
                std::cout << std::setw(8) << std::left << iter_ap->second[j];
            }
            std::cout << std::endl;
            // recall
            std::cout << std::setw(20) << std::left << " ";
            for(int x=0; x<tag_ar_dict[iter_ap->first].size(); x++)
            {
                std::cout << std::setw(8) << std::left << tag_ar_dict[iter_ap->first][x];
            }
            std::cout << "wait_for_cal" << std::endl;
            std::cout << "                    -------------------------------------------------------------------------------------------" << std::endl;
        }
        iter_ap++;
    }

    if(tag_ap_dict.count("TOTAL") > 0)
    {

        std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2);
        std::cout << HIGHTLIGHT_COLOR << std::setw(20) << std::left << "TOTAL";
        for(int j=0; j<tag_ap_dict["TOTAL"].size(); j++)
        {
            std::cout << HIGHTLIGHT_COLOR << std::setw(8) << std::left << tag_ap_dict["TOTAL"][j];
        }
        std::cout << std::endl;
        // recall
        std::cout << std::setw(20) << std::left << " ";
        for(int x=0; x<tag_ar_dict["TOTAL"].size(); x++)
        {
            std::cout << std::setw(8) << std::left << tag_ar_dict["TOTAL"][x];
        }
        std::cout << "wait_for_cal" << STOP_COLOR << std::endl;
    }

    std::cout << "---------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << std::setw(10) << std::left << "AVERAGE" << std::setw(10) << std::left << "P(100%)" << std::setw(10) << std::left << "R(100%)" 
              << std::setw(10) << std::left << "IOU : " + std::to_string(DeteAcc::iou) << std::endl;
    std::cout << "---------------------------------------------------------------------------------------------------------------" << std::endl;


    // 将打印的数据输出到 TXT 中去
    if(save_path != "")
    {
        std::ofstream OutFile(save_path); 
        auto iter_ap = tag_ap_dict.begin();
        while(iter_ap != tag_ap_dict.end())
        {
            OutFile << iter_ap->first << std::endl;

            // p
            OutFile << "p";
            for(int j=0; j<iter_ap->second.size(); j++)
            {
                OutFile << ","<< iter_ap->second[j];
            }
            OutFile << std::endl;
            
            // r
            OutFile << "r";
            for(int x=0; x<tag_ar_dict[iter_ap->first].size(); x++)
            {
                OutFile << "," << tag_ar_dict[iter_ap->first][x];
            }
            OutFile << std::endl;
        
            iter_ap++;
        }
        OutFile.close();
    }
}



}