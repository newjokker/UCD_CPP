#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <time.h>
#include <fstream>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <httplib.h>
#include <string>
#include "include/md5.hpp"
#include "include/pystring.h"
#include "include/the_book_of_change.hpp"


std::vector<std::string> change_info = {
    "乾,qián",
    "坤,kūn",
    "屯,zhūn",
    "蒙,méng",
    "需,xū",
    "讼,sòng",
    "师,shī",
    "比,bǐ",
    "小畜,xiǎo xù",
    "履,lǚ",
    "泰,tài",
    "否,pǐ",
    "同人,tóng rén",
    "大有,dà yǒu",
    "谦,qiān",
    "豫,yù",
    "随,suí",
    "蛊,gǔ",
    "临,lín",
    "观,guān",
    "噬嗑,shì hé",
    "贲,bì",
    "剥,bō",
    "复,fù",
    "无妄,wú wàng",
    "大畜,dàxù",
    "颐,yí",
    "大过,dà guò",
    "坎,kǎn",
    "离,lí",
    "咸,xián",
    "恒,héng",
    "遁,dùn",
    "大壮,dà zhuàng",
    "晋,jìn",
    "明夷,míng yí",
    "家人,jiā rén",
    "睽,kuí",
    "蹇,jiǎn",
    "解,xiè",
    "损,sǔn",
    "益,yì",
    "夬,guài",
    "姤,gòu",
    "萃,cuì",
    "升,shēng",
    "困,kùn",
    "井,jǐng",
    "革,gé",
    "鼎,dǐng",
    "震,zhèn",
    "艮,gèn",
    "渐,jiàn",
    "归妹,guī mèi",
    "丰,fēng",
    "旅,lǚ",
    "巽,xùn",
    "兑,duì",
    "涣,huàn",
    "节,jié",
    "中孚,zhōng fú",
    "小过,xiǎo guò",
    "既济,jì jì",
    "未济,wèi jì",
};


void get_change(std::string text)
{
    std::string md5 = get_str_md5(text);

    // get int 1 - 64, 
    int count = 0;
    for(int i=0; i<md5.size(); i++)
    {
        int each = md5[i];
        count += each;
    }

    count = count % 64;
    std::cout << "------------------------------------------" << std::endl;

    std::vector<std::string> gua_info = pystring::split(change_info[count], ",");

    std::cout << "卦象 : " << gua_info[0] << " (" << gua_info[1] << ")" << std::endl;
    
    std::cout << "" << std::endl;
    
    std::cout << "参考 : https://baike.baidu.com/item/" << gua_info[0] << "卦" << std::endl;

    std::cout << "------------------------------------------" << std::endl;

}
