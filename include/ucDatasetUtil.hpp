

#ifndef _UCDATASETUTIL_HPP_
#define _UCDATASETUTIL_HPP_

#include <iostream>
#include <vector>
#include "include/lablelmeObj.hpp"
#include "include/deteRes.hpp"
using namespace jotools;


class UCDataset
{
    public:
        // uc 列表
        std::vector<std::string> uc_list;
        
        // 数据集名字
        std::string dataset_name;
        
        // 模型名字
        std::string model_name;
        
        // 模型版本
        std::string model_version;
        
        // 使用的标签
        std::vector<std::string> label_used;

        // labelme 中的对象 {uc: [PointObj, CricleObj, retangleObj ...]}
        std::map<std::string, std::vector<LabelmeObj*> > object_info;

        // size_info {uc: [width, height]}
        std::map<std::string, std::vector<int> > size_info;

        // 分卷个数
        int volume_count;

        // 分卷大小 
        int volume_size;

        // 分卷文件夹
        std::string volumn_dir;

        // 分卷名称
        std::string volume_name;

        // ucd 新建的时间
        double add_time;
        
        // ucd 更新的时间
        double update_time;

        // ucd 的描述
        std::string describe;
        
        // 构造函数
        UCDataset(std::string ucd_path="");
        
        // ~UCDataset();
        
        // 将 uc 转为 date 之间互相转换
        std::string uc_to_date(std::string uc);

        // 将 date 转为 uc
        std::string date_to_uc_head(std::string date);

        // 解析 ucd 数据
        void parse_ucd(bool parse_xml_info=false);
        
        // 装载 ucd 将读取 uci 的 volume 信息
        void load_uci(std::string uci_path);

        // 解析某一个分卷 volume，默认对 obj 进行过滤
        void parse_volume(int volumn_index, bool parse_szi=false, bool parse_obi=false, bool clear_obj=true);

        // 打印 json 数据
        void print_ucd_info();
        
        // 打印分卷信息
        void print_volume_info();

        // 打印指定 uc 对应的信息
        void print_assign_uc_info(std::string uc);
        
        // uc list 去重
        void unique();
        
        // 指定 uc 中是否存在某个对象
        bool has_obj(std::string uc, LabelmeObj *obj);
        
        // 查看 uc_list 中是否包含指定 uc
        bool has_uc(std::string uc);

        void add_obj(std::string uc, LabelmeObj *obj);

        // 删除 obj, 选择是否删除指针
        void delete_obj(std::string uc, LabelmeObj *obj, bool clear_obj=true); 

        // 对 uc 进行切片
        std::vector<std::string> uc_slice(int start, int end);
        
        // 统计标签个数
        std::map<std::string, std::map<std::string, int> > count_tags();
        
        // 分卷统计标签的个数
        std::map<std::string, std::map<std::string, int> > count_volume_tags();

        // 修改属性
        void change_attar(std::string attr_name, std::string attr_value);

        // 增量解析 xml 数据到 ucd 中
        void add_voc_xml_info(std::string uc, std::string voc_xml_path);

        // 增加解析 yolo_txt 到 ucd 中
        void add_yolo_txt_info(std::string uc, std::string txt_path, int width, int height);

        // 增量 添加 deteRes 数据到 ucd 中
        void add_dete_res_info(std::string uc, DeteRes dete_res);
        
        // 增量解析 labelme 的 json 数据
        void add_labelme_json_info(std::string uc, std::string labelme_json_path);

        // 增量解析 saturndatabase 的 json 数据
        void add_saturndatabase_json_info(std::string uc, std::string labelme_json_path);

        // 增量解析 ucd_info
        void add_ucd_info(std::string ucd_path);
        void add_ucd_info(UCDataset* ucd);

        // 保存为 ucd（斜框矩形如何进行保存）
        void save_to_ucd(std::string save_path);
        
        // 分卷保存为 ucd
        void save_to_huge_ucd(std::string save_dir, std::string ucd_name, int volume_index);

        // 保存一个 xml 文件
        void save_to_voc_xml_with_assign_uc(std::string save_path, std::string img_path, std::string assign_uc);

        // 保存一个 json 文件
        void save_to_labelme_json_with_assign_uc(std::string save_path, std::string img_path, std::string assign_uc);

