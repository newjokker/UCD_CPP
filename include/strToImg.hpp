#ifndef _STRTOIMG_HPP_
#define _STRTOIMG_HPP_

#include <opencv2/opencv.hpp>
#include <iostream>


static std::string base64Decode(const char* Data, int DataByte);

static std::string base64Encode(const unsigned char* Data, int DataByte);

std::string Mat2Base64(const cv::Mat &img, std::string imgType);

std::string Mat2Base64(const cv::Mat &img, std::string imgType);

#endif