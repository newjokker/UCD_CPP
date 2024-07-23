#include <iostream>
#include <fstream>
#include <set>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
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
#include <algorithm>
#include <nlohmann/json.hpp>
#include "include/lablelmeObj.hpp"
#include <httplib.h>
#include <mysql/mysql.h>
using namespace httplib;
using json = nlohmann::json;
using namespace jotools;
using namespace std;



// 推荐算法，ucd recommend ucd_stand ucd_predit ucd_to_lable, 根据标准集合与预测出来的结果，对模型进行衡量，并从待标注的图片中找到优先标注的那些图片

// 

int main(int argc, char ** argv)
{
    // 
}