        // save_to_yolo_train_data
        void save_to_yolo_train_txt_with_assign_uc(std::string save_path, std::string img_path, std::string assign_uc, std::vector<std::string> label_list);

        // 保存为一个 dete_res
        void get_dete_res_with_assign_uc(jotools::DeteRes* dete_res, std::string assign_uc);

        // 根据阈值进行过滤
        void filter_by_conf(float conf_th, bool clear_obj=true);

        // 根据标签进行过滤
        void filter_by_tags(std::set<std::string> tags, std::string mode="or", bool clear_obj=true);

        // 根据 uc 进行过滤
        void filter_by_uc_set(std::set<std::string> uc_set, bool clear_obj=true);

        // 根据面积进行过滤
        void filter_by_area(float area_th, bool clear_obj=true);

        // 对 ucd 做 nms
        void filter_by_nms(float nms_th, bool ignore_tag, bool clear_obj=true);

        // 对日期进行筛选
        void filter_by_date(std::vector<std::string> assign_date, bool clear_obj=true, std::string method="eq");

        // crop_dete_res
        void crop_dete_res_with_assign_uc(std::string uc, std::string img_path, std::string save_dir, bool is_split=true);

        // 保存指定范围，和指定范围内的目标
        void save_assign_range_with_assign_uc(std::string uc, std::string img_path, std::string save_img_dir, std::string save_label_dir, std::string assign_tag, std::vector<std::string> tag_list, float iou_th=0.5, std::string mode="txt");

        // 获得子序列
        void get_sub_ucd(int sub_count, bool is_random, std::string save_path);

        // 去除没有 object_info 的uc
        void update_uc_list_by_object_info(std::string save_path);

        // 随机将 ucd 分割为一定比例的几个部分
        void split(std::string ucd_part_a, std::string ucd_part_b, float ratio);

        // 将 ucd 按照日期分为几部分
        void split_by_date(std::string save_dir, std::string save_name="");

        // 将 ucd 按照标签分为几个部分
        void split_by_tags(std::string save_dir, std::string save_name="");

        // 将 ucd 按照置信度分为几部分
        void split_by_conf(std::string save_dir, std::string save_name="");
        void split_by_conf_change_tags(float step=0.1);

        // 从指定 ucd 中提取需要的内容
        void absorb(std::string meat_ucd, std::string save_path, std::string need_attr);

        // 将数据集随机平分为几部分
        void devide(std::string save_path, int devide_count);

        // exec command 中的子任务
        void command_ADD(std::vector<std::string> tokens);
        void command_DROP(std::vector<std::string> tokens);
        void command_SET(std::vector<std::string> tokens);

        // 执行 ucd command 脚本, 保存为新的 ucd 
        void exec(std::string command_path);

        // 抛弃标签
        void drop_tags(std::set<std::string> tags, bool clear_obj=true);

        // 更新标签
        void update_tags(std::map< std::string, std::string > tag_map);

        // 舍弃没有 obj 的 uc
        void drop_empty_uc();

        // 拿到不重复的标签
        std::set<std::string> get_tags();

        // 清空数据，释放内存
        void clear_obj_info();

        // 获取分卷路径, 0 是初始分卷
        std::string get_uci_path(int index);
        std::string get_obi_path(int index);
        std::string get_szi_path(int index);

        // 保存为 json 数据
        void to_json(std::string json_path);

        // 保存为 uci 数据
        void to_uci(std::string uci_path, int volume_size=30);

        // 获取信息数量, uc 数目 + size_info 数目 * 2 + obj 数目 * 4
        int get_info_count();

        // 检测框范围往外扩展
        int do_augment(float x1, float x2, float y1, float y2,  bool is_relative);

        // 删除多余的信息，没有出现在 uc_list 中，但是 size_info 和 object_info 中有结果
        int drop_extra_info();

        // 将 uc_list 转为 uc_set 方便对比
        std::set<std::string> get_uc_set();
        

    private:
        std::string json_path;
};


class UCDatasetUtil
{
    public:
        // 缓存文件夹，多个人多个项目使用的时候可以直接缓存图片
        std::string cache_dir;
        std::string cache_img_dir;
        std::string cache_xml_dir;
        std::string cache_crop_dir;
        std::string color_file;

        // 颜色映射字典
        std::map< std::string, Color > color_map;
        
        // ucd json_path
        std::string json_path;

