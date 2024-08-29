#ifndef _OPERATEDETERES_HPP_
#define _OPERATEDETERES_HPP_


#include <iostream>
#include <vector>
#include <map>
#include "./deteRes.hpp"
#include "./fileOperateUtil.hpp"
#include "include/ucDatasetUtil.hpp"

using namespace jotools;

// hpp 和 cpp 要使用同样的命名空间
namespace jotools
{
    // 统计标签个数
    std::map<std::string, int> count_tags(std::string floder_path);

    // 根据大图和 xml 截取小图
    void cut_small_img(std::string img_dir, std::string xml_dir, std::string save_dir, bool split_by_tag);
    
    // 从截图中反推出小图
    void get_ucd_from_crop_img(std::string crop_dir,  std::string save_path, bool origin_tag = false);
    void get_xml_from_crop_img(std::string crop_dir,  std::string save_xml_dir);
    void get_xml_from_crop_img_use_origin_tag(std::string crop_dir,  std::string save_xml_dir);

    // 查看文件分布
    void count_files(std::string folder_path, bool recursive=true);

    // 检查 xml 和 img 是否符合规范
    void xml_check(std::string xml_dir, std::string img_dir, int size_th, bool remove_error_file=true);

    // 将文件夹下面的所有文件使用 md5 进行重命名
    void rename_all_files_by_md5(std::string folder_path);

    // 将对应的 xml 和 img 重命名为 img 的 md5
    void rename_xml_img_by_md5(std::string xml_folder, std::string img_folder);



    class DeteAcc
    {
        public:


            // iou 阈值
            float iou;

            DeteAcc();

            // 对比一个 dete_res 结果
            std::map<std::string, std::map<std::string, int> > compare_customer_and_standard(DeteRes a, DeteRes b, std::string uc, UCDataset * c);

            // 检测结果计算 acc rec
            // void cal_acc_rec(std::string ucd_customer, std::string ucd_standard, std::string save_ucd_path="");
            std::map<std::string, std::vector<float> > cal_acc_rec(UCDataset* customer_ucd, UCDataset* standard_ucd, std::string save_ucd_path="", bool print=true);

            // 计算 MAP
            void cal_map(UCDataset* customer_ucd, UCDataset* standard_ucd, std::string save_path="");


    };


}

#endif