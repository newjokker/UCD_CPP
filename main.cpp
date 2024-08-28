
#include <iostream>
#include <fstream>
#include <set>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "include/strToImg.hpp"
#include "include/deteRes.hpp"
#include "include/operateDeteRes.hpp"
#include "include/pystring.h"
#include "include/fileOperateUtil.hpp"
#include "include/ucDatasetUtil.hpp"
#include "include/xini_file.h"
#include "include/saturn_database_sql.hpp"
#include "include/paramInfo.hpp"
#include "include/printCpp.hpp"
#include "include/lablelmeObj.hpp"
#include <nlohmann/json.hpp>
#include "include/tqdm.h"
#include <regex>
#include "include/the_book_of_change.hpp"
#include "include/parseArgs.hpp"
#include <hiredis/hiredis.h> 
#include "include/redisBook.hpp"
#include <thread>
#include <httplib.h>
#include "include/ucd_tools.hpp"

using json = nlohmann::json;
using namespace jotools;
using namespace std;

#define ERROR_COLOR         "\x1b[1;31m"    // 红色
#define HIGHTLIGHT_COLOR    "\033[1;35m"    // 品红
#define WARNNING_COLOR      "\033[1;33m"    // 橙色
#define STOP_COLOR          "\033[0m"       // 复原


// nginx 负载均衡，可以在风火轮上部署，转到 111 和 209 服务器上

// 完成 C++ 版本的 文件 服务，再部署到 docker 上面，这样在哪个服务器上都能方便进行启动

// 下载某一些图片报错，库中就没对应的图片，是先 rename 后入库导致的, 还有就是没有权限读取缓存文件导致的

// 解析出来的 json 中存放的 jpg 图片是压缩过的，这样存放的数据能小一点

// add meachine learning , basic content,  

// label_used 是无序的要记得这个

// 数据组平时要维护的四个服务（1）检测服务 （2）数据集标注错误检查服务, cleanlab （3）数据集标注遗漏检查服务（4）数据集更新工具, ucd 实现

// idea -> interface -> app -> language

// 仿照数据库语言的解析方式和语法，搞一个 ucd 语言，简单一点的只是个 demo 就行

// 将需要的功能进行服务化，
// ucd 只是一个数据集处理的工具，不需要增加太多的功能

// ucd 支持自定义的服务接口（1）检测服务 （2）数据检查服务，查看数据检查的进度，还有几个在等待的，还需要等待大概多长时间，检测到什么进度了等信息

// server 
// 查看提供的检测服务
// ucd dete_check (每一个服务做成一个 docker, 可以查看当前服务是否可用, 所有的服务做成 post uc get xml_file 的形式，可以直接保存，客户端那边不需要复杂的解析操作)
// ucd dete prebase test.json save_dir (存在缓存文件的不进行检测)
// ucd dete 
// 检测过程使用进度条显示进度，默认是自动使用保存文件夹下面的路径

// draw count_tags cut_small_img 都要能针对一个 uc 进行处理

// 检测服务函数要的，docker 和 ucd 之间的交互要使用新的方式，（1）写一个 docker 管理器，管理当前电脑上的 docker 服务（2）docker 不使用的时候不要占据太多的资源，可以一直在后台启动

// ucd 作为内部检测服务的标注检测接口，快速部署，灵活变动，最高的速度，共用缓存（ucd_cache）

// ucd dete ucd_path host port save_dir（（1）dete 作为标准接受接口，（2）一张图一个返回，返回的是标准的 xml 文件，（3）能实时看检测的效果（4）保存历史结果，对于历史上检测过的信息直接返回（5）正在检测时，阻止其他服务连接）

// ucd dete 一次发一个 uc 过去，返回一个 xml 文件回来

// 使用 ucd 直接启动 docker，就直接使用武汉那个接口，指定输入检测文件夹换为指定输入 ucd 即可，不用 ucd 直接启动 docker，直接用 ucd 打印启动的命令即可，记不住的是 docker 启动命令

// 增加全文匹配额功能，对指定的关键字进行查找

// 定时任务 + 一次检测一个 文件夹 ucd + 检测结果也是 ucd Die_prebase_0_5_0, 这样的话只要分析一下 uc_analysis 就知道要下载哪些 ucd 进行 absorb

// TODO: 写一个规则打印的函数，每次都要查询烦得要死，还不如自己实现一个

// 超大数据集可以直接存储为存文本数据，这样可以一行一行读取，前面是头文件，就是数据的 uc 信息，以及不同的 uc 信息在多少行，这样的话只要遍历一下头文件就能快速拿到 data 数据，这样更加合理

// 看看是不是 labelmeObj 结构体占用的空间太大了，去掉其中没用的属性和函数

// TODO: 一个训练分类的服务，输入是一个 ucd 和 保存模型的文件夹地址，或者直接就是 一个模型地址，值保留在验证集中效果最好的一个模型就行

// 申请更多的内存，现在共享内存用不了多少就崩掉了，如何能申请更多的内存，一般服务器都是 128G 内存

// 有些图片截取小图之后是反色的，为什么会这样，是因为原图的问题吗？Dxm00d0

// TODO: count_folder， 统计文件夹中的文件信息，统计文件夹的大小

// TODO: 做成 python 类型，输入 python 之后直接进入 pythyon 的环境变量，一直到 按 ctrl + c 才退出

// TODO: AP50 的概念搞错了，是控制 iou 而不是 conf 

// TODO: ucd + docker 可以根据模型 + config 直接跑出结果，貌似比 saturn_lib 更加方便

// TODO: 使用 ucd 检查可用的服务，服务需要一直开着才能接收到 ucd 的交互信息

// TODO: ucd 中集成那些可以训练或者统计的东西，（1）统计每个函数查看语法的概率 （2）查看每个函数的使用频次 （3）

// TODO: ucd 语法的自动填充，节省人力，减少报错

// TODO: 根据各个部件之间的相关关系推断，某一个部件的检测是否存在错误，部件的相关关系，K 近邻的方式对一张图进行描述，比如是输电还是配电，衡量距离的方式就是 检测目标分布的差异性

// FIXME: save 进度条会出现不动的情况，当保存的地址是有图片的时候

// TODO: 增加 cleanlab_server 方式的 cut_small_img ，或者 先 split 分为 N 份，裁剪小图的代码多写几个使用分好隔开就行

// TODO: 可以指定所有的标签都是一个颜色，用于结果检查，for_improve_recall， --assign_color [0,0,255] 

// TODO: 所有人使用一个版本就是最新版本，每次只要我自己去更新那几个文件就行，但是我如何把权限给所有的人呢

// TODO: drop_tags 和 filter_by_tags 是不是文件名影响的 

// TODO: 完善一个项目词汇表，将所有的命名规范化，统一调整所有的接口，新接口和老接口分开（filter，fix，drop，abosrb，minus，merge，from，get，set，[to(xml, labelme_json, yolo_txt)]）

// TODO: diff_obj 对比 目标对象有对少是重合的，按照标签打印其中重合的个数和重合的比例，打印出每个标签有多少重合的，各自有多少不一样的，

// TODO: 所有操作都有涉及到 uc 或者设计 obj 两种的操作，是默认使用 uc 操作，增加 -o 使用 obj 操做，全部是这个逻辑比较好，minus, merge, diff, absorb

// FIXME: ucd 多个用户使用的时候，a 下载的图片 b 没有读取的权限，需要将缓存文件夹设置为组内所有人都可以读写 chown user:group /path/to/folder

// TODO: intersection 支持多个文件进行合并

// TODO: 将 libtorch 搞成一个很大的可执行文件，这样很多事情就非常方便地进行处理了，甚至可以对 prebase 之类的直接生成一个 可执行文件，和 ucd 绑定起来，这样检测文件非常方便

// TODO: 可以不主动设置缓存文件夹，主动找一个路径，比如 /usr/local/ucd_cache ，这样用着就会很方便

// TODO: 对那些没有标准化的数据也能分析，不一定要改为 uc 命名，可以设置是否改为 uc 命名

// TODO: ucd 成为一门语言

// TODO: 将 MD5 生成 uc 这一步也放到 ucd 来做，
        // (0) ucd 连接不到 80 服务器就直接报错 (1) get_uc (2) ucd 生成的和saturnDatabase 生成的用一种方法进行分开 (3) 服务端接收上传的图片复制整理到对应的文件夹中去
        // (4) .json 文件可有可无，没必要一定去生成这个文件
        // (5) 考虑快速入库的方法，如何快速申请一连串的 uc，输入 md5_list 返回的是 uc_list
        // (6) 输入 uc_list 返回 md5_list
        // (7) 申请 uc 的过程可以更加优雅一些，要更加的有 鲁棒性，有一个在申请，另外一个直接就报错就行，要绝对保证 uc 的唯一性
        // （8）支持文件名为 md5 和 uc，两个都是合法的文件名，uc 是作为 md5 的简化，每一个 md5 应该能动态生成唯一的 uc
        // (9)入库在 111 上专门写一个服务，用于接收传输过去的图片，传输的格式为 (uc, md5, img_data)
        // (10) 数据库需要保存的数据为 md5_uc, 日期_已分配uc数目
        // (11) 专门入库的这个功能单独做成一个小工具，不一定要和 ucd 进行合并

// TODO: ucd 可以检查当前主板的 ID ，绑定主板 ID，当前版本只能这个主板使用，在使用之前需要进行联网激活, sudo dmidecode -s system-uuid, 03C00218-044D-0543-5A06-950700080009
            // UUID是由系统生成的，因此除非您更改了硬件或重新安装操作系统，否则UUID应该是唯一的且不变的

// TODO: 分组信息，有些数据 add 进去是有组的信息的，可以在 merge 的时候指定数据的组信息，group，对 uc 进行分组还是对 obj 进行分组
        // split_by_group
        // obj_info 中可以增加组的信息，随着 obj 中的 uc 进行存储 
        // obj 中增加 group 信息，可以在 merge 的时候进行指定
        // filter_by_group
        // 

// TODO: 请求 uc 部分应该是 服务端提供的功能，指定一个 md5_list 服务端就应该返回一个 md5_dict(md5:uc), 这应该全部是服务端提供的功能
            // uc_v2_server
            // uc_v1 和 uc_v2 之间是不能混合使用的，新的 uc 第一个字母是小写，后面两个字母是大写和数字, 日期按照天数来计算，计算到 1970 年到现在一共过去了多少天，


// TODO: 共用同一批 config 文件

// TODO: 增加一批可以直接调用的 运行文件，这样可以作为额外的服务进行部署, 可以改为通用的 docker 环境，只需要配置 模型和配置文件即可

// TODO: saki 的小说里面的每一个人物的赏析

// TODO: from_crop 中增加选项可以从本地文件中找 uc 的大小获取 size_info 信息

// TODO: relate_anlysis 关联性分析，出现 a 标签的同时出现 b 标签的概率

// TODO: 完善 load 和 upload , 指定需要上传的文件夹

// TODO: to_yolo_train_data 打印用于训练的各个元素的个数 

// TODO: 在使用 ucd 处理信息的时候将使用的环境发送给服务器，这样的话，就能知道谁在下载数据库里面的数据，下载了多少，下的都是什么数据，可以根据 history 日志来获取

// TODO: 支持 chartgpt api 已经部署在 221 服务器 post 11223，速度貌似不快（1）生成一段，显示一段（2）将显示的内容格式化打印出来

// TODO: 配网缺陷表之类的数据可以直接在 ucd book 中查找到

// TODO: 根据某一个标签对所有的 obj 进行删选，可以提前把指定的 去删选的标签标定为同样的名字，

// TODO: 给 ucd 增加自定义的插件，只要符合规则进行编写就行，就类似实现简单的自定义的语言

// TODO: crop 的时候可以将图画出来，这样就能在大图中看画出的小图，同理 draw 时候可以指定范围，但这两种和方法都是有限制的，需要想一个中间一点的方法

// TODO: 方便将 compare.json 调整为需要的可以方便调整测试集的形式，比如文件夹名字和里面的标签名字不一致

// TODO: cache_from, 可以将指定文件夹中的符合 ucd 命名的图片吸收到缓存中去

// TODO: update --source 可以指定提供服务的地址，可以放在 github 上，提供下载，


// TODO: filter_by_date --gt 大于某一天 --lt 小于某一天 (时间和 uc 之间的转换已经在 ucDataset 中实现了)0

// TODO: 开辟 other，存放一些不是为了 json 实现的小功能

// TODO: 对比两个文件夹，删除了哪些文件，新增了哪些文件

// TODO: 实现插件功能，插件可以单独开发编译，

// TODO: 大部分的功能支持未入库重命名的数据

// TODO: 增加离线版本

// TODO: 可以将整理的文档放到 github 上，ucd 增加下载在线文档的功能，下载后在更新到对应的 redis 服务中去，或者直接更新到本地，这样的话更加的方便安全

// TODO: 对于 ucd 想要快速地查看各个软件的 命令，应该如何处理，



void handle_post(const httplib::Request& req, httplib::Response& res) 
{
    // 打印接收到的 POST 内容和额外的参数
    std::cout << "Received POST data: " << req.body << std::endl;

    // 返回响应
    res.set_content("OK", "text/plain");
}


int main(int argc_old, char ** argv_old)
{
    // param
    UcdParamOpt *ucd_param_opt = new UcdParamOpt();
    ucd_param_opt->load_param_info();

    // 自定义的方法解析参数
    std::vector<std::string> argv;
    std::set<std::string> short_args;
    std::map< std::string, std::string > long_args;
    bool status = parse_args(argc_old, argv_old, argv, short_args, long_args);
    int argc = argv.size();

    // server
    std::string host        = "192.168.3.111";
    int port                = 11101;
    std::string config_path = "";
    std::string history_path= "";
    
    // sql info 
    int sql_port = 3306;
    std::string sql_host    = "192.168.3.101";
    std::string sql_user    = "root";
    std::string sql_pwd     = "root123";
    std::string sql_db      = "Saturn_Database_V1";
    
    // milvus info 
    int milvus_port = 19530;
    std::string milvus_host = "192.168.3.33";

    // redis info
    int redis_port          = 6379;
    std::string redis_host  = "192.168.3.221";
    std::string redis_name  = "anonymity";
    
    // app dir path
    std::string app_dir     = "/home/ldq/Apps_jokker";

    // version
    std::string app_version = "v4.10.7";

    // uci_info
    int volume_size         = 20;

    // castration function 阉割功能
    std::string castration_function;

    // cache dirmake
    std::string cache_dir;

    // get user name
    struct passwd* pwd;
    uid_t userid;
	userid              = getuid();
	pwd                 = getpwuid(userid);
    std::string pw_name = pwd->pw_name;
    
    // if config_path is "~/ucdconfig.ini" can't read the file, so should get the user name for ~
    if(pw_name == "root")
    {
        config_path     = "/" + pw_name + "/ucdconfig.ini";
        history_path    = "/" + pw_name + "/.ucdhistory.txt";
        cache_dir       = "/" + pw_name + "/.cache";
    }
    else
    {
        config_path     = "/home/" + pw_name + "/ucdconfig.ini";
        history_path    = "/home/" + pw_name + "/.ucdhistory.txt";
        cache_dir       = "/home/" + pw_name + "/.cache";
    }

    // read config
    if(is_file(config_path))
    {
        xini_file_t xini_file(config_path);
        host        = (const std::string &)xini_file["server"]["host"];
        port        = (const int &)xini_file["server"]["port"];
        app_dir     = (const std::string &)xini_file["server"]["app_dir"];
        sql_host    = (const std::string &)xini_file["sql"]["host"];
        sql_port    = (const int &)xini_file["sql"]["port"];
        sql_user    = (const std::string &)xini_file["sql"]["user"];
        sql_pwd     = (const std::string &)xini_file["sql"]["pwd"];
        sql_db      = (const std::string &)xini_file["sql"]["db"];
        cache_dir   = (const std::string &)xini_file["cache"]["dir"];
        volume_size = ((const int &)xini_file["uci"]["volume_size"]);
        redis_port  = ((const int &)xini_file["redis"]["port"]);
        redis_host  = ((const std::string &)xini_file["redis"]["host"]);
        redis_name  = ((const std::string &)xini_file["redis"]["name"]);
        castration_function  = ((const std::string &)xini_file["server"]["castration_function"]);
        milvus_host = (const std::string &)xini_file["milvus"]["host"];
        milvus_port = (const int &)xini_file["milvus"]["port"];
        // 分卷大小不能为 0 会有很多问题 
        if(volume_size <= 0)
        {
            volume_size = 30;
        }
    }
    else
    {
        xini_file_t xini_write(config_path);
        xini_write["cache"]["dir"]      = cache_dir;
        xini_write["server"]["host"]    = host;
        xini_write["server"]["port"]    = port;
        xini_write["server"]["app_dir"] = app_dir;
        xini_write["sql"]["host"]       = sql_host;
        xini_write["sql"]["port"]       = sql_port;
        xini_write["sql"]["user"]       = sql_user;
        xini_write["sql"]["pwd"]        = sql_pwd;
        xini_write["sql"]["db"]         = sql_db;
        xini_write["uci"]["volume_size"]= volume_size;
        xini_write["redis"]["port"]     = redis_port;
        xini_write["redis"]["host"]     = redis_host;
        xini_write["redis"]["name"]     = redis_name;
        xini_write["server"]["castration_function"]     = castration_function;
        xini_write["milvus"]["host"]    = milvus_host;
        xini_write["milvus"]["port"]    = milvus_port;
        xini_write.dump(config_path);   
    }

    // save history info
    ofstream outfile(history_path, ofstream::app);
    time_t now = time(0);
    tm *ltm = localtime(&now);
    outfile << 1900 + ltm->tm_year << "-" << 1 + ltm->tm_mon << "-" << ltm->tm_mday << " " << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << " : ";
    // 
    for(int i=0; i<argc_old; i++)
    {
        outfile << argv_old[i];
        outfile << " ";
    }
    outfile << std::endl;
    outfile.close(); 

    // init ucd_util
    UCDatasetUtil* ucd_util = new UCDatasetUtil(host, port, cache_dir);

    if(!is_write_dir(ucd_util->cache_img_dir))
    {
        create_folder(ucd_util->cache_img_dir);
    }
    
    if(!is_write_dir(ucd_util->cache_xml_dir))
    {
        create_folder(ucd_util->cache_xml_dir);
    }
    
    // ucd | ucd -v | ucd -V 
    if ((argc < 2) && (short_args.count("v")==0) && (short_args.count("V")==0))
    {
        // std::cout << "need parameter number >= 1 get : " << argc-1 << std::endl;
        // refer : https://patorjk.com/software/taag/#p=display&f=Graffiti&t=Type%20Something%20

        std::cout << "\x1b[1;35m" ;
        std::cout <<"            _____                      _____                      _____          "<< std::endl;
        std::cout <<"           /\\    \\                    /\\    \\                    /\\    \\         "<< std::endl;
        std::cout <<"          /::\\____\\                  /::\\    \\                  /::\\    \\        "<< std::endl;
        std::cout <<"         /:::/    /                 /::::\\    \\                /::::\\    \\       "<< std::endl;
        std::cout <<"        /:::/    /                 /::::::\\    \\              /::::::\\    \\      "<< std::endl;
        std::cout <<"       /:::/    /                 /:::/\\:::\\    \\            /:::/\\:::\\    \\     "<< std::endl;
        std::cout <<"      /:::/    /                 /:::/  \\:::\\    \\          /:::/  \\:::\\    \\    "<< std::endl;
        std::cout <<"     /:::/    /                 /:::/    \\:::\\    \\        /:::/    \\:::\\    \\   "<< std::endl;
        std::cout <<"    /:::/    /      _____      /:::/    / \\:::\\    \\      /:::/    / \\:::\\    \\  "<< std::endl;
        std::cout <<"   /:::/____/      /\\    \\    /:::/    /   \\:::\\    \\    /:::/    /   \\:::\\ ___\\ "<< std::endl;
        std::cout <<"  |:::|    /      /::\\____\\  /:::/____/     \\:::\\____\\  /:::/____/     \\:::|    |"<< std::endl;
        std::cout <<"  |:::|____\\     /:::/    /  \\:::\\    \\      \\::/    /  \\:::\\    \\     /:::|____|"<< std::endl;
        std::cout <<"   \\:::\\    \\   /:::/    /    \\:::\\    \\      \\/____/    \\:::\\    \\   /:::/    / "<< std::endl;
        std::cout <<"    \\:::\\    \\ /:::/    /      \\:::\\    \\                 \\:::\\    \\ /:::/    /  "<< std::endl;
        std::cout <<"     \\:::\\    /:::/    /        \\:::\\    \\                 \\:::\\    /:::/    /   "<< std::endl;
        std::cout <<"      \\:::\\__/:::/    /          \\:::\\    \\                 \\:::\\  /:::/    /    "<< std::endl;
        std::cout <<"       \\::::::::/    /            \\:::\\    \\                 \\:::\\/:::/    /     "<< std::endl;
        std::cout <<"        \\::::::/    /              \\:::\\    \\                 \\::::::/    /      "<< std::endl;
        std::cout <<"         \\::::/    /                \\:::\\____\\                 \\::::/    /       "<< std::endl;
        std::cout <<"          \\::/____/                  \\::/    /                  \\::/____/        "<< std::endl;
        std::cout <<"           ~~                         \\/____/                    ~~              "<< std::endl;
        std::cout << "\033[0m" << std::endl;
        std::cout << "" << std::endl;

        std::cout << WARNNING_COLOR << "Welcome to UCD!" << STOP_COLOR << std::endl;
        std::cout << " " << std::endl;
        std::cout <<"   * use " << HIGHTLIGHT_COLOR << "ucd help "   << STOP_COLOR << "get ucd function : chinese explain" << std::endl;
        std::cout << "" << std::endl;
        std::cout <<"   * use " << HIGHTLIGHT_COLOR << "ucd grammar "  << STOP_COLOR << "get ucd function : grammar" << std::endl;
        std::cout << " " << std::endl;
        std::cout <<"   * use " << HIGHTLIGHT_COLOR << "ucd help help "  << STOP_COLOR << "get the way to use help" << std::endl;
        std::cout << " " << std::endl;
        std::cout <<"   * use " << HIGHTLIGHT_COLOR << "ucd set cache_dir {cache_dir} "  << STOP_COLOR << "set cache_dir" << std::endl;
        std::cout << " " << std::endl;
        return -1;
    }
    else if((short_args.count("v")>0) || (short_args.count("V")>0))
    {
        ucd_util->get_ucd_version_info(app_dir, app_version);
        return -1;
    }

    std::string command_1;
    if(argc >= 2)
    {
        command_1 = argv[1];
    }
    else
    {
        std::cout << ERROR_COLOR << "not enough parameter" << STOP_COLOR << std::endl;
        return -1;
    }

    // must set ucd_cache 
    if((! is_write_dir(cache_dir)) && (command_1 != "set") && (command_1 != "help")) 
    {
        std::cout << WARNNING_COLOR << "cache_dir not exists, edit ucdconfig.ini cache/dir : " << STOP_COLOR << std::endl;
        std::cout << "ucdconfig path : " << config_path << std::endl;
        std::cout << "-----------------------------------------------------------" << std::endl;
        std::cout << WARNNING_COLOR << "ucd set cache_dir {cache_dir}" << STOP_COLOR << " to set cache_dir " << std::endl;
        std::cout << "-----------------------------------------------------------" << std::endl;
        return -1;
    }
    
    // 检查文件夹权限
    if(command_1 != "set")
    {
        // 文件夹权限检查    
        if(! is_read_dir(ucd_util->cache_img_dir))
        {
            std::cout << ERROR_COLOR << "WARNING : cache_img folder don't have read access : " << ucd_util->cache_img_dir << STOP_COLOR << std::endl;
            return -1;
        }
        else if(! is_write_dir(ucd_util->cache_img_dir))
        {
            std::cout << ERROR_COLOR << "WARNING : cache_img folder don't have write access : " << ucd_util->cache_img_dir << STOP_COLOR << std::endl;
            return -1;
        }
    }

    // key word
    if(command_1 == "zen")
    {
        // ucd 之禅
        // 统一带来流畅与快速
        // 统一在一起慢慢进步
        // 统一才能走的远，各自为政走得快，但会走歪
        // idea -> interface -> app -> language，止与一个有永远达不到的目标
        // 工具成为身体的一部分，因为有了手，
        // 数据集有了 ucd 就像孔子编写了十翼给易经插上了翅膀
        // 
        std::cout << "-------ucd-------"   << std::endl;
        std::cout << "* 统一 操作接口"      << std::endl;
        std::cout << "* 简化 数据集操作"    << std::endl;
        std::cout << "* 促进 信息流动"      << std::endl;
        std::cout << "* 减轻 缓存压力"      << std::endl;
        std::cout << "-----------------"   << std::endl;
        std::cout << "* 简单的功能不要多次实现"         << std::endl;
        std::cout << "* 复杂的功能大家要错错一样的"     << std::endl;
        std::cout << "* 麻烦的功能只恶心编写者一个人"   << std::endl;
        std::cout << "-----------------"   << std::endl;
    }
    else if(command_1 == "check")
    {

        bool print_uc_count = false;
        if(short_args.count("c") > 0)
        {
            print_uc_count = true;
        }

        std::string name = "";
        if(long_args.count("name") > 0)
        {
            name = long_args["name"];
        }

        std::string assign_uc = "";
        if(long_args.count("assign_uc") > 0)
        {
            assign_uc = long_args["assign_uc"];
        }

        if(argc == 2)
        {
            ucd_util->search_ucd(assign_uc, true, print_uc_count, name);
        }
        else if(argc == 3)
        {
            std::string ucd_json_name = argv[2];
            std::map< std::string, std::string > ucd_info;
            ucd_info = ucd_util->get_ucd_json_info(ucd_json_name);
            if(ucd_info.count("error") >0)
            {
                std::cout << ERROR_COLOR << "error : " << ucd_info["error"] << STOP_COLOR << std::endl;
            }
            else
            {
                std::cout << "----------------------------------------------------" << std::endl;
                std::cout << "                UCD JSON INFO" << std::endl;
                std::cout << "----------------------------------------------------" << std::endl;
                std::cout << "dataset_name  :   " << ucd_info["dataset_name"] << std::endl;
                std::cout << "uc_count      :   " << ucd_info["uc_count"] << std::endl;
                std::cout << "file_size     :   " << ucd_info["size"] << std::endl;
                std::cout << "json_path     :   " << ucd_info["json_path"] << std::endl;
                std::cout << "model_name    :   " << ucd_info["model_name"] << std::endl;
                std::cout << "model_version :   " << ucd_info["model_version"] << std::endl;
                std::cout << "json_name     :   " << ucd_info["json_name"] << std::endl;
                std::cout << "add_time      :   " << ucd_info["add_time"] << std::endl;
                std::cout << "update_time   :   " << ucd_info["update_time"] << std::endl;
                std::cout << "----------------------------------------------------" << std::endl;
            }
        }
        else
        {
            ucd_param_opt->print_command_info("check");
            return -1;
        }
    }
    else if(command_1 == "load")
    {        
        if(argc <= 2)
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
        std::string ucd_name = argv[2];        std::string ucd_save_path;

        if(pystring::endswith(ucd_name, ".json"))
        {
            ucd_name = ucd_name.substr(0, ucd_name.size()-5);
        }

        if(argc == 3)
        {
            ucd_save_path = ucd_name + ".json";
        }
        else if (argc == 4)
        {
            ucd_save_path = argv[3];
            if(is_dir(ucd_save_path))
            {
                ucd_save_path += "/" + ucd_name + ".json";
            }
            else if(! pystring::endswith(ucd_save_path, ".json"))
            {
                ucd_save_path += ".json";
            }
        }
        else
        {
            ucd_param_opt->print_command_info("load");
            return -1;
        }

        if(is_write_file(ucd_save_path))
        {
            std::cout << ERROR_COLOR << "save path exists : " << ucd_save_path << STOP_COLOR << std::endl;
            return -1;
        }

        ucd_util->load_ucd(ucd_name, ucd_save_path);
    }
    else if(command_1 == "delete")
    {
        if(argc != 3)
        {
            ucd_param_opt->print_command_info("delete");
            return -1;
        }
        else
        {
            std::string std_name = argv[2];
            ucd_util->delete_ucd(std_name);
        }
    }
    else if(command_1 == "save")
    {

        bool xml_from_json = true;
        if(short_args.count("s") > 0)
        {
            xml_from_json = false;
        }

        if((argc == 5) || (argc == 6))
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];
            std::string save_mode = argv[4];
            
            if(! is_file(ucd_path))
            {
                std::cout << ERROR_COLOR << "json_path not exists : " << ucd_path << STOP_COLOR << std::endl;
                throw "json_path not exists";
            }

            create_folder(save_dir);
           if(! is_dir(save_dir))
            {
                std::cout << ERROR_COLOR << "unable to create multiple levels of directories simultaneously, create folder failed : " << save_dir << STOP_COLOR  << std::endl;
                throw "save_dir not exists";
             }

            // 判断对文件是否有写入权限
            if(access(save_dir.c_str(), 2) != 0)
            {
                std::cout << ERROR_COLOR << "[31mdon't have write access : " << save_dir << STOP_COLOR << std::endl;
                throw "don't have write access";
            }

            // 判断缓存文件夹是否有读取权限
            if(access(ucd_util->cache_img_dir.c_str(), 4) != 0)
            {
                std::cout << ERROR_COLOR << "don't have read access : " << ucd_util->cache_img_dir << STOP_COLOR << std::endl;
                throw "don't have write access";
            }

            if(save_mode.size() != 2)
            {
                std::cout << ERROR_COLOR << "save_mode illeagal, need save_mode such as 11 | 10  " << save_dir << STOP_COLOR << std::endl;
                throw "save_mode illeagal";
            }

            bool need_img, need_xml;
            if(save_mode[0] == '0')
            {
                need_img = false;
            }
            else
            {
                need_img = true;
            }

            if(save_mode[1] == '0')
            {
                need_xml = false;
            }
            else
            {
                need_xml = true;
            }

            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd();

            int need_count = -1;
            std::vector<std::string> uc_vector;
            if(argc == 6)
            {
                need_count = std::stoi(argv[5]);
                uc_vector = ucd->uc_slice(0, need_count);
            }
            else
            {
                uc_vector = ucd->uc_list;
            }

            // 
            std::cout << HIGHTLIGHT_COLOR << "如果出错, 查看对缓存文件夹是否有权限，使用 ucd meta | grep dir 查看缓存文件夹，使用 sudo chmod 777 {cache_dir} -R 修改权限" << STOP_COLOR << std::endl;

            // download img
            if(need_img){ucd_util->load_img(save_dir, uc_vector);}

            // parse xml 
            if(need_xml)
            {
                if(xml_from_json)
                {
                    if(need_img)
                    {
                        ucd_util->parse_voc_xml(save_dir, save_dir, ucd_path);
                    }
                    else
                    {
                        ucd_util->parse_voc_xml(ucd_util->cache_img_dir, save_dir, ucd_path);
                    }
                }
                else
                {
                    ucd_util->load_xml(save_dir, uc_vector);
                }
            }
            delete ucd;
        }
        else
        {
            ucd_param_opt->print_command_info("save");
            return -1;
        }
    }
    else if(command_1 == "save_remote_ucd")
    {
        // 将远程的 ucd 文件全部同步到本地
        if (argc == 3)
        {
            std::string save_dir = argv[2];
            ucd_util->save_remote_ucd(save_dir);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "save_cache")
    {
        // 保存到缓存中，xml 和 img 都有对应的缓存

        // 判断对文件是否有写入权限
        if(access(ucd_util->cache_img_dir.c_str(), 2) == -1)
        {
            std::cout << "don't have write access : " << ucd_util->cache_img_dir << std::endl;
            throw "don't have write access";
        }

        if(argc == 4)
        {
            std::string json_path = argv[2];
            std::string save_mode = argv[3];
            
            // json_path
            if(! is_file(json_path))
            {
                std::cout << "json_path not exists : " << json_path << std::endl;
                throw "json_path not exists";
            }

            // save_mode
            if(save_mode.size() != 2)
            {
                std::cout << "save_mode illeagal, need save_mode such as 11 | 10  " << std::endl;
                throw "save_mode illeagal";
            }

            // load 
            bool need_img, need_xml;
            // std::string save_img_dir = ucd_util->cache_img_dir;
            // std::string save_xml_dir = 
        
            if(save_mode[0] == '0')
            {
                need_img = false;
            }
            else
            {
                need_img = true;
            }

            if(save_mode[1] == '0')
            {
                need_xml = false;
            }
            else
            {
                need_xml = true;
            }

            // load
            UCDataset* ucd = new UCDataset(json_path);
            ucd->parse_ucd();
            // ucd_util->json_path = json_path;
            std::vector<std::string> uc_vector = ucd->uc_list;
            // 
            if(need_img){ucd_util->load_img(ucd_util->cache_img_dir, uc_vector);}
            if(need_xml){ucd_util->load_xml(ucd_util->cache_xml_dir, uc_vector);}
            delete ucd;
        }
        else
        {
            ucd_param_opt->print_command_info("save_cache");
        }
    }
    else if(command_1 == "save_model_res")
    {
        // 下载指定模型库中的数据
        // 最后要打印出有多少结果是没有被下载的，也就是没有跑出结果的。

        // ucd save_model_res model_name ucd_path save_path

    }
    else if(command_1 == "upload")
    {

        // 指定需要上传的文件夹的级别，这样是不是只能上传一个层次的文件夹

        std::string ucd_path, assign_ucd_name;
        if(argc == 3)
        {
            ucd_path = argv[2];
            ucd_util->upload_ucd(ucd_path, "");
        }
        else if(argc == 4)
        {
            ucd_path = argv[2];
            assign_ucd_name = argv[3];
            
            if(pystring::endswith(assign_ucd_name, ".json"))
            {
                assign_ucd_name = assign_ucd_name.substr(0, assign_ucd_name.size()-5);
            }
            
            ucd_util->upload_ucd(ucd_path, assign_ucd_name);
        }
        else
        {
            ucd_param_opt->print_command_info("upload");
            return -1;
        }
    }        
    else if(command_1 == "info")
    {
        if(argc == 3)
        {
            std::string ucd_path = argv[2];
            UCDataset * ucd = new UCDataset(ucd_path);

            if(ucd_util->is_ucd_path(ucd_path))
            {
                ucd->parse_ucd();
                ucd->print_ucd_info();
            }
            else if(ucd_util->is_uci_path(ucd_path))
            {
                ucd->load_uci(ucd_path);
                ucd->print_volume_info();
            }
            else
            {
                std::cout << ERROR_COLOR << "illegal uci path : " << ucd_path << STOP_COLOR << std::endl;
            }

            // 打印标签信息 
            if(short_args.count("a") > 0)
            {
                ucd_util->count_ucd_tags(ucd_path);
            }
            delete ucd;
        }
        else
        {
            ucd_param_opt->print_command_info("info");
            return -1;
        }
    }
    else if(command_1 == "from_assign_crop_xml")
    {
        if(argc == 4)
        {
            std::string xml_dir = argv[2];
            std::string ucd_path = argv[3];
            ucd_util->get_ucd_from_crop_xml(xml_dir, ucd_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "from_crop")
    {
        bool origin_tag = false;
        if(short_args.count("o") != 0)
        {
            origin_tag = true;
        }

        if(argc == 4)
        {
            std::string crop_dir    = argv[2];
            std::string save_path   = argv[3];
            jotools::get_ucd_from_crop_img(crop_dir, save_path, origin_tag);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "from_img")
    {
        if(argc == 4)
        {
            std::string img_path = argv[2];
            std::string ucd_name = argv[3];
            ucd_util->get_ucd_from_img_dir(img_path, ucd_name);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "from_xml")
    {
        if(argc == 4)
        {
            std::string xml_dir = argv[2];
            std::string ucd_path = argv[3];
            ucd_util->get_ucd_from_xml_dir(xml_dir, ucd_path);
        }
        else
        {
            ucd_param_opt->print_command_info("from_xml");
            return -1;
        }
    }
    else if(command_1 == "from_huge_xml")
    {
        if(argc == 4 or argc == 5)
        {
            std::string xml_dir     = argv[2];
            std::string save_path   = argv[3];
            int volume_size         = volume_size;                   // 30 大概 280M

            if(argc == 5)
            {
                std::string volume_size_str = argv[4];
                volume_size = std::stoi(volume_size_str);
            }
            ucd_util->get_ucd_from_huge_xml_dir(xml_dir, save_path, volume_size);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "from_json")
    {
        if(argc == 4)
        {
            std::string json_dir = argv[2];
            std::string ucd_path = argv[3];
            ucd_util->get_ucd_from_json_dir(json_dir, ucd_path);
        }
        else
        {
            ucd_param_opt->print_command_info("from_json");
            return -1;       
        }
    }
    else if(command_1 == "from_file")
    {
        if(argc == 4)
        {
            std::string file_dir = argv[2];
            std::string ucd_path = argv[3];
            ucd_util->get_ucd_from_file_dir(file_dir, ucd_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;       
        }
    }
    else if(command_1 == "from_uc")
    {
        if(argc > 3)
        {
            std::string save_ucd_path = argv[2];
            std::vector<std::string> uc_list;
            for(int i=3; i<argc; i++)
            {
                std::string uc = argv[i];
                uc_list.push_back(uc);
            }
            ucd_util->get_ucd_from_uc_list(save_ucd_path, uc_list);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "from_yolo_txt")
    {
        // 用于辅助获取 size_info 的 json 文件
        std::string size_ucd_path = "";
        if(long_args.count("size_ucd") > 0)
        {
            size_ucd_path = long_args["size_ucd"];
        }

        if(argc == 4)
        {
            std::string txt_folder      = argv[2];
            std::string save_ucd_path   = argv[3];
            ucd_util->get_ucd_from_yolo_txt_dir(txt_folder, save_ucd_path, size_ucd_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "from_dete_server")
    {
        if(argc == 5)
        {
            std::string server_dir  = argv[2];
            std::string ucd_path    = argv[3];
            std::string save_path   = argv[4];
            ucd_util->get_ucd_from_dete_server(server_dir, ucd_path, save_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "parse_xml")
    {
        if(argc == 4)
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];

            if(! is_file(ucd_path))
            {
                std::cout << "ucd_path not exists : " << ucd_path << std::endl;
                throw "ucd_path not exists";
            }
            ucd_util->parse_voc_xml(ucd_util->cache_img_dir, save_dir, ucd_path);
        }
        else
        {
            ucd_param_opt->print_command_info("parse_xml");
            return -1;
        }
    }
    else if(command_1 == "parse_volume_xml")
    {
        if(argc == 4)
        {
            std::string uci_path = argv[2];
            std::string save_dir = argv[3];

            if(! is_read_file(uci_path))
            {
                std::cout << ERROR_COLOR << "illegal uci_path : " << STOP_COLOR << uci_path << std::endl;
                return -1;
            }
            ucd_util->parse_volume_voc_xml(ucd_util->cache_img_dir, save_dir, uci_path);
        }
        else
        {
            ucd_param_opt->print_command_info("parse_xml");
            return -1;
        }
    }  
    else if(command_1 == "parse_json")
    {
        // todo 在 ucd_util 中实现一下，是一个图像一个图像进行保存，不要
        if(argc == 4)
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];

            if(! is_file(ucd_path))
            {
                std::cout << "ucd_path not exists : " << ucd_path << std::endl;
                throw "ucd_path not exists";
            }
            ucd_util->parse_labelme_json(ucd_util->cache_img_dir, save_dir, ucd_path);
        }
        else
        {
            ucd_param_opt->print_command_info("parse_json");
        }
    }
    else if(command_1 == "show")
    {
        std::string uc;
        if(argc == 3)
        {
            uc = argv[2];
        }
        else if(argc == 2)
        {
            uc = "{UC}";
        }
        else
        {
            ucd_param_opt->print_command_info("show");
            return -1;
        }
        // 展示所有的下载路径，为了方便单张图片的信息下载查看，可以使用 curl 下载单个 uc 对应的信息
        std::cout << "load img      : http://" + ucd_util->host + ":" + std::to_string(ucd_util->port) + "/file/" + uc + ".jpg" << std::endl;
        std::cout << "load xml      : http://" + ucd_util->host + ":" + std::to_string(ucd_util->port) + "/file/" + uc + ".xml" << std::endl;
        std::cout << "load json     : http://" + ucd_util->host + ":" + std::to_string(ucd_util->port) + "/file/" + uc + ".json" << std::endl;
        std::cout << "load ucd      : http://" + ucd_util->host + ":" + std::to_string(ucd_util->port) + "/ucd/{ucd_name}.json" << std::endl;
        std::cout << "check         : http://" + ucd_util->host + ":" + std::to_string(ucd_util->port) + "/ucd/check" << std::endl;
    }
    else if(command_1 == "diff")
    {
        // todo 只是 uc 的差异，是否可以表示 tag 的差异 
        if(argc == 4)
        {
            std::string ucd_path_1 = argv[2];
            std::string ucd_path_2 = argv[3];
            ucd_util->ucd_diff(ucd_path_1, ucd_path_2);
        }
        else
        {
            ucd_param_opt->print_command_info("diff");
            return -1;
        }
    }
    else if(command_1 == "merge")
    {
        if(argc >= 5)
        {
            std::string save_path = argv[2];
            std::vector<std::string> ucd_path_vector;
            for(int i=0; i<argc-3; i++)
            {
                std::string each_ucd_path = argv[i+3];
                ucd_path_vector.push_back(each_ucd_path);
            }
            ucd_util->merge_ucds(save_path, ucd_path_vector);
        }
        else
        {
            ucd_param_opt->print_command_info("merge");
            return -1;
        }
    }
    else if(command_1 == "merge_all")
    {
        // 将一个文件夹下面的 .json 文件全部 merge 到一起
        if(argc == 4)
        {
            std::string save_path   = argv[2];
            std::string ucd_dir     = argv[3];
            std::vector<std::string> ucd_path_vector;

            if(! is_read_dir(ucd_dir))
            {
                std::cout << ERROR_COLOR << "ucd folder don't have access : read : " << ucd_dir << STOP_COLOR << std::endl;
                return 0;
            }

            ucd_path_vector = get_all_file_path(ucd_dir, {".json"});
            ucd_util->merge_ucds(save_path, ucd_path_vector);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "minus_obj")
    {
        if(argc == 5)
        {
            std::string ucd_path_1      = argv[2];
            std::string ucd_path_2      = argv[3];
            std::string ucd_save_path   = argv[4];
            ucd_util->ucd_minus_obj(ucd_save_path, ucd_path_1, ucd_path_2);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "minus_uc")
    {
        if(argc == 5)
        {
            std::string ucd_path_1      = argv[2];
            std::string ucd_path_2      = argv[3];
            std::string ucd_save_path   = argv[4];
            ucd_util->ucd_minus_uc(ucd_save_path, ucd_path_1, ucd_path_2);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "interset")
    {
        // 交集
        if(argc == 5)
        {
            std::string ucd_path_a  = argv[4];
            std::string ucd_path_b  = argv[3];
            std::string save_path   = argv[2];
            ucd_util->interset_ucds(save_path, ucd_path_a, ucd_path_b);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "meta")
    {
        if(argc == 2)
        {
            std::cout << "-----------------------------" << std::endl;
            std::cout << std::setw(15) << "ucd " << app_version << std::endl;
            std::cout << "-----------------------------" << std::endl;
            std::cout << HIGHTLIGHT_COLOR << "[cache]" << STOP_COLOR << std::endl;
            std::cout << "dir           : " << cache_dir << std::endl;
            std::cout << HIGHTLIGHT_COLOR << "[uci]" << STOP_COLOR   << std::endl;
            std::cout << "volume_size   : " << volume_size << std::endl;
            std::cout << HIGHTLIGHT_COLOR << "[server]"  << STOP_COLOR << std::endl;
            std::cout << "host          : " << host << std::endl;
            std::cout << "port          : " << port << std::endl;
            std::cout << "app_dir       : " << app_dir << std::endl;
            std::cout << "config_path   : " << config_path << std::endl;
            std::cout << "history_path  : " << history_path << std::endl;
            std::cout << "castration_function  : " << castration_function << std::endl;
            std::cout << HIGHTLIGHT_COLOR << "[sql]" << STOP_COLOR << std::endl;
            std::cout << "host          : " << sql_host << std::endl;
            std::cout << "port          : " << sql_port << std::endl;
            std::cout << "user          : " << sql_user << std::endl;
            std::cout << "pwd           : " << sql_pwd << std::endl;
            std::cout << "db            : " << sql_db << std::endl;
            std::cout << HIGHTLIGHT_COLOR << "[redis]" << STOP_COLOR << std::endl;
            std::cout << "host          : " << redis_host << std::endl;
            std::cout << "port          : " << redis_port << std::endl;
            std::cout << "name          : " << redis_name << std::endl;
            std::cout << HIGHTLIGHT_COLOR << "[milvus]" << STOP_COLOR << std::endl;
            std::cout << "host          : " << milvus_host << std::endl;
            std::cout << "port          : " << milvus_port << std::endl;
            std::cout << "-----------------------------" << std::endl;
            return -1;
        }
        else if(argc == 3)
        {
            std::string attr_name = argv[2];
            if(attr_name == "host") {std::cout << host << std::endl ;}
            else if(attr_name == "port") {std::cout << port << std::endl ;}
            else if(attr_name == "config_path") {std::cout << config_path << std::endl ;}
            else if(attr_name == "sql_host") {std::cout << sql_host << std::endl ;}
            else if(attr_name == "sql_port") {std::cout << sql_port << std::endl ;}
            else if(attr_name == "sql_user") {std::cout << sql_user << std::endl ;}
            else if(attr_name == "sql_pwd") {std::cout << sql_pwd << std::endl ;}
            else if(attr_name == "sql_db") {std::cout << sql_db << std::endl ;}
            else if(attr_name == "cache_dir") {std::cout << cache_dir << std::endl ;}
            else if(attr_name == "milvus_host") {std::cout << milvus_host << std::endl ;}
            else if(attr_name == "milvus_port") {std::cout << milvus_port << std::endl ;}
            else if(attr_name == "redis_host") {std::cout << redis_host << std::endl ;}
            else if(attr_name == "redis_port") {std::cout << redis_port << std::endl ;}
            else{std::cout << "no attr name : " << attr_name << std::endl; }
            return -1;
        }
        else 
        {
            ucd_param_opt->print_command_info("meta");
            return -1;
        }
    }
    else if(command_1 == "set")
    {
        if(argc == 4)
        {
            xini_file_t xini_write(config_path);
            std::string option = argv[2]; 

            if(option == "host")
            {
                std::string host_new = argv[3];
                xini_write["server"]["host"] = host_new;
            }
            else if(option == "app_dir")
            {
                std::string app_dir_new = argv[3];
                xini_write["server"]["app_dir"] = app_dir_new;      
            }
            else if(option == "port")
            {
                int port_new = std::stoi(argv[3]);
                xini_write["server"]["port"] = port_new;      
            }
            else if(option == "sql_host")
            {
                std::string sql_host_new = argv[3];
                xini_write["sql"]["host"] = sql_host_new;
            }
            else if(option == "sql_port")
            {
                int sql_port_new = std::stoi(argv[3]);
                xini_write["sql"]["port"] = sql_port_new;   
            }
            else if(option == "sql_user")
            {
                std::string sql_user_new = argv[3];
                xini_write["sql"]["user"] = sql_user_new;
            }
            else if(option == "sql_pwd")
            {
                std::string sql_pwd_new = argv[3];
                xini_write["sql"]["pwd"] = sql_pwd_new;
            }
            else if(option == "sql_db")
            {
                std::string sql_db_name_new = argv[3];
                xini_write["sql"]["db"] = sql_db_name_new;
            }
            else if(option == "redis_port")
            {
                std::string redis_port_str = argv[3];
                xini_write["redis"]["port"] = std::stoi(redis_port_str);
            }
            else if(option == "redis_host")
            {
                std::string redis_host_str = argv[3];
                xini_write["redis"]["host"] = redis_host_str;
            }
            else if(option == "redis_name")
            {
                std::string redis_name_str = argv[3];
                xini_write["redis"]["name"] = redis_name_str;
            }
            else if(option == "cache_dir")
            {
                std::string cache_dir_new = argv[3];
                // 
                if(!is_dir(cache_dir_new))
                {
                    std::cout << "cache dir not exist" << std::endl;
                    throw "cache dir not exist";
                }
                else
                {
                    // 创建对应的文件夹
                    std::string img_cache_dir = cache_dir_new + "/" + "img_cache";
                    if(! is_dir(img_cache_dir))
                    {
                        create_folder(img_cache_dir);
                    }
                }
                xini_write["cache"]["dir"] = cache_dir_new;
                return 0;
            }
            else if(option == "milvus_host")
            {
                std::string redis_host_str = argv[3];
                xini_write["milvus"]["host"] = redis_host_str;
            }
            else if(option == "milvus_port")
            {
                std::string redis_port_str = argv[3];
                xini_write["milvus"]["port"] = std::stoi(redis_port_str);
            }
            else if(option == "castration_function")
            {
                std::string c_function = argv[3];
                xini_write["server"]["castration_function"] = c_function;
            }
            else
            {
                ucd_param_opt->print_command_info("set");
                return -1;
            }
            xini_write.dump(config_path);   
            return -1;
        }
        else
        {
            ucd_param_opt->print_command_info("set");
            return -1;
        }
    }
    else if(command_1 == "rename_img")
    {
        // 一次处理的图片的个数，目前用不到
        int buffer_img_size = 100;
        if(long_args.count("buffer_img_size") > 0)
        {
            buffer_img_size = std::stoi(long_args["check_uc"]);
        }
        // 是否强制根据 md5 得到 uc
        bool check_uc = true;
        if(short_args.count("c") > 0)
        {
            check_uc = true;
        }
        else
        {
            check_uc = false;
        }

        if(argc == 3)
        {
            std::string img_dir = argv[2];
            if(is_dir(img_dir))
            {
                SaturnDatabaseSQL *sd_sql = new SaturnDatabaseSQL(sql_host, sql_port, sql_user, sql_pwd, sql_db);
                sd_sql->rename_img_dir(img_dir, 100, check_uc);
                delete sd_sql;
            }
            else
            {
                std::cout << ERROR_COLOR << "image dir not exists : " << img_dir << STOP_COLOR << std::endl;
                ucd_param_opt->print_command_info("rename_img");
                return -1;
            }
        }
        else
        {
            ucd_param_opt->print_command_info("rename_img");
            return -1;
        }
    }
    else if(command_1 == "rename_img_xml")
    {
        bool check_uc = true;
        if(short_args.count("c") > 0)
        {
            check_uc = true;
        }
        else
        {
            check_uc = false;
        }
        if(argc == 4)
        {
            std::string img_dir = argv[2];
            std::string xml_dir = argv[3];
            SaturnDatabaseSQL *sd_sql = new SaturnDatabaseSQL(sql_host, sql_port, sql_user, sql_pwd, sql_db);
            sd_sql->rename_img_xml_dir(img_dir, xml_dir, 100, check_uc);
            delete sd_sql;
        }
        else
        {
            ucd_param_opt->print_command_info("rename_img_xml");
            return -1;
        }
    }
    else if(command_1 == "rename_img_json")
    {
        
        bool check_uc = true;
        if(short_args.count("c") > 0)
        {
            check_uc = true;
        }
        else
        {
            check_uc = false;
        }

        if(argc == 4)
        {
            std::string img_dir = argv[2];
            std::string json_dir = argv[3];
            SaturnDatabaseSQL *sd_sql = new SaturnDatabaseSQL(sql_host, sql_port, sql_user, sql_pwd, sql_db);
            sd_sql->rename_img_json_dir(img_dir, json_dir, 100, check_uc);
            delete sd_sql;
        }
        else
        {
            ucd_param_opt->print_command_info("rename_img_json");
            return -1;
        }
    }
    else if(command_1 == "count_uc_by_tags")
    {
         if(argc == 3)
         {
            std::string ucd_path = argv[2];
            if(ucd_util->is_ucd_path(ucd_path))
            {
                ucd_util->count_uc_by_tags(ucd_path);
            }
            else
            {
                std::cout << ERROR_COLOR << argv[2] << " is not json file " << STOP_COLOR << std::endl;
            }
         }
         else
         {
            ucd_param_opt->print_command_info("count_uc_by_tags");
            return -1; 
         }
    }
    else if(command_1 == "count_tags")
    {
        if(argc== 3)
        {
            std::string xml_dir = argv[2];
            if(ucd_util->is_ucd_path(xml_dir))
            {
                ucd_util->count_ucd_tags(xml_dir);
            }
            else if(ucd_util->is_uci_path(xml_dir))
            {
                ucd_util->count_volume_tags(xml_dir);
            }
            else if(is_dir(xml_dir))
            {
                count_tags(xml_dir);
            }
            else
            {
                std::cout << "ucd_path not exists or xml dir not exists : " << xml_dir << std::endl;
                ucd_param_opt->print_command_info("count_tags");
                return -1;
            }
        }
        else
        {
            ucd_param_opt->print_command_info("count_tags");
            return -1;
        }
    }
    else if(command_1 == "count_files")
    {
        if ((argc==3) || (argc ==4))
        {
            std::string xml_dir     = argv[2];
            bool is_recursive       = true;

            if(argc ==4)
            {
                std::string recursive   = argv[3];
                if((recursive == "false") || (recursive == "False") || (recursive == "0"))
                {
                    is_recursive = false;
                }            
            }
            count_files(xml_dir, is_recursive);
        }
        else
        {
            ucd_param_opt->print_command_info("count_files");
            return -1;
        }
    }
    else if(command_1 == "count_folder")
    {
        // 查看文件夹中包含的文件夹中存储的信息，对每一个文件夹使用 count_files
        // 查看每一个文件夹的大小

        // du -sh * 查看当前目录下的所有文件夹的大小
        // 


    }
    else if(command_1 == "cut_small_img")
    {
        // 低缓存模式
        bool no_cache = false;
        if(long_args.count("no_cache") > 0)
        {
            std::string no_cache_str = long_args["no_cache"];
            if(no_cache_str == "1" || no_cache_str == "True" || no_cache_str == "true")
            {
                no_cache = true;
            }
        }

        bool is_split = true;
        if(short_args.count("s") != 0)
        {
            is_split = false;
        }

        if (argc == 4)
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];
            ucd_util->cut_small_img(ucd_path, save_dir, is_split, no_cache);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "crop_to_xml")
    {
        if (argc== 4)
        {
            std::string crop_dir = argv[2];
            std::string save_dir = argv[3];
            jotools::get_xml_from_crop_img(crop_dir, save_dir);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "crop_to_xml_with_origin_tag")
    {
        if (argc== 4)
        {
            std::string crop_dir = argv[2];
            std::string save_dir = argv[3];
            jotools::get_xml_from_crop_img_use_origin_tag(crop_dir, save_dir);
        }
        else
        {
            ucd_param_opt->print_command_info("crop_to_xml_with_origin_tag");
            return -1;
        }  
    }
    else if(command_1 == "xml_check")
    {
        ucd_param_opt->not_ready(command_1);
        return -1;

        if ((argc == 6))
        {
            std::string xml_dir = argv[2];
            std::string img_dir = argv[3];
            int size_th = std::stoi(argv[4]);
            std::string remove_error_path = argv[5];
            
            bool remove_error = true;
            if((remove_error_path != "true") && (remove_error_path != "True") && (remove_error_path != "1"))
            {
                remove_error = false;
            }
            xml_check(xml_dir, img_dir, size_th, remove_error);
        }
        else
        {
            ucd_param_opt->print_command_info("xml_check");
            return -1;
        }
    }
    else if(command_1 == "say")
    {
        if((argc == 3) || (argc == 4) || (argc == 5))
        {
            std::string words = argv[2];
            int height, width;
            if(argc == 3)
            {
                height = 30;
                width = 30;
            }
            else if(argc == 4)
            {
                height = std::stoi(argv[3]);
                width = std::stoi(argv[3]);
            }
            else
            {
                height = std::stoi(argv[3]);
                width = std::stoi(argv[4]);
            }
            ucd_util->print_words(words, height, width);
            return -1;
        }
        else
        {
            ucd_param_opt->print_command_info("say");
        }
    }
    else if(command_1 == "cache_info")
    {
        if(! is_dir(ucd_util->cache_img_dir))
        {
            std::cout << "cache dir is not exists" << std::endl;
            return -1;
        }

        int all_cache_img_count = 0;
        int exist_cache_img_count = 0;
        std::string cache_info, json_path;
        if((argc == 2) || (argc == 3))
        {
            if(argc == 3)
            {
                json_path = argv[2];
                if((! is_file(json_path)) || (json_path.substr(json_path.size()-5, json_path.size()) != ".json"))
                {
                    std::cout << "ucd path not exists : " << json_path << std::endl;
                    return -1;
                }

                UCDataset* ucd = new UCDataset(json_path);
                ucd->parse_ucd();
                std::string each_img_path;
                float exist_ratio;
                std::set<std::string> suffix {".jpg", ".JPG", ".png", ".PNG"};
                // 
                for(int i=0; i<ucd->uc_list.size(); i++)
                {
                    each_img_path = get_file_by_suffix_set(ucd_util->cache_img_dir, ucd->uc_list[i], suffix);
                    if(each_img_path != "")
                    {
                        exist_cache_img_count++;
                    }
                }
                exist_ratio = (float)exist_cache_img_count / (float)ucd->uc_list.size();
                std::cout << "----------------------------------" << std::endl;
                std::cout << "all img count         : " << ucd->uc_list.size() << std::endl;
                std::cout << "exist cache img count : " << exist_cache_img_count << std::endl;
                std::cout << "exist ratio           : " << exist_ratio * 100 << " %" << std::endl;
                std::cout << "----------------------------------" << std::endl;
                delete ucd;
            }
            else
            {
                // 没有参数，查看基础情况
                std::set<std::string> suffix {".jpg", ".JPG", ".png", ".PNG"};
                std::vector<std::string> img_path_vector;
                img_path_vector = get_all_file_path(ucd_util->cache_img_dir, suffix);
                all_cache_img_count = img_path_vector.size();
                std::cout << "----------------------------------" << std::endl;
                std::cout << "all cache img count   : " << all_cache_img_count << std::endl;
                std::cout << "----------------------------------" << std::endl;
            }
        }
        else
        {
            ucd_param_opt->print_command_info("cache_info");
        }
    }
    else if(command_1 == "cache_clear")
    {
        if(! is_dir(ucd_util->cache_img_dir))
        {
            std::cout << "cache dir is not exists" << std::endl;
            return -1;
        }

        if((argc == 2) || (argc == 3))
        {
            // 确认是否删除
            char confirm;
            std::cout << "Would clear cache:" << std::endl;
            std::cout << "Proceed (y/n)? ";
            std::cin >> confirm;
            if(confirm != 'y')
            {
                std::cout << "clear canceled" << std::endl;
                return -1;
            }
            // 删除 ucd 包含的所有图片
            if(argc == 3)
            {
                bool reversal = false;
                if(short_args.count("r") > 0)
                {
                    reversal = true;
                }

                std::string ucd_path = argv[2];
                if(! ucd_util->is_ucd_path(ucd_path))
                {
                    std::cout << "ucd path not exists : " << ucd_path << std::endl;
                    throw "ucd path not exists";
                }
                else
                {
                    ucd_util->cache_clear(ucd_path, reversal);
                }
            }
            // 删除所有的文件
            else
            {
                ucd_util->cache_clear();
            }
        }
        else
        {
            ucd_param_opt->print_command_info("cache_clear");
        }
    }
    else if(command_1 == "cache_clean")
    {
        // 删除图片缓存中大小为 0 的空图片
        if(argc == 2)
        {
            ucd_util->cache_clean(ucd_util->cache_img_dir);
        }
        else if(argc == 3)
        {
            std::string clean_dir = argv[2];
            ucd_util->cache_clean(clean_dir);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "cache_from")
    {
        // 还是不要实现这个功能了吧，还是直接从服务器上下载比较靠谱
    }
    else if(command_1 == "acc")
    {
        if(argc == 5 || argc == 4)
        {
            std::string ucd_customer = argv[2];
            std::string ucd_standard = argv[3];
            std::string ucd_save_res;
            if(argc == 4)
            {
                ucd_save_res = "";
            }
            else
            {
                ucd_save_res = argv[4];
            }

            // 
            if(! (is_file(ucd_customer) && is_file(ucd_standard)))
            {
                std::cout << "ucd path not exists " << std::endl;
                throw "ucd path not exists";
            }

            UCDataset* ucd_a = new UCDataset(ucd_customer);
            UCDataset* ucd_b = new UCDataset(ucd_standard);
            ucd_a->parse_ucd(true);
            ucd_b->parse_ucd(true);

            jotools::DeteAcc* acc = new DeteAcc();

            if(long_args.count("iou") > 0)
            {
                float iou = std::stof(long_args["iou"]);
                acc->iou = iou;
            }
            else
            {
                acc->iou = 0.5;
            }

            acc->cal_acc_rec(ucd_a, ucd_b, ucd_save_res);
            delete acc;
            delete ucd_a;
            delete ucd_b;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "map")
    {

        std::cout << WARNNING_COLOR << "结果存在问题，仅供参考" << STOP_COLOR << std::endl;

        if((argc == 4) || (argc == 5))
        {
            std::string ucd_customer = argv[2];
            std::string ucd_standard = argv[3];
            jotools::DeteAcc* acc = new DeteAcc();

            // assign iou
            if(long_args.count("iou") > 0)
            {
                float iou = std::stof(long_args["iou"]);
                acc->iou = iou;
            }
            else
            {
                acc->iou = 0.5;
            }

            if(! (is_file(ucd_customer) && is_file(ucd_standard)))
            {
                std::cout << "ucd path not exists " << std::endl;
                throw "ucd path not exists";
            }

            UCDataset* ucd_a = new UCDataset(ucd_customer);
            UCDataset* ucd_b = new UCDataset(ucd_standard);
            ucd_a->parse_ucd(true);
            ucd_b->parse_ucd(true);

            if(argc == 5)
            {
                std::string save_path = argv[4];
                acc->cal_map(ucd_a, ucd_b, save_path);
            }
            else
            {
                acc->cal_map(ucd_a, ucd_b);
            }

            delete ucd_a;
            delete ucd_b;
            delete acc;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "to_yolo_txt")
    {
        if(argc == 4 || argc == 5)
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];
            if(argc == 4)
            {
                ucd_util->parse_yolo_train_data(ucd_util->cache_img_dir, save_dir, ucd_path);
            }
            else
            {
                std::vector<std::string> label_list;
                std::string label_list_str = argv[4];
                label_list = pystring::split(label_list_str, ",");
                ucd_util->parse_yolo_train_data(ucd_util->cache_img_dir, save_dir, ucd_path, label_list);
            }
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "search_similar")
    {
        if(argc == 3 || argc == 4)
        {
            int similar_count = 10;
            std::string save_path = "";
            std::string img_path = argv[2];

            if(argc == 4)
            {
                save_path = argv[3];
            }

            if(long_args.count("limit") != 0)
            {
                similar_count = std::stof(long_args["limit"]);
            }

            ucd_util->search_similar_uc(img_path, similar_count, save_path, milvus_host, milvus_port);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "query")
    {
        if(argc == 3)
        {

            int max_tokens = 1000;
            std::string system_content = "你是一个优秀的助手，能用简洁但是明确的回答帮助询问者,通常你用一句话就能解决提问者的疑惑";
            float threshold = 0.1;
            bool search_database = true;

            if(long_args.count("max_tokens") != 0)
            {
                max_tokens = std::stof(long_args["max_tokens"]);
            }

            if(short_args.count("l") != 0)
            {
                system_content = "你是一个优秀的助手,特别善于解决图像检测和声音信息处理方面的问题";
            }

            if(long_args.count("system_content") != 0)
            {
                system_content = long_args["system_content"];
            }

            if(long_args.count("threshold") != 0)
            {
                threshold = std::stof(long_args["threshold"]);
            }

            if(short_args.count("f") != 0)
            {
                search_database = false;
            }
            std::string query = argv[2];

            ucd_util->query_to_database_and_chartgpt(query, system_content, max_tokens, threshold, search_database);
            return 1;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "to_img")
    {
        if(argc == 4)
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];
            UCDataset *ucd = new UCDataset(ucd_path);
            ucd->parse_ucd();
            ucd_util->load_img(save_dir, ucd->uc_list);
            delete ucd;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "to_crop")
    {
        // 低缓存模式
        bool no_cache = false;
        if(long_args.count("no_cache") > 0)
        {
            std::string no_cache_str = long_args["no_cache"];
            if(no_cache_str == "1" || no_cache_str == "True" || no_cache_str == "true")
            {
                no_cache = true;
            }
        }

        bool is_split = true;
        if(short_args.count("s") != 0)
        {
            is_split = false;
        }

        bool split_by_conf = false;
        if(short_args.count("c") > 0)
        {
            split_by_conf = true;
        }

        if (argc == 4)
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];
            ucd_util->cut_small_img(ucd_path, save_dir, is_split, no_cache, split_by_conf);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "to_labelme_json")
    {
        if(argc == 4)
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];

            if(! is_file(ucd_path))
            {
                std::cout << "ucd_path not exists : " << ucd_path << std::endl;
                throw "ucd_path not exists";
            }
            ucd_util->parse_labelme_json(ucd_util->cache_img_dir, save_dir, ucd_path);
        }
        else
        {
            ucd_param_opt->print_command_info("to_labelme_json");
        }
    }
    else if(command_1 == "to_assign_crop_xml")
    {
        float iou_th = 0.85;
        if(long_args.count("iou_th") > 0)
        {
            iou_th = std::stof(long_args["iou_th"]);
        }

        if(argc == 5)
        {
            std::string ucd_path    = argv[2];
            std::string save_dir    = argv[3];
            std::string assign_tag  = argv[4];
            ucd_util->save_assign_range(ucd_path, save_dir, assign_tag, iou_th, "xml");
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "to_assign_crop_txt")
    {
        float iou_th = 0.85;
        if(long_args.count("iou_th") > 0)
        {
            iou_th = std::stof(long_args["iou_th"]);
        }

        if(argc == 5)
        {
            std::string ucd_path    = argv[2];
            std::string save_dir    = argv[3];
            std::string assign_tag  = argv[4];
            ucd_util->save_assign_range(ucd_path, save_dir, assign_tag, iou_th, "txt");
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "to_xml")
    {
        if(argc == 4)
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];
            ucd_util->parse_voc_xml(ucd_util->cache_img_dir, save_dir, ucd_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "to_yolo_train_data")
    {
        float ratio = 0.8;
        if(long_args.count("ratio") != 0)
        {
            ratio = std::stof(long_args["ratio"]);
        }

        float iou_th = 0.85;
        if(long_args.count("iou_th") != 0)
        {
            iou_th = std::stof(long_args["iou_th"]);
        }
 
        std::string assign_tag = "";
        if(long_args.count("assign_tag") != 0)
        {
            assign_tag = long_args["assign_tag"];
        }
 
        std::string tag_str = "";
        if(long_args.count("tags") != 0)
        {
            tag_str = long_args["tags"];
        }

        if(argc == 4)
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];
            if(ucd_util->is_ucd_path(ucd_path))
            {
                create_folder(save_dir);
                if(is_write_dir(save_dir))
                {
                    if(assign_tag == "")
                    {
                        ucd_util->save_to_yolo_detect_train_data(ucd_path, save_dir, tag_str, ratio);
                    }
                    else
                    {
                        ucd_util->save_to_yolo_train_data_with_assign_range(ucd_path, save_dir, tag_str, assign_tag, ratio, iou_th);
                    }
                }
                else
                {
                    std::cout << ERROR_COLOR << "WARNING : save folder don't have write access : " << save_dir << STOP_COLOR << std::endl;
                    return -1; 
                }
            }
            else
            {
                std::cout << "ucd path not exists : " << ucd_path << std::endl;
                throw "ucd path not exists";
            }
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "to_yolo_classify_train_data")
    {
        float ratio = 0.8;
        if(long_args.count("ratio") != 0)
        {
            ratio = std::stof(long_args["ratio"]);
        }

        std::string tag_str = "";
        if(long_args.count("tags") != 0)
        {
            tag_str = long_args["tags"];
        }

        if(argc == 4)
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];
            if(ucd_util->is_ucd_path(ucd_path))
            {
                create_folder(save_dir);
                if(is_write_dir(save_dir))
                {

                    ucd_util->save_to_yolo_classify_train_data(ucd_path, save_dir, tag_str, ratio);

                }
                else
                {
                    std::cout << ERROR_COLOR << "WARNING : save folder don't have write access : " << save_dir << STOP_COLOR << std::endl;
                    return -1; 
                }
            }
            else
            {
                std::cout << "ucd path not exists : " << ucd_path << std::endl;
                throw "ucd path not exists";
            }
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "attr")
    {
        // 修改 ucd 文件的属性信息，因为对于比较大的 ucd 打开自己去修改非常麻烦
        if(argc == 5)
        {
            std::string ucd_path = argv[2];
            if(ucd_util->is_ucd_path(ucd_path))
            {
                std::string attr_name = argv[3];
                std::string attr_value = argv[4];
                UCDataset * ucd = new UCDataset(ucd_path);
                ucd->parse_ucd(true);
                ucd->update_time = getPythonStyleTimestamp();
                ucd->change_attar(attr_name, attr_value);
                delete ucd;
            }
            else
            {
                std::cout << "ucd path not exists : " << ucd_path << std::endl;
                throw "ucd path not exists";
            }
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "help")
    {
        if (argc == 3)
        {
            std::string command = argv[2];
            if(ucd_param_opt->has_command(command))
            {
                ucd_param_opt->print_command_info(command);
            }
            else if(ucd_param_opt->has_group(command))
            {
                ucd_param_opt->print_group_info(command);
            }
            else
            {
                if(ucd_param_opt->has_simliar_command(command_1))
                {
                    ucd_param_opt->print_similar_command_info(command);
                }
            }
            return -1;
        }
        else if(argc ==2)
        {
            if(castration_function == "")
            {
                ucd_param_opt->print_all_fun_chinese();
            }
            else
            {
                ucd_param_opt->print_castration_fun_chinese(castration_function);
            }
        }
        else
        {
            ucd_param_opt->print_command_info("help");
            return -1;
        }
    }
    else if(command_1 == "uc_check")
    {
        if(argc == 3)
        {
            std::string file_dir = argv[2];

            if(! is_dir(file_dir))
            {
                std::cout << "file_dir not exists : " << file_dir << std::endl;
                return -1;
            }

            std::vector<string> uc_vector;
            std::set<std::string> file_suffix {".jpg", ".JPG", ".png", ".PNG", ".json", ".xml"}; 
            std::vector<std::string> file_vector = get_all_file_path_recursive(file_dir, file_suffix);
        
            for(int i=0; i<file_vector.size(); i++)
            {
                std::string uc = get_file_name(file_vector[i]);
                uc_vector.push_back(uc);
            }

            SaturnDatabaseSQL *sd_sql = new SaturnDatabaseSQL(sql_host, sql_port, sql_user, sql_pwd, sql_db);
            std::map<std::string, bool> is_uc_map = sd_sql->check_uc_by_mysql(uc_vector);

            int is_uc = 0;
            int not_uc = 0;
            auto iter = is_uc_map.begin();
            while(iter != is_uc_map.end())
            {
                if(iter->second)
                {
                    is_uc += 1;
                }
                else
                {
                    not_uc += 1;
                }
                iter ++;
            }

            std::cout << "------------------------" << std::endl;
            std::cout << "is  uc count : " << is_uc << std::endl;
            std::cout << "not uc count : " << not_uc << std::endl;
            std::cout << "check name format for (.jpg, .JPG, .png, .PNG. .xml, .json) in sql " << std::endl;
            std::cout << "------------------------" << std::endl;
            delete sd_sql;
        }
        else
        {
            ucd_param_opt->print_command_info("uc_check");
            return -1;
        }        
    }
    else if(command_1 == "move_uc" || command_1 == "move_not_uc")
    {

        // the same name not different suffix 
        
        bool is_uc;
        if(command_1 == "move_uc")
        {
            is_uc = true;
        }
        else
        {
            is_uc = false;
        }

        if(argc == 4)
        {
            std::string file_dir = argv[2];
            std::string save_dir = argv[3];

            if(! is_dir(file_dir))
            {
                std::cout << "file_dir not exists : " << file_dir << std::endl;
                return -1;
            }

            std::vector<string> uc_vector;
            std::vector<std::string> file_vector = get_all_file_path_recursive(file_dir);

            for(int i=0; i<file_vector.size(); i++)
            {
                std::string uc = get_file_name(file_vector[i]);
                uc_vector.push_back(uc);
            }

            SaturnDatabaseSQL *sd_sql = new SaturnDatabaseSQL(sql_host, sql_port, sql_user, sql_pwd, sql_db);
            std::map<std::string, bool> is_uc_map = sd_sql->check_uc_by_mysql(uc_vector);
            delete sd_sql;

            std::vector<std::string> move_file_vector;
            for(int i=0; i<file_vector.size(); i++)
            {
                std::string file_name = get_file_name(file_vector[i]);
                if(is_uc_map[file_name] && is_uc)
                {
                    move_file_vector.push_back(file_vector[i]);
                }
                else if((! is_uc_map[file_name]) && (! is_uc))
                {
                    move_file_vector.push_back(file_vector[i]);
                }
            }
            move_file_vector_to_dir(move_file_vector, save_dir);
        }
        else
        {
            if(is_uc)
            {
                ucd_param_opt->print_command_info("move_uc");
            }
            else
            {
                ucd_param_opt->print_command_info("move_not_uc");
            }
            return -1;
        }   
    }
    else if(command_1 == "filter_by_nms")
    {
        if(argc == 6)
        {
            std::string ucd_path = argv[2];
            std::string ucd_save_path = argv[3];
            float nms_th = std::stof(argv[4]);
            std::string ignore_tag_str = argv[5];
            bool ignore_tag = true;

            if((ignore_tag_str != "true") && (ignore_tag_str != "True") && (ignore_tag_str != "1"))
            {
                ignore_tag = false;
            }

            //
            if(! is_file(ucd_path))
            {
                std::cout << "ucd_path not exists : " << ucd_path;
                throw "ucd_path not exists";
            }

            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->filter_by_nms(nms_th, ignore_tag);
            ucd->save_to_ucd(ucd_save_path);
            delete ucd; 
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "filter_by_conf")
    {
        if(argc == 5)
        {
            std::string ucd_path = argv[2];
            std::string ucd_save_path = argv[3];
            float conf_th = std::stof(argv[4]);

            //
            if(! is_file(ucd_path))
            {
                std::cout << "ucd_path not exists : " << ucd_path;
                throw "ucd_path not exists";
            }

            std::cout << ucd_path << std::endl;

            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->filter_by_conf(conf_th);
            ucd->save_to_ucd(ucd_save_path);
            delete ucd; 
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "filter_by_area")
    {
        if(argc == 5)
        {
            std::string ucd_path = argv[2];
            std::string ucd_save_path = argv[3];
            float area_th = std::stof(argv[4]);

            //
            if(! is_file(ucd_path))
            {
                std::cout << "ucd_path not exists : " << ucd_path;
                throw "ucd_path not exists";
            }

            std::cout << ucd_path << std::endl;
            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->filter_by_area(area_th);
            ucd->save_to_ucd(ucd_save_path);
            delete ucd; 
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "filter_by_uc")
    {
        if(argc > 4)
        {
            std::string ucd_path = argv[2];
            std::string save_path = argv[3];
            std::set<std::string> uc_set;

            if(! is_read_file(ucd_path))
            {
                std::cout << ERROR_COLOR << "ucd path not readable : " << ucd_path << STOP_COLOR << std::endl;
                return -1;
            }

            if(! pystring::endswith(ucd_path, ".json"))
            {
                std::cout << ERROR_COLOR << "illegal ucd path : " << ucd_path << STOP_COLOR << std::endl;
                return -1;
            }

            for(int i=4; i<argc; i++)
            {
                std::string uc = argv[i];
                uc_set.insert(uc);
            }

            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->filter_by_uc_set(uc_set, true);
            ucd->save_to_ucd(save_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "filter_by_tags")
    {

        // 增加模式， or 和 and 模式
        // 
        std::string mode = "or";
        if(short_args.count("a") > 0)
        {
            mode = "and";
        }

        if(argc > 4)
        {
            // std::cout << HIGHTLIGHT_COLOR << "使用通配符匹配的时候，某些机器会存在无法匹配的问题（特别是a 开头的 tag），使用完函数，最好手工核对一下" << STOP_COLOR << std::endl;
            std::string ucd_path = argv[2];
            std::string save_path = argv[3];
            std::set<std::string> tags;
            for(int i=4; i<argc; i++)
            {
                std::string each_tag = argv[i];
                tags.insert(each_tag);
            }
            if(ucd_util->is_ucd_path(ucd_path))
            {
                UCDataset* ucd = new UCDataset(ucd_path);
                ucd->parse_ucd(true);
                ucd->filter_by_tags(tags, mode);
                ucd->save_to_ucd(save_path);
                delete ucd;
            }
            else if(ucd_util->is_uci_path(ucd_path))
            {
                std::cout << WARNNING_COLOR << "filter uci use 'filter_volume_by_tags' " << STOP_COLOR << std::endl;
            }
            else
            {
                std::cout << ERROR_COLOR << "illeagal ucd_path and uci_path : " << ucd_path << STOP_COLOR << std::endl;
                return -1;
            }
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "filter_by_date")
    {
        // 根据时间进行筛选，只需要指定时间的，时间之间使用逗号分隔

        std::string gt = "";
        std::string lt = "";
        std::string eq = "";

        if(long_args.count("gt") > 0)
        {
            gt = long_args["gt"];
        }

        if(long_args.count("lt") > 0)
        {
            lt = long_args["lt"];
        }

        if(argc == 5)
        {
            eq = argv[4];
        }

        if(argc == 5)
        {
            std::string ucd_path = argv[2];
            std::string save_path = argv[3];
            std::string assign_date_str = argv[4];
            std::vector<std::string> assign_date = pystring::split(assign_date_str, ",");

            if(! is_read_file(ucd_path))
            {
                std::cout << ERROR_COLOR << "ucd path is not readable : " << ucd_path << STOP_COLOR << std::endl;
                return -1; 
            }

            UCDataset *ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            // TODO 增加 method 选项
            ucd->filter_by_date(assign_date, true, "eq");
            ucd->save_to_ucd(save_path);
            delete ucd;
            return 1;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "filter_volume_by_tags")
    {
        if(argc > 5)
        {
            std::string ucd_path = argv[2];
            std::string save_path = argv[3];
            std::string volume_size_str = argv[4];
            int volume_size = std::stoi(volume_size_str);

            if(! pystring::endswith(save_path, ".uci"))
            {
                std::cout << ERROR_COLOR << "illegal uci path : " << save_path << STOP_COLOR << std::endl;
                return -1;
            }

            std::set<std::string> tags;
            for(int i=5; i<argc; i++)
            {
                std::string each_tag = argv[i];
                tags.insert(each_tag);
            }

            std::string save_dir = get_file_folder(save_path);
            std::string save_name = get_file_name(save_path);
            ucd_util->filter_by_tags_volume(tags, ucd_path, save_dir, save_name, volume_size);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "keep_only_uc")
    {
        // 去掉除 uc 之外的所有信息
        if(argc == 4)
        {
            std::string ucd_path    = argv[2];
            std::string save_path   = argv[3];
            
            UCDataset *ucd = new UCDataset(ucd_path);
            UCDataset *ucd_new = new UCDataset();
            ucd->parse_ucd(false);
            ucd_new->uc_list = ucd->uc_list;
            ucd_new->save_to_ucd(save_path);
            delete ucd;
            delete ucd_new;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "uc_analysis")
    {
        if(argc == 3)
        {
            std::string ucd_path = argv[2];
            ucd_util->uc_analysis(ucd_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "conf_analysis")
    {
        // ucd 的置信度分布
        if(argc == 3)
        {
            std::string ucd_path = argv[2];
            ucd_util->conf_analysis(ucd_path, 10);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "area_analysis")
    {
        // ucd_param_opt->not_ready(command_1);
        // return -1;

        if(argc == 4)
        {
            std::string ucd_path = argv[2];
            int seg_count = std::stoi(argv[3]);
            ucd_util->area_analysis(ucd_path, seg_count);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "server_info")
    {

        ucd_param_opt->not_ready();
        return -1;

        // // check if sshpass is installed, apt list --installed | grep sshpass/focal
        // // c++ run bash 
        // // use sshpass -p txkj ssh txkj@192.168.3.101 "nvidia-smi"
        // // ucd server_info 221 101 
        // // drive | gpu type | free space (gpu)

        // std::system("echo -n '221 : '  && nvidia-smi | sed -n 3p | awk '{print $4, $5, $6}'"); // 执行 UNIX 命令 "ls -l >test.txt"
        
        // std::system("echo -n '200 : '  && sshpass -p txkj2020 ssh txkj@192.168.3.200 nvidia-smi | sed -n 3p | awk '{print $4, $5, $6}'"); // 执行 UNIX 命令 "ls -l >test.txt"
        
        // std::system("echo -n '155 : '  && sshpass -p ldq ssh ldq@192.168.3.155 nvidia-smi | sed -n 3p | awk '{print $4, $5, $6}'"); // 执行 UNIX 命令 "ls -l >test.txt"
        
        // std::system("echo -n '21  : '  && sshpass -p ldq ssh ldq@192.168.3.21 nvidia-smi | sed -n 3p | awk '{print $4, $5, $6}'"); // 执行 UNIX 命令 "ls -l >test.txt"
        
        // std::system("echo -n '101 : '  && sshpass -p txkj ssh txkj@192.168.3.101 nvidia-smi | sed -n 3p | awk '{print $4, $5, $6}'"); // 执行 UNIX 命令 "ls -l >test.txt"
        
        // std::system("echo -n '132 : '  && sshpass -p txkj ssh txkj@192.168.3.101 nvidia-smi | sed -n 3p | awk '{print $4, $5, $6}'"); // 执行 UNIX 命令 "ls -l >test.txt"
        
    }
    else if(command_1 == "get")
    {
        if(argc == 4)
        {
            std::string attr_name = argv[2];
            std::string ucd_path = argv[3];

            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            if(attr_name == "dataset_name")
            {
                std::cout << ucd->dataset_name << std::endl;
            }
            else if(attr_name == "model_name")
            {
                std::cout << ucd->model_name << std::endl;
            }
            else if(attr_name == "model_version")
            {
                std::cout << ucd->model_version << std::endl;     
            }
            else if(attr_name == "add_time")
            {
                std::cout << ucd->add_time << std::endl;     
            }
            else if(attr_name == "update_time")
            {
                std::cout << ucd->update_time << std::endl;     
            }
            else if(attr_name == "describe")
            {
                std::cout << ucd->describe << std::endl;     
            }
            else if(attr_name == "tags")
            {
                std::set<std::string> tags = ucd->get_tags();
                auto iter_tag = tags.begin();
                while(iter_tag != tags.end())
                {
                    std::cout << iter_tag->data() << ",";
                    iter_tag++;
                }
                std::cout << std::endl;
            }
            else if(attr_name == "label_used")
            {
                if(ucd->label_used.size() == 0)
                {
                    std::cout << " " << std::endl;
                    return -1;
                }

                for(int i=0; i<ucd->label_used.size()-1; i++)
                {
                    std::cout << ucd->label_used[i] << ",";
                }
                std::cout << ucd->label_used[ucd->label_used.size()-1] << std::endl;
            }
            else if(attr_name == "uc_list")
            {
                for(int i=0; i<ucd->uc_list.size()-1; i++)
                {
                    std::cout << ucd->uc_list[i] << " ";
                }
                std::cout << ucd->uc_list[ucd->uc_list.size()-1] << std::endl;
            }
            else if(attr_name == "uc_count")
            {
                std::cout << ucd->uc_list.size() << std::endl;     
            }
            else if(attr_name == "label_used_count")
            {
                std::cout << ucd->label_used.size() << std::endl;     
            }
            else
            {
                std::cout << "attr_name not in [tags, dataset_name, uc_count, label_used_count, model_name, model_version, add_time, update_time, describe, label_used, uc_list]" << std::endl;     
            }
            return -1;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);  
        }
    }
    else if(command_1 == "has_uc")
    {
        if(argc >= 4)
        {
            std::string ucd_path = argv[2];
            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd();

            std::cout << "------------------" << std::endl;
            std::cout << "   UC    : HAS UC " << std::endl;
            std::cout << "------------------" << std::endl;

            for(int i=3; i<argc; i++)
            {
                std::string uc = argv[i];
                if(ucd->has_uc(uc))
                {
                    std::cout << uc << "  :  " << "True" << std::endl;
                }
                else
                {
                    std::cout << ERROR_COLOR << uc << "  :  " << "False" << STOP_COLOR << std::endl;
                }
            }
            std::cout << "------------------" << std::endl;
            return -1;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "history")
    {

        int need_line       = 9999;
        bool need_unique    = false;    // 是否需要去重
        bool need_info      = false;    // 是否打印统计信息 


        if(long_args.count("line"))
        {
            need_line = std::stoi(long_args["line"]);
        }

        if(short_args.count("u") > 0)
        {
            need_unique = true;
        }

        if(short_args.count("i") > 0)
        {
            need_info = true;
        }

        if(argc == 2)
        {
            // make sure if history file exists
            if(! is_file(history_path))
            {
                std::cout << HIGHTLIGHT_COLOR << "cant find history file : " << history_path << STOP_COLOR << std::endl;
                return -1;
            }

            // parse txt
            std::ifstream txt_file;
            txt_file.open(history_path);
            assert(txt_file.is_open());

            std::string line;
            std::vector<std::string> txt_info;
            while(getline(txt_file, line))
            {
                txt_info.push_back(line);
            }

            // print history
            std::cout << "---------------------------------------------------------------------------" << std::endl;
            std::cout << "                                history" << std::endl;
            std::cout << "---------------------------------------------------------------------------" << std::endl;
            
            int line_count = txt_info.size();
            int start_line = std::max(line_count - need_line, 0);
            start_line = std::min(line_count, start_line);

            if(need_info == false)
            {
                std::string command_str_old = "";
                for(int i=start_line; i<line_count; i++)
                {
                    std::vector<std::string> line_info = pystring::split(txt_info[i], " :");
                    std::string time_str = line_info[0];
                    std::string command_str = txt_info[i].substr(time_str.size(), txt_info[i].size());
                    
                    if((need_unique == true) && (command_str_old == command_str))
                    {
                        continue;
                    }

                    std::cout << std::setw(6) << std::left << i << std::setw(21) << std::left << time_str << std::setw(40) << std::left << command_str << std::endl;
                    command_str_old = command_str;
                }
            }
            else
            {
                std::map< std::string, int > key_map;
                for(int i=start_line; i<line_count; i++)
                {
                    std::vector<std::string> line_info = pystring::split(txt_info[i], " :");
                    std::string time_str = line_info[0];
                    std::string command_str = txt_info[i].substr(time_str.size(), txt_info[i].size());
                    command_str = pystring::strip(command_str, " ");
                    std::vector<std::string> command_info = pystring::split(command_str, " ");
                    std::string command_key = "ucd";
                    if(command_info.size() >= 3)
                    {
                        command_key = command_info[2];                    
                    }

                    if(key_map.count(command_key) > 0)
                    {
                        key_map[command_key] += 1;
                    }
                    else
                    {
                        key_map[command_key] = 1;
                    }
                }

                // 打印统计结果
                auto iter = key_map.begin();
                while(iter != key_map.end())
                {
                    std::cout << std::setw(30) << std::left << iter->first << " : " << iter->second << std::endl;
                    iter++;
                }
            }

            std::cout << "---------------------------------------------------------------------------" << std::endl;

            // std::cout << config_path << std::endl;
            return 1;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1); 
        }
    }
    else if(command_1 == "fix_size_info")
    {
        bool no_cache = false;
        if(long_args.count("no_cache") > 0)
        {
            std::string no_cache_str = long_args["no_cache"];
            if(no_cache_str == "1" || no_cache_str == "True" || no_cache_str == "true")
            {
                no_cache = true;
            }
        }

        std::string size_ucd = "";
        if(long_args.count("size_ucd") > 0)
        {
            size_ucd = long_args["size_ucd"];
        }

        if(argc == 4)
        {
            std::string ucd_path    = argv[2];
            std::string save_path   = argv[3];
            ucd_util->fix_size_info(ucd_path, save_path, no_cache, size_ucd);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "drop")
    {
        if(argc == 5)
        {
            std::string attr_name = argv[2];
            std::string ucd_path = argv[3];
            std::string save_ucd_path = argv[4];

            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd();
            if(attr_name == "dataset_name")
            {
                ucd->dataset_name = "";
            }
            else if(attr_name == "model_name")
            {
                ucd->model_name = "";
            }
            else if(attr_name == "model_version")
            {
                ucd->model_version = "";    
            }
            else if(attr_name == "add_time")
            {
                ucd->add_time = -1;  
            }
            else if(attr_name == "update_time")
            {
                ucd->update_time = -1;
            }
            else if(attr_name == "describe")
            {
                ucd->describe = ""; 
            }
            else if(attr_name == "label_used")
            {
                ucd->label_used = {};
            }
            else if(attr_name == "uc_list")
            {
                ucd->uc_list = {};
            }
            else if(attr_name == "object_info")
            {
                ucd->object_info = {};
            }
            else if(attr_name == "size_info")
            {
                ucd->size_info = {};
            }
            else
            {
                std::cout << "attr_name not in [dataset_name, object_info, size_info, model_name, model_version, add_time, update_time, describe, label_used, uc_list]" << std::endl;     
            }
            ucd->update_time = getPythonStyleTimestamp();
            ucd->save_to_ucd(save_ucd_path);
            return -1;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);  
        }
    }
    else if(command_1 == "uc_info")
    {
        if(argc >= 4)
        {
            std::string ucd_path = argv[2];
            std::vector<std::string> uc_list;
            for(int i=3; i<argc; i++)
            {
                std::string each_uc = argv[i];
                uc_list.push_back(each_uc);
            }

            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);

            // uc_list -> uc_set
            set<std::string> uc_set(ucd->uc_list.begin(), ucd->uc_list.end());

            for(int i=0; i<uc_list.size(); i++)
            {
                std::string uc = uc_list[i];                
                
                std::cout << WARNNING_COLOR << "-----------------------------" << uc << "----------------------------" << STOP_COLOR << std::endl;
                
                if(uc_set.count(uc) == 0)
                {
                    std::cout << ERROR_COLOR << "uc not exists : " << uc << STOP_COLOR << std::endl;
                    continue;
                }

                // size_info
                std::cout << "-----------------------" << std::endl;
                if(ucd->size_info.count(uc) > 0)
                {
                    std::cout << "(w, h) : (" << ucd->size_info[uc][0] << ", " << ucd->size_info[uc][1] << ")" << std::endl;
                }
                else
                {
                    std::cout << "(w, h) : (" << "-1" << ", " << "-1" << ")" << std::endl;
                }
                std::cout << "-----------------------" << std::endl;

                // obj_info
                if(ucd->object_info.count(uc) > 0)
                {
                    for(int i=0; i<ucd->object_info[uc].size(); i++)
                    {
                        ucd->object_info[uc][i]->print_info();
                        std::cout << "-----------------------" << std::endl;
                    }
                }
                else
                {
                    std::cout << "obj : empty" << std::endl;
                }
            }


        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "sub")
    {
        if(argc == 6)
        {
            std::string ucd_path = argv[2];
            std::string save_path = argv[3];
            int count = std::stoi(argv[4]);
            std::string is_random_str = argv[5];
            bool is_random = true;
            if((is_random_str == "false") || (is_random_str == "False") || (is_random_str == "0"))
            {
                is_random = false;
            }
            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->get_sub_ucd(count, is_random, save_path);
            return -1;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "update")
    {
        if(! is_dir(app_dir))
        {
            std::cout << ERROR_COLOR << "app must in path : " << app_dir << ", no such folder"<< STOP_COLOR << std::endl;
            return -1;
        }
        
        if(argc == 3 || argc == 2)
        {
            std::string version;
    
            if(argc == 3)
            {
                version = argv[2];
            }
            else
            {
                std::string check_url = "http://" + host + ":" + std::to_string(port);
                httplib::Client cli(check_url);
                auto res = cli.Get("/ucd/ucd_version_list");
                
                if(res != nullptr)
                {
                    json data = json::parse(res->body);
                    
                    // customer
                    for(int i=0; i<data["ucd_version_info"].size(); i++)
                    {
                        std::cout << "  remote : " << data["ucd_version_info"][i] << std::endl;
                    }

                    int version_count = data["ucd_version_info"].size();
                    if(version_count > 0)
                    {
                        version = data["ucd_version_info"][version_count-1];
                        version = "v" + version.substr(1, version.size()-1);
                    }
                    else
                    {
                        std::cout << ERROR_COLOR << "no ucd version in remote server" << STOP_COLOR << std::endl;
                    }
                }
                else
                {
                    std::cout << ERROR_COLOR << "get latest ucd version failed " << STOP_COLOR << std::endl;
                    std::cout << ERROR_COLOR << "connect error : " << check_url << STOP_COLOR << std::endl;
                    return -1;
                }
            }
            
            // 下载数据
            ucd_util->load_ucd_app(version, app_dir);
            std::cout << "--------------------------------------" << std::endl;
            std::cout << "load ucd_" + version + " success" << std::endl;
            std::cout << "--------------------------------------" << std::endl;
            std::cout << "" << std::endl;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "install")
    {
        std::cout << "直接运行下述命令在新机器上安装 ucd，注意修改 ucd 版本和缓存路径"                       << std::endl;
        std::cout << "--------------------------------------------------------------------------------" << std::endl;
        std::cout << "mkdir -p  /home/ldq/Apps_jokker  /usr/ucd_cache"                                  << std::endl;
        std::cout << "curl  -o  /home/ldq/Apps_jokker/ucd http://192.168.3.111:11101/ucd_app/v4.8.9"    << std::endl;
        std::cout << "chmod 777 /home/ldq/Apps_jokker/ucd /usr/ucd_cache -R"                            << std::endl;
        std::cout << "echo 'alias ucd=/home/ldq/Apps_jokker/ucd' >> /root/.bash_aliases"              << std::endl;
        std::cout << ". /root/.bash_aliases"                                                       << std::endl;
        std::cout << "/home/ldq/Apps_jokker/ucd set cache_dir /usr/ucd_cache"                           << std::endl;
        std::cout << "--------------------------------------------------------------------------------" << std::endl;
    }
    else if(command_1 == "split")
    {
        // 随机将数据集按照一定的比例分为两个部分
        if(argc == 6)
        {
            std::string ucd_path = argv[2];
            std::string ucd_part_a = argv[3];
            std::string ucd_part_b = argv[4];
            float ratio = std::stof(argv[5]);
            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->split(ucd_part_a, ucd_part_b, ratio);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "split_by_date" || command_1 == "split_by_uc")
    {
        if(argc == 4)
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];

            if(! is_read_file(ucd_path))
            {
                std::cout << ERROR_COLOR << "ucd path is not readable : " << ucd_path << STOP_COLOR << std::endl;
                return -1; 
            }

            // 获取保存的名字
            std::string save_name = "";
            if(long_args.count("save_name") > 0)
            {
                save_name = long_args["save_name"];
            }
            else
            {
                save_name = get_file_name(ucd_path);
            }

            UCDataset *ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->split_by_date(save_dir, save_name);
            delete ucd;
            return 1;
        }
        else
        {
            ucd_param_opt->print_command_info("split_by_date");
        }
    }
    else if(command_1 == "split_by_conf")
    {

        std::string mode = "split_json";  // change tag 
        std::string save_name = "";

        if(short_args.count("t") > 0)
        {
            mode = "change_tag";
        }
        
        if(long_args.count("save_name") > 0)
        {
            save_name = long_args["save_name"];
        }

        if(argc == 4 && mode == "split_json")
        {
            std::string ucd_path    = argv[2];
            std::string save_dir    = argv[3];
            UCDataset *ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->split_by_conf(save_dir, save_name);
            delete ucd;
        }
        else if(argc == 4 && mode == "change_tag")
        {
            std::string ucd_path    = argv[2];
            std::string save_path   = argv[3];
            UCDataset *ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->split_by_conf_change_tags();
            ucd->save_to_ucd(save_path);
            delete ucd;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "split_by_tags")
    {
        // 按照 tag 分为不同的 json 文件，保存在本地

        if(argc == 4)
        {
            std::string ucd_path = argv[2];
            std::string save_dir = argv[3];

            if(! is_read_file(ucd_path))
            {
                std::cout << ERROR_COLOR << "ucd path is not readable : " << ucd_path << STOP_COLOR << std::endl;
                return -1; 
            }

            // 获取保存的名字
            std::string save_name = "";
            if(long_args.count("save_name") > 0)
            {
                save_name = long_args["save_name"];
            }
            else
            {
                save_name = get_file_name(ucd_path);
            }

            UCDataset *ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            std::map<std::string, std::map<std::string, int> > count_map = ucd->count_tags();

            auto iter = count_map["rectangle"].begin();
            while(iter != count_map["rectangle"].end())
            {
                UCDataset *each_ucd = new UCDataset(ucd_path);
                each_ucd->parse_ucd(true);
                std::cout << "split -> " << iter->first << std::endl;
                std::string each_save_path = save_dir + "/" + save_name + "_" + iter->first + ".json";
                std::set<std::string> tags = {iter->first};
                each_ucd->filter_by_tags(tags);
                each_ucd->drop_empty_uc();
                each_ucd->update_time=getPythonStyleTimestamp();
                each_ucd->save_to_ucd(each_save_path);
                delete each_ucd;
                iter++;
            }
            delete ucd;
            return 1;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "absorb")
    {
        // 从其他 ucd 中吸收需要的内容 size_info, object_info
        if(argc == 6)
        {
            std::string ucd_path = argv[2];
            std::string meat_ucd_path = argv[3];
            std::string save_path = argv[4];
            std::string attr = argv[5];
            // 
            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->absorb(meat_ucd_path, save_path, attr);
            delete ucd;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
            return -1;
        }
    }
    else if(command_1 == "buddha_bless")
    {
        std::string name = "you";
        if(argc > 2)
        {
            name = argv[2];
        }

        std::cout << "               _ooOoo_                " << std::endl;
        std::cout << "              o8888888o               " << std::endl;
        std::cout << "              88' . '88               " << std::endl;
        std::cout << "              (| -_- |)               " << std::endl;
        std::cout << "              O\\  =  /O     bless " << name << std::endl;
        std::cout << "           ____/`---'\\____            " << std::endl;
        std::cout << "         .'  \\|       |    `.         " << std::endl;
        std::cout << "        /  \\|||||  :  |||   |        " << std::endl;
        std::cout << "       /  _||||| -:- |||||- |        " << std::endl;
        std::cout << "       |   | \\  -  / |   |            " << std::endl;
        std::cout << "       | \\_|  ''\---/''  |   |        " << std::endl;
        std::cout << "       \\  .-\\__  `-`  ___/-. /        " << std::endl;
        std::cout << "     ___`. .'  /--.--\\  `. . __       " << std::endl;
        std::cout << "  ."" '<  `.___\\_<|>_/___.'  >'"".    " << std::endl;
        std::cout << " | | :  `- \\`.;`\\ _ /`;.`/ - ` : | |  " << std::endl;
        std::cout << " \\  \\ `-.   \_ __\\ /__ _/   .-` /  /  " << std::endl;
        std::cout << "===`-.____`-.___\\_____/___.-`____.-'==" << std::endl;
        std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
        std::cout << "        Buddha Bless, No Bug !        " << std::endl;
    }
    else if(command_1 == "devide")
    {
        if(argc == 6)
        {
            std::string ucd_path    = argv[2];
            std::string save_dir    = argv[3];
            std::string save_name   = argv[4];
            int devide_count = std::stoi(argv[5]);
            std::string save_path = pystring::strip(save_dir, "/") + "/" + save_name + ".json";
            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->devide(save_path, devide_count);
            delete ucd;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "exec")
    {
        // 执行命令脚本中的命令 
        // command 中也能指定正常的 ucd 命令！！！场景就是使用 ucd 进行 批处理，批处理使用 bash 就够了，但是bash 中为什么用不了自定义的关键字 https://blog.csdn.net/qq_33709508/article/details/101822329
        // todo 第一个参数是 command_path，第二个参数是 ucd_path，最后一个参数是 save_path 

        if(argc == 5)
        {
            std::string ucd_path = argv[2];
            std::string command_path = argv[3];
            std::string save_path = argv[4];

            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->exec(command_path);
            ucd->save_to_ucd(save_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "to_exec")
    {
        // 将 ucd 转为 .ucd 格式的代码, 类似于 .sql 脚本能直接运行，

    }
    else if(command_1 == "run")
    {
        // 执行复杂的任务，结果不一定是 ucd，结果直接打印出来即可
        // 设置常用的关键字, count, select, save, drop, filter 等，ucd 先解析命令字符串然后直接运行，类似于 python 那种的形式
        // ucd run "count nc where uc[:2] == 'CA' and obj.area > 10"
    }
    else if(command_1 == "dete")
    { 
        // 设置的标准启动模型，发送模型，配置文件和逻辑进去

        // model 使用一个叫做 model_uc 的东西，model 都维护在一个目录中，在 80 服务器上，（需要注意的是，对于大的文件需要解决传输断裂的问题，是不是要将大文件拆分为小文件进行处理）

        // config 和 logic 直接使用二进制文件编码，服务端下载了直接能用就行

        // 

    }
    else if(command_1 == "chart_gpt")
    {
        // 和 chart gpt 进行交互，主要看（1）一点点打印如何实现（2）格式化的输出
    }
    else if(command_1 == "fake_uc")
    {
        std::cout << WARNNING_COLOR << "fake_uc 功能慎用，不要将 fake_uc 与正确 uc 混合使用" << STOP_COLOR << std::endl;

        if(argc == 3)
        {
            std::string fake_folder = argv[2];
            ucd_util->set_fack_uc(fake_folder);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "update_tags")
    {
        if(argc > 4)
        {
            std::string ucd_path = argv[2];
            std::string save_path = argv[3];
            std::map<std::string, std::string> tag_map;
            for(int i=4; i<argc; i++)
            {
                std:;string tag_info = argv[i];
                std::vector<std::string> split_res = pystring::split(tag_info, ":");

                if(split_res.size() != 2)
                {
                    std::cout << "update format error, need old_tag:new_tag" << std::endl;
                    throw "update format error, need old_tag:new_tag";
                }
                else
                {
                    tag_map[split_res[0]] = split_res[1];
                } 
            }
            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->update_tags(tag_map);
            ucd->update_time = getPythonStyleTimestamp();
            ucd->save_to_ucd(save_path);
            delete ucd;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "drop_tags")
    {
        if(argc > 4)
        {
            std::string ucd_path = argv[2];
            std::string save_path = argv[3];
            std::set<std::string> tags;
            for(int i=4; i<argc; i++)
            {
                std::string each_tag = argv[i];
                tags.insert(each_tag);
            }
            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->drop_tags(tags);
            ucd->save_to_ucd(save_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "draw")
    {
        // 画图，不指定颜色的全部使用默认颜色，默认和指定颜色在 ucd_cache/color.txt

        std::string ucd_path;
        std::string save_dir;
        std::vector<std::string> uc_list = {};

        // 是否覆盖之前的图片
        bool cover_old = false;

        if(short_args.count("f") > 0)
        {
            cover_old = true;
        }

        if(argc >= 4)
        {
            ucd_path = argv[2];
            save_dir = argv[3];
            
            for(int i=4; i<argc; i++)
            {
                uc_list.push_back(argv[i]);
            }
            ucd_util->draw_res(ucd_path, save_dir, uc_list, cover_old);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "drop_empty_uc")
    {
        // 当 uc 中不包含 obj 时，删除这个空的 uc
        if(argc == 4)
        {
            std::string ucd_path = argv[2];
            std::string save_path = argv[3];
            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->drop_empty_uc();
            ucd->save_to_ucd(save_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "drop_extra_info")
    {
        if(argc == 4)
        {
            std::string ucd_path    = argv[2];
            std::string save_path   = argv[3];
            UCDataset* ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->drop_extra_info();
            ucd->save_to_ucd(save_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }   
    }
    else if(command_1 == "random_color")
    {
        // 将 ucd 中包含的 tag 设置随机颜色
        if(argc == 3)
        {
            std::string ucd_path = argv[2];
            ucd_util->get_random_color_map(ucd_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "dete")
    {
        // 将标准 docker 服务的启动方式打印出来即可
        // docker info : md5, name, version
        // docker run --gpus device=0 -v /home/input_dir:/usr/input_dir -v /home/output_dir:/usr/output_dir -d dete_server:v0.0.1 
    }
    else if(command_1 == "book")
    {
        if(argc == 3 || argc == 2)
        {
            std::string book_index = "";
            
            if(argc == 3)
            {
                book_index = argv[2];
            }
            RedisBook *book = new RedisBook(redis_host, redis_port, redis_name);
            book->menu_loop(book_index);
            book->close();
        }
        else
        {
            ucd_param_opt->print_command_info(command_1); 
        }
    }
    else if(command_1 == "todo")
    {
        // TODO: 查看一个较大日期范围 todo 信息

        // 指定用户
        std::string assign_name = "";
        if(long_args.count("name") != 0)
        {
            assign_name = long_args["name"];
        }
        else
        {
            assign_name = redis_name;
        }

        TodoList *todo_list = new TodoList(redis_host, redis_port, assign_name);
        std::string assign_date;

        // 指定时间
        if(long_args.count("date") != 0)
        {
            assign_date = long_args["date"];
            assign_date = todo_list->get_date(assign_date);
        }
        else
        {
            assign_date = todo_list->get_date();
        }

        if(not todo_list->is_valid_date(assign_date))
        {
            std::cout << ERROR_COLOR << "* 日期格式不合法, 日期格式为 2023-06-14 或者 06-14" << STOP_COLOR << std::endl;
            return 0;
        }

        std::string method  = "";
        std::string info    = "";
        if(argc == 2 && long_args.count("date") == 0)
        {
            method = "check";
            todo_list->print_todo_info_assign_date_range(assign_date, 5);
        }
        else if(argc == 2)
        {
            todo_list->print_todo_info(assign_date);
        }
        else
        {
            method  = argv[2];

            for(int i=3; i<argc; i++)
            {
                info += argv[i] + ",";
            }

            if(method == "add")
            {
                todo_list->add_todo_info(assign_date, info);
                todo_list->print_todo_info(assign_date);
            }
            else if(method == "del" || method == "done" || method == "undo")
            {
                int index;
                if(info == "all,")
                {
                    index = -1;
                }
                else
                {
                    index = std::stoi(info);
                }

                if(method == "del")
                {
                    todo_list->delete_todo_info(assign_date, index);
                }
                else if(method == "done")
                {
                    todo_list->finish_todo(assign_date, index);
                }
                else
                {
                    todo_list->undo_todo(assign_date, index);
                }
                todo_list->print_todo_info(assign_date);
            }
            else if(method == "check")
            {
                todo_list->print_todo_info(assign_date);
            } 
            else
            {
                ucd_param_opt->print_command_info(command_1);
            }
        }
        delete todo_list;
    }
    else if(command_1 == "receiver")
    {

        // TODO: 做一个数据的服务器，接收相对位置，返回找到的文件并返回
        // TODO: 成为接收器，接收消息，receiver_v1, receive_v2, 心跳信息之类的
        // TODO: 作为税局中转站，将请求进行转发
        // TODO: 获取 meta, cache 等信息，提供给其他机器调用
        // TODO: 设置为协程启动，并进行阻塞


        int port            = 11223;
        std::string name    = "";

        if(long_args.count("port") != 0)
        {
            port = std::stoi(long_args["port"]);
        }

        if(long_args.count("name") != 0)
        {
            name = long_args["name"];
        }

        httplib::Server server;

        std::string additional_param = "123";

        // auto bound_handle_post = std::bind(handle_post, std::placeholders::_1, std::placeholders::_2, additional_param);

        // server.Post("/" + name, bound_handle_post);


        server.Post("/" + name, handle_post);
        server.listen("0.0.0.0", port);

    }
    else if(command_1 == "img_server")
    {

        // 当 80 服务器失效时，可以使用 ucd 和当前机器的缓存提供图片服务

        int port = 5001;
        std::string img_dir = "";

        // img_dir
        if(long_args.count("img_dir") != 0 )
        {
            img_dir = long_args["img_dir"];
        }
        else
        {
            img_dir = ucd_util->cache_img_dir;
        }

        // port
        if(long_args.count("port") != 0 )
        {
            port = stoi(long_args["port"]);
        }

        std::string ip_str = get_ip_address();
        std::cout << "---------------------------------------------------" << std::endl;
        std::cout << "port      : " << port << std::endl;
        std::cout << "url       : " << "http://" << ip_str << ":" << port << "/image/{uc}" << std::endl;
        std::cout << "file_dir  : " << img_dir << std::endl;
        std::cout << "---------------------------------------------------" << std::endl;

        httplib::Server server;
        server.Get("/file/(.*)", [&](const httplib::Request& req, httplib::Response& res) 
        {
            std::string image_name  = req.matches[1];                            // 获取路径参数中的图片名
            std::string image_path  = img_dir + "/" + image_name;      // 获取图片文件路径
            std::string clientIP    = req.remote_addr;

            if(is_read_file(image_path))
            {
                std::cout  << "get image [" << clientIP << "] -> " << image_path << std::endl;
            }
            else
            {
                std::cout << ERROR_COLOR << "get image [" << clientIP << "] -> " << image_path << " [failed] " << STOP_COLOR << std::endl;
            }

            // 读取图片文件
            std::ifstream file(image_path, std::ios::binary);

            if (file) 
            {
                // 获取文件大小
                file.seekg(0, std::ios::end);
                size_t file_size = file.tellg();
                file.seekg(0, std::ios::beg);

                // 分配缓冲区并读取文件内容
                std::vector<char> buffer(file_size);
                file.read(buffer.data(), file_size);

                // 将缓冲区内容转换为字符串
                std::string content(buffer.data(), file_size);

                // 设置响应内容类型为图片
                res.set_header("Content-Type", "image/jpeg");
                res.set_content(content, "image/jpeg");
            } 
            else 
            {
                // 文件不存在，返回404错误
                res.status = 404;
                res.set_content("Image not found", "text/plain");
            }
        });

        // 启动服务器，监听端口为8080
        server.listen("0.0.0.0", port);
    }
    else if(command_1 == "post_v2")
    {
        if(argc == 3 || argc ==4)
        {
            std::string ucd_path    = argv[2];

            std::string save_xml_dir = "";
            if(argc == 4)
            {
                save_xml_dir = argv[3];
            }

            int server_port         = 111;
            int post_port           = -1;
            std::string server_host = "192.168.3.221";
            std::string batch_id    = "test_ucd_post_v2";
            std::vector<std::string> model_list = {"nc", "kkx"};

            if(long_args.count("host") != 0)
            {
                server_host = long_args["host"];
            }

            if(long_args.count("batch_id") != 0)
            {
                batch_id = long_args["batch_id"];
            }

            if(long_args.count("port") != 0)
            {
                server_port = std::stoi(long_args["port"]);
            }

            if(long_args.count("post_port") != 0)
            {
                post_port = std::stoi(long_args["post_port"]);
            }

            if(long_args.count("model_list") != 0)
            {
                std::string model_list_str = long_args["model_list"];
                model_list = pystring::split(model_list_str, ",");
            }

            std::string url = "http://" + server_host + ":" + std::to_string(server_port);
            post_v2(host, port, ucd_path, url, model_list, batch_id, save_xml_dir, post_port);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "augment")
    {
        if(argc == 8 || argc == 9)
        {
            std::string ucd_path    = argv[2];
            std::string save_path   = argv[3];

            if((! is_read_file(ucd_path)) || (! pystring::endswith(ucd_path, ".json")))
            {
                std::cout << ERROR_COLOR << "ucd path not illeagle : " << ucd_path << STOP_COLOR << std::endl;
                // throw "ucd path not illeagle";
                return -1;
            }

            float x1, x2, y1, y2;
            x1 = std::stof(argv[4]);
            x2 = std::stof(argv[5]);
            y1 = std::stof(argv[6]);
            y2 = std::stof(argv[7]);

            bool is_relative = true;
            if(argc == 9)
            {
                std::string is_relative_str   = argv[8];
                if((is_relative_str == "false") || (is_relative_str == "False") || (is_relative_str == "0"))
                {
                    is_relative = false;
                }
            }

            UCDataset *ucd = new UCDataset(ucd_path);
            ucd->parse_ucd(true);
            ucd->do_augment(x1, x2, y1, y2, is_relative);
            ucd->update_time = getPythonStyleTimestamp();
            ucd->save_to_ucd(save_path);
            delete ucd;
            return 1;
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "foretell")
    {
        // 预言
        if(argc == 3)
        {
            std::string text = argv[2];
            get_change(text);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "ls")
    {
        if(argc == 3)
        {
            std::string folder_path = argv[2];
            ucd_util->list_uci(folder_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "rm")
    {
        if(argc == 3)
        {
            std::string uci_path = argv[2];
            ucd_util->delete_uci(uci_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "cp")
    {
        if(argc == 4)
        {
            std::string uci_path = argv[2];
            std::string save_path = argv[3];
            ucd_util->copy_uci(uci_path, save_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "mv")
    {
        if(argc == 4)
        {
            std::string uci_path = argv[2];
            std::string save_path = argv[3];
            ucd_util->move_uci(uci_path, save_path);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "json_to_uci")
    {
        if(argc == 4 || argc == 5)
        {
            std::string json_path   = argv[2];
            std::string uci_path    = argv[3];
            int volume_size         = volume_size;
            if(argc == 5)
            {
                std::string volume_size_str = argv[4];
                volume_size = std::stoi(volume_size_str);
            }
            ucd_util->json_to_uci(json_path, uci_path, volume_size);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "uci_to_json")
    {
        if((argc == 4) || (argc == 5))
        {
            std::string uci_path    = argv[2];
            std::string json_path   = argv[3];
            int volume_size         = volume_size;

            if(argc == 5)
            {
                std::string volume_size_str = argv[4];
                volume_size                 = std::stoi(volume_size_str);
            }

            ucd_util->uci_to_json(uci_path, json_path, volume_size);
        }
        else
        {
            ucd_param_opt->print_command_info(command_1);
        }
    }
    else if(command_1 == "obi_to_obi")
    {
        // 重新划分分卷大小
    }
    else if(command_1 == "search_word")
    {
        // 全文本匹配功能
        // 指定需要寻找的文件夹，指定需要配置的文件的类型，指定
        // 类似 : grep jokker /home/ldq/ -r , 但是功能更加强大一些
    }
    else if(command_1 == "test")
    {
        while(true)
        {
            std::string command;
            std::cout << "请输入命令：";
            std::getline(std::cin, command);
            std::cout << "您输入的命令是：" << command << std::endl;
        }
    }
    else if(command_1 == "grammar")
    {
        if(castration_function == "")
        {
            ucd_param_opt->print_all_fun();
        }
        else
        {
            ucd_param_opt->print_castration_fun(castration_function);
        }
        return -1;
    }
    else if(command_1 == "game")
    {

        while(1)
        {
            std::cout << "               _ooOoo_                " << std::endl;
            sleep(1);
            std::system("clear");
        }

        // std::create


    }
    else if(ucd_param_opt->has_simliar_command(command_1))
    {
        ucd_param_opt->print_similar_command_info(command_1);
        return -1;
    }
    else
    {
        ucd_param_opt->print_all_fun();
        return -1;
    }
    
    delete ucd_util;
    delete ucd_param_opt;
	return 0;
}