        // 下载服务的主机地址
        std::string host;

        // 下载服务的下载路径
        std::string root_url;
        
        // 下载服务器的端口号
        int port;
        
        // 构造函数 
        UCDatasetUtil(std::string host, int port, std::string cache_dir="");
        
        // 从服务器下载对应的数据
        void save_img_xml_json(std::string save_dir, bool need_img=true, bool need_xml=true, int need_count=-1);
        
        // 下载图像 | xml | ucd
        void load_img(std::string save_dir, std::vector<std::string> uc_list);
        void load_img_with_assign_uc(std::string save_dir, std::string assign_uc);
        void load_xml(std::string save_dir, std::vector<std::string> uc_list);
        void load_ucd(std::string ucd_name, std::string save_dir);
        void load_ucd_app(std::string version, std::string save_path);
        
        // 从 ucd 中解析指定的 uc 的 labelme json 文件
        void parse_labelme_json(std::string img_dir, std::string save_dir, std::string ucd_path);

        // 从 ucd 中解析出指定 uc 的 voc_xml 文件 （img_dir 用于读取图片的长宽，要是不存在的话那么就设置为 -1）
        void parse_voc_xml(std::string img_dir, std::string save_dir, std::string ucd_path);
        void parse_volume_voc_xml(std::string img_dir, std::string save_dir, std::string ucd_path);

        // to yolo train_data
        void parse_yolo_train_data(std::string img_dir, std::string save_dir, std::string ucd_path, std::vector<std::string> label_list = {});
        
        // 查看库中的 ucd
        std::map< std::string, std::vector<std::string> > search_ucd(std::string assign_uc="", bool print_info=true, bool json_info=false, std::string name="");
        
        // 查看库中指定 ucd 的信息
        std::map< std::string, std::string > get_ucd_json_info(std::string ucd_json_name);

        // 下载库中的 ucd 到本地
        bool save_remote_ucd(std::string save_dir);

        // 是不是 ucd path （1）是不是合法文件 （2）是否为 .json 结尾的文件
        bool is_ucd_path(std::string ucd_path);
        
        // 是不是 uci path
        bool is_uci_path(std::string uci_path);

        // 统计每个标签出现在多少个图片中
        void count_uc_by_tags(std::string ucd_path);

        // 删除库中的 ucd
        void delete_ucd(std::string ucd_name);
        
        // 上传 ucd
        void upload_ucd(std::string ucd_path, std::string ucd_name="");
        
        // 将 img_dir 中的文件符合 UC 规范的组织为一个 ucd
        void get_ucd_from_img_dir(std::string img_dir, std::string ucd_path);
        
        // 将 xml 信息 保存在 ucd 中
        void get_ucd_from_xml_dir(std::string xml_dir, std::string ucd_path);
        
        // 将 crop_xml 信息保存到 ucd 中
        void get_ucd_from_crop_xml(std::string xml_dir, std::string ucd_path);

        // 从大量的 xml 中获取 ucd 数据集
        void get_ucd_from_huge_xml_dir(std::string xml_dir, std::string save_path, int volume_size=30);

        // 将 labelme json 信息保存到 ucd 中
        void get_ucd_from_json_dir(std::string json_dir, std::string ucd_path);
        
        // 从文件中获取 ucd，只解析文件名，不解析文件内容
        void get_ucd_from_file_dir(std::string file_dir, std::string ucd_path);

        // 从 yolo 训练的 txt 格式生成 json
        void get_ucd_from_yolo_txt_dir(std::string yolo_txt_dir, std::string ucd_path, std::string size_ucd="");

        // 从检测服务结果计算 ucd
        void get_ucd_from_dete_server(std::string  dete_server_dir, std::string ucd_path, std::string save_path);

        // 将多个 ucd 进行合并
        void merge_ucds(std::string save_path, std::vector<std::string> ucd_path_vector);
        
        // 求两个 ucd 的交集
        void interset_ucds(std::string save_path, std::string ucd_path_a, std::string ucd_path_b);

        // 查看两个 ucd 之间的差异
        void ucd_diff(std::string ucd_path_1, std::string ucd_path_2);
        
        // 去除在 ucd1 中 ucd2 中也存在的 uc
        void ucd_minus_obj(std::string save_path, std::string ucd_path_1, std::string ucd_path_2);
        
