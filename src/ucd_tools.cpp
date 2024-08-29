
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ucd_tools.hpp"
#include <netdb.h>
#include <vector>
#include "include/deteRes.hpp"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "include/pystring.h"
#include "include/ucDatasetUtil.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include "include/fileOperateUtil.hpp"
#include <chrono>

#define ERROR_COLOR         "\x1b[1;31m"    // 红色
#define HIGHTLIGHT_COLOR    "\033[1;35m"    // 品红
#define WARNNING_COLOR      "\033[1;33m"    // 橙色
#define STOP_COLOR          "\033[0m"       // 复原

using namespace httplib;
using json = nlohmann::json;

std::string save_xml_dir = "";


int findUnusedPort() 
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;

    // Start from a specific port range (e.g., 5000-6000)
    for (int port = 5000; port <= 6000; ++port) {
        addr.sin_port = htons(port);
        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) != -1) {
            close(sock);
            return port;
        }
    }

    close(sock);
    return -1;  // No available port found
}

std::string get_ip_address() 
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];
    std::string ip_address;

    if (getifaddrs(&ifaddr) == -1) 
    {
        std::cerr << "Failed to get network interface information." << std::endl;
        return ip_address;
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr)
            continue;

        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
            if (s != 0) {
                std::cerr << "Failed to get IP address: " << gai_strerror(s) << std::endl;
                return ip_address;
            }

            if (std::strcmp(ifa->ifa_name, "lo") != 0) {
                ip_address = host;
                break;
            }
        }
    }

    freeifaddrs(ifaddr);

    return ip_address;
}

void receive_img(const Request& req, Response& res) 
{
    try 
    {
        json json_dict          = json::parse(req.body);
        json alarms             = json_dict["alarms"];
        std::string file_name   = json_dict["file_name"];
        std::string batch_id    = json_dict["batch_id"];
        int width               = json_dict["width"];
        int height              = json_dict["height"];

        // 将数据转化为 DeteRes
        DeteRes *dete_res   = new DeteRes();
        dete_res->width     = width;
        dete_res->height    = height;
        dete_res->depth     = 3;
        dete_res->file_name = file_name;

        std::cout << HIGHTLIGHT_COLOR << "dete_res_info : " << STOP_COLOR << std::endl;
        std::cout << "    batch_id  : " << batch_id << std::endl;
        std::cout << "    file_name : " << file_name << std::endl;
        std::cout << "    alarms    : " << std::endl;

        for (const auto& alarm : alarms) 
        {
            int x1 = alarm[1];
            int y1 = alarm[2];
            int x2 = alarm[3];
            int y2 = alarm[4];
            float conf = alarm[6];
            std::string tag = alarm[5];
            std::cout << "                " << std::left << std::setw(6) << std::to_string(x1) + ", " << std::left << std::setw(6)<< std::to_string(y1) + ", " << std::left << std::setw(6)<< std::to_string(x2) + ", " << std::left << std::setw(6)<< std::to_string(y2) + ", " << std::left << std::setw(8)<< conf << ", " << std::left << std::setw(10)<< tag << std::endl;
            dete_res->add_dete_obj(x1, y1, x2, y2, conf, tag);
        }
        std::cout << std::endl;

        if(save_xml_dir != "")
        {
            std::string save_xml_path = save_xml_dir + "/" + file_name + ".xml";
            dete_res->save_to_xml(save_xml_path);
        }
        res.set_content("success", "text/plain");
        delete dete_res;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        res.set_content("ERROR: " + std::string(e.what()), "text/plain");
    }

}

void heart_beat(const Request& req, Response& res) 
{
    try 
    {
        json json_dict = json::parse(req.body);
        int img_count = json_dict["img_count"];
        int img_index = json_dict["img_index"];
        std::string batch_id = json_dict["batch_id"];
        std::string if_end = json_dict["if_end"];
        json error_info = json_dict["error_info"];
        std::vector<std::string> error_001 = error_info["001"];
        std::vector<std::string> error_002 = error_info["002"];
        std::vector<std::string> error_003 = error_info["003"];

        std::cout << WARNNING_COLOR << "heart_beat_info : " << STOP_COLOR << std::endl;
        std::cout << "    " << "index/count : " << img_index << "/" << img_count << std::endl; 
        std::cout << "    " << "batch_id    : " << batch_id << std::endl; 
        if(if_end == "False")
        {
            std::cout << "    " << "if_end      : " << if_end << std::endl; 
        }
        else
        {
            std::cout << "    " << "if_end      : " << ERROR_COLOR << if_end << STOP_COLOR << std::endl; 
        }
        std::cout << "    " << "error_info  : " << std::endl; 
        std::cout << "                 " << "dete error      : " << ERROR_COLOR << pystring::join(",", error_001) << STOP_COLOR << std::endl; 
        std::cout << "                 " << "download error  : " << ERROR_COLOR << pystring::join(",", error_002) << STOP_COLOR << std::endl; 
        std::cout << "                 " << "post error      : " << ERROR_COLOR << pystring::join(",", error_003) << STOP_COLOR << std::endl; 
        std::cout << std::endl;
        res.set_content("OK", "text/plain");
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        res.set_content("ERROR: " + std::string(e.what()), "text/plain");
    }
}

int post_v2(std::string img_server_host, int img_server_port, std::string ucd_path, std::string url, std::vector<std::string> model_list, std::string batch_id, std::string save_dir, int receive_port)
{
    // 获取返回的 url 地址
    int use_port;
    if(receive_port == -1)
    {
        use_port    = findUnusedPort();
    }
    else
    {
        use_port    = receive_port;
    }

    std::string local_host = get_ip_address();
    std::string post_url        = "http://" + local_host + ":" + std::to_string(use_port) + "/res";
    std::string heart_beat_url  = "http://" + local_host + ":" + std::to_string(use_port) + "/heart_beat";

    UCDataset *ucd = new UCDataset(ucd_path);
    ucd->parse_ucd(false);

    std::string all_img_path = "";
    for(int i=0; i<ucd->uc_list.size(); i++)
    {
        if(i == 0)
        {
            all_img_path = all_img_path + "http://" + img_server_host + ":" + std::to_string(img_server_port) + "/file/" + ucd->uc_list[i] + ".jpg" + "-+-" + ucd->uc_list[i];
        }
        else
        {
            all_img_path = all_img_path + ",http://" + img_server_host + ":" + std::to_string(img_server_port) + "/file/" + ucd->uc_list[i] + ".jpg" + "-+-" + ucd->uc_list[i];
        }
    }

    // python 中 form 中拿到的都是表单对象，当时应该放到 raw json 对象中去的
    std::string model_list_str = pystring::join(",", model_list);
    httplib::MultipartFormDataItems items = {
        { "model_list",     model_list_str, "", "" },
        { "img_path_list",  all_img_path,   "", "" },
        { "post_url",       post_url,       "", "" },
        { "heart_beat_url", heart_beat_url, "", "" },
        { "batch_id",       batch_id }
    };

    // 打印 post 信息
    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << WARNNING_COLOR << "img_path_num                : " << STOP_COLOR << ucd->uc_list.size() << std::endl;
    std::cout << std::endl;
    std::cout << WARNNING_COLOR << "url                         : " << STOP_COLOR << url + "/dete" << std::endl;
    std::cout << std::endl;
    std::cout << WARNNING_COLOR << "model_list                  : " << STOP_COLOR << model_list_str << std::endl;
    std::cout << std::endl;
    std::cout << WARNNING_COLOR << "(receive)post_url           : " << STOP_COLOR << post_url << std::endl;
    std::cout << std::endl;
    std::cout << WARNNING_COLOR << "(receive)heart_beat_url     : " << STOP_COLOR << heart_beat_url << std::endl;
    std::cout << std::endl;
    std::cout << WARNNING_COLOR << "batch_id                    : " << STOP_COLOR << batch_id << std::endl;
    std::cout << std::endl; 
    if(save_dir == "")
    {
        std::cout << WARNNING_COLOR << "save_xml_dir            : " << STOP_COLOR << "None" << std::endl;
    }
    else
    {
        save_xml_dir = save_dir + "/" + batch_id;
        create_folder(save_dir);
        create_folder(save_xml_dir);
        std::cout << WARNNING_COLOR << "save_xml_dir      : " << STOP_COLOR << save_dir << std::endl; 
    }
    std::cout << "--------------------------------------------------------------" << std::endl;

    // 将 JSON 对象序列化为字符串
    httplib::Client client(url.c_str());
    httplib::Result result = client.Post("/dete", items);

    // 查看是否正常 post 
    if (result.error() == httplib::Error::Success) 
    {
        std::cout << "Response: " << result.value().body << std::endl;
    }
    else 
    {
        std::cerr << "Failed to send POST request: " << result.error() << std::endl;
        return 0;
    }

    // 开始接收心跳和返回信息
    Server server;
    server.Post("/res",         receive_img);
    server.Post("/heart_beat",  heart_beat);
    server.listen("0.0.0.0",    use_port);

    delete ucd;
    return 1;
}

double getPythonStyleTimestamp() 
{
    using namespace std::chrono;
    auto now = system_clock::now();
    auto duration = now.time_since_epoch();
    double timestamp = duration.count() / double(system_clock::period::den);
    return timestamp;
}

std::string timestampToString(double timestamp) 
{
    using namespace std::chrono;

    // 将时间戳转换为系统时间点
    auto ts = time_t(timestamp);
    auto time_point = system_clock::from_time_t(ts);

    // 获取当前时间的本地时间
    std::tm tm = *std::localtime(&ts);

    // 创建一个stringstream用于格式化输出
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}

bool remove_image_meta_data(std::string img_path, std::string save_path, int quality)
{
    cv::Mat image = cv::imread(img_path, cv::IMREAD_COLOR);
    if (!image.data)
    {
        std::cout << "Could not open or find the image" << std::endl;
        return false;
    }

    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(quality);

    bool success = cv::imwrite(save_path, image, compression_params);
    return success;
}