        void ucd_minus_uc(std::string save_path, std::string ucd_path_1, std::string ucd_path_2);

        // 统计标签的个数
        void count_ucd_tags(std::string ucd_path);
        
        // 统计分卷数据的个数
        void count_volume_tags(std::string uci_path);

        // 清空缓存
        void cache_clear();
        void cache_clear(std::string ucd_path, bool reversal=false);
        void cache_clear_assign_uc(std::string uc);

        // 打印文字
        void print_words(std::string name, int width=50, int height=50);
        
        // 裁切小图
        void cut_small_img(std::string ucd_path, std::string save_dir, bool is_split, bool no_cache=false, bool split_by_conf=false);

        // uc_check
        void uc_check(std::vector<std::string> file_vector);

        // uc_analysis
        void uc_analysis(std::string ucd_path);

        // conf_analysis
        void conf_analysis(std::string ucd_path, int seg_count);

        // area_analysis
        void area_analysis(std::string ucd_path, int seg_count);

        // 缓存文件清洗
        void cache_clean(std::string clean_folder);

        // 将文件赋予假的uc，FakeUC
        void set_fack_uc(std::string folder_path);

        // 对检测结果进行画图
        void draw_res(std::string ucd_path, std::string save_dir, std::vector<std::string> uc_list, bool cover_old_img=false);

        // 随机给不同的标签分配不同的颜色
        void get_random_color_map(std::string ucd_path);

        // 打印出文件夹中的 ucd 的信息
        void list_uci(std::string folder_path);

        // 删除指定的 uci 文件信息，包含所有的分卷
        void delete_uci(std::string uci_path);

        // 复制 uci
        void copy_uci(std::string src, std::string dst);

        // 移动 uci
        void move_uci(std::string src, std::string dst);

        // 将 json 保存的 ucd 换成 uci 保存的 ucd
        void json_to_uci(std::string json_path, std::string uci_path, int volume_size=30);

        // 将 uci 保存的 ucd 换乘 json 保存的 ucd 
        void uci_to_json(std::string uci_path, std::string json_path, int volume_size=30);

        // 对标签进行筛选
        void filter_by_tags_volume(std::set<std::string> tags, std::string uci_path, std::string save_dir, std::string save_name, int volume_size=30);

        // 从 uc_list 生成 ucd
        void get_ucd_from_uc_list(std::string save_path, std::vector<std::string> uc_list);

        // 查看本地路径下的 ucd 版本信息
        void get_ucd_version_info(std::string ucd_dir, std::string app_version);

        // 修复 size_info 信息
        void fix_size_info(std::string ucd_path, std::string save_path, bool no_cache, std::string size_ucd = "");

        // 找到缓存中 uc 图片
        std::string get_cache_uc_img_path(std::string uc);

        // 保存为 yolo 检测默认的训练格式
        void save_to_yolo_detect_train_data(std::string ucd_path, std::string save_dir, std::string tag_str, float ratio=0.8);

        // 保存为 yolo 分类的默认格式
        void save_to_yolo_classify_train_data(std::string ucd_path, std::string save_dir, std::string tag_str, float ratio=0.8);

        // 保存为 yolo 默认的训练格式 指定截取的范围
        void save_to_yolo_train_data_with_assign_range(std::string ucd_path, std::string save_dir, std::string tag_str, std::string assign_tag, float ratio=0.8, float iou_th=0.85);

        // 保存指定范围的图片和标签
        void save_assign_range(std::string ucd_path, std::string save_dir, std::string assign_tag, float iou_th=0.85, std::string mode="xml");

        // 在数据库中找到相似的图片
        std::vector<std::string> search_similar_uc(std::string img_path, int similar_count=10, std::string save_path="", std::string milvus_host="192.168.3.33", int port=19530);

        // 在私人数据库中查找资料，找不到的话使用 chartgpt 寻找结果
        std::map< std::string, std::string> query_to_database_and_chartgpt(
            std::string query, 
            std::string system_content="你是一个优秀的助手，能用简洁但是明确的回答帮助询问者,通常你用一句话就能解决提问者的疑惑", 
            int max_tokens=1000, 
            float threshold=0.4, 
            bool search_database=true);

    private:
        
        // 下载云上的数据
        void load_file(std::string url, std::string save_path, int index=-1);
};


#endif