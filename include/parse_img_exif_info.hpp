/*
* Authors:
* Simba <lansheng228@163.com>
*
*/
#ifndef IMAGEINFO_H
#define IMAGEINFO_H
// #include <QObject>
// #include <QDebug>
// #include <QUrl>
#include <string>
// #include <QSize>
// #include <QDate>
// #include "imageinfo.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
// #include <QFile>
// #include <QFileInfo>



// class ImageInfo :
// {
//     Q_OBJECT
//     enum IMAGE_FORMAT{
//         BMP_FORMAT,
//         JPG_FORMAT,
//         GIF_FORMAT,
//         PNG_FORMAT,
//         NVL_FORMAT
//     };
// public:
//     explicit ImageInfo(QObject *parent = 0);
//     ~ImageInfo();
// public:
//     Q_INVOKABLE QString getImageFormat(QString imageUrl);
//     Q_INVOKABLE QString getImageSize(QString imageUrl);
//     Q_INVOKABLE QSize getImageDimension(QString imageUrl);
//     Q_INVOKABLE QDate getImageDate(QString imageUrl);
//     Q_INVOKABLE QString getImageTitle(QString imageUrl);
// Q_SIGNALS:
// public Q_SLOTS :
// private:
//     int getImageFormat(std::string path);
//     long getBMPSize(std::string path);
//     long getGIFSize(std::string path);
//     long getPNGSize(std::string path);
//     long getJPGSize(std::string path);
//     QSize getBMPDimension(std::string path);
//     QSize getPNGDimension(std::string path);
//     QSize getJPGDimension(std::string path);
//     QSize getGIFDimension(std::string path);
// };


#define QString std::string;

ImageInfo::ImageInfo(QObject *parent) :
    QObject(parent)
{
    qDebug() << "---------------------------- image info constructed ";
}

ImageInfo::~ImageInfo()
{
}

QDate ImageInfo::getImageDate(QString imageUrl)
{
    QDate date;
    if(!imageUrl.isEmpty()) {
        QUrl fileUrl(imageUrl);
        QString filePath = fileUrl.toLocalFile();
        if(QFile::exists(filePath)) {
            QFileInfo fileinfo(filePath);
            date = fileinfo.lastModified().date();
        }
    }
    return date;
}

//从文件头中读取相应字段以判断图片格式
//详情参看: http://www.garykessler.net/library/file_sigs.html
int ImageInfo::getImageFormat(std::string path)
{
    //BMP格式特征码
    unsigned char BMPHeader[] = {0x42, 0x4d};
    //JPG,JPEG格式特征码
    unsigned char JPGHeader1[] = {0xff, 0xd8, 0xff, 0xdb};
    unsigned char JPGHeader2[] = {0xff, 0xd8, 0xff, 0xe0};
    unsigned char JPGHeader3[] = {0xff, 0xd8, 0xff, 0xe1};
    unsigned char JPGHeader4[] = {0xff, 0xd8, 0xff, 0xe2};
    unsigned char JPGHeader5[] = {0xff, 0xd8, 0xff, 0xe3};
    unsigned char JPGHeader6[] = {0xff, 0xd8, 0xff, 0xe8};
    //GIF格式特征码
    unsigned char GIFHeader1[] = {0x47, 0x49, 0x46, 0x38, 0x37, 0x61};
    unsigned char GIFHeader2[] = {0x47, 0x49, 0x46, 0x38, 0x39, 0x61};
    //PNG格式特征码
    unsigned char PNGHeader[] = {0x89, 0x50, 0x4E, 0x47};
    int count = 0;
    int step = 2;
    //以二进制方式打开文件并读取前几个字节
    unsigned char header[16];
    qDebug()<<"文件路径: "<<path.c_str();
    std::ifstream readf(path.c_str(), std::ios::binary);
    if(!readf.is_open()) {
        qDebug()<<"打开文件失败";
        return NVL_FORMAT;
    }
    //先读两个，判断是否BMP格式
    for(int i=0; i<step; i++) {
        readf>>header[count+i];
    }
    count = count + step;
    if(memcmp(header, BMPHeader, count) == 0) {
        qDebug()<<"文件格式特征码:";
        for(int i=0; i<count; i++) {
            printf("%0x\t",header[i]);
        }
        printf("\n");
        qDebug()<<"BMP格式";
        return BMP_FORMAT;
    }
    //再读两个，判断是否JPG格式、PNG格式
    for(int i=0; i<step; i++) {
        readf>>header[count+i];
    }
    count = count + step;
    if((memcmp(header, JPGHeader1, count) == 0)
            || (memcmp(header, JPGHeader2, count) == 0)
            || (memcmp(header, JPGHeader3, count) == 0)
            || (memcmp(header, JPGHeader4, count) == 0)
            || (memcmp(header, JPGHeader5, count) == 0)
            || (memcmp(header, JPGHeader6, count) == 0)) {
        qDebug()<<"文件格式特征码:";
        for(int i=0; i<count; i++) {
            printf("%0x\t",header[i]);
        }
        printf("\n");
        qDebug()<<"JPG格式";
        return JPG_FORMAT;
    } else if(memcmp(header, PNGHeader, count) == 0) {
        qDebug()<<"文件格式特征码:";
        for(int i=0; i<count; i++) {
            printf("%0x\t",header[i]);
        }
        printf("\n");
        qDebug()<<"PNG格式";
        return PNG_FORMAT;
    }
    //再读两个，判断是否GIF格式
    for(int i=0; i<step; i++) {
        readf>>header[count+i];
    }
    count = count + step;
    if((memcmp(header, GIFHeader1, count) == 0)
            || (memcmp(header, GIFHeader2, count) == 0)) {
        qDebug()<<"文件格式特征码:";
        for(int i=0; i<count; i++) {
            printf("%0x\t",header[i]);
        }
        printf("\n");
        qDebug()<<"GIF格式";
        return GIF_FORMAT;
    }
    qDebug()<<"文件格式特征码:";
    for(int i=0; i<count; i++) {
        printf("%0x\t",header[i]);
    }
    printf("\n");
    qDebug()<<"不属于以上任何一种格式";
    return NVL_FORMAT;
}

QString ImageInfo::getImageFormat(QString imageUrl)
{
    QString strFormat = "NA";
    if(!imageUrl.isEmpty()) {
        QUrl fileUrl(imageUrl);
        QString filePath = fileUrl.toLocalFile();
        if(QFile::exists(filePath)) {
            std::string path = filePath.toStdString();
            int iFormat = getImageFormat(path);
            switch(iFormat) {
            case BMP_FORMAT:
                strFormat = "BMP";
                break;
            case JPG_FORMAT:
                strFormat = "JPG";
                break;
            case GIF_FORMAT:
                strFormat = "GIF";
                break;
            case PNG_FORMAT:
                strFormat = "PNG";
                break;
            default:
                break;
            }
        }
    }
    return strFormat;
}

std::string getImageSize(QString imageUrl)
{
    QString strSize;
    long size = 0;
    if(!imageUrl.isEmpty()) {
        QUrl fileUrl(imageUrl);
        QString filePath = fileUrl.toLocalFile();
        if(QFile::exists(filePath)) {
            QFile file(filePath);
            bool ret = file.open(QIODevice::ReadOnly);
            if (!ret) {
                qDebug()<<"打开文件失败";
                size = 0;
            } else {
                size = file.size();
            }
            file.close();
        }
    }
    qDebug()<<"!!!!!"<<size;
    strSize = QString::number(size, 10);
    qDebug()<<strSize;
    return strSize;
}

//BMP文件头的第2、3字为文件大小信息
long ImageInfo::getBMPSize(std::string path)
{
    FILE *fid;
    long int size;
    if((fid=fopen(path.c_str(),"rb+"))==NULL) {
        qDebug()<<"打开文件失败";
        return 0;
    }
    //跳过图片特征码
    fseek(fid, 2, SEEK_SET);
    fread(&size, sizeof(long), 1, fid);
    fclose(fid);
    qDebug()<<"size="<<size;
    return size;
}
long ImageInfo::getGIFSize(std::string path)
{
    Q_UNUSED(path);
    return 0;
}
long ImageInfo::getPNGSize(std::string path)
{
    Q_UNUSED(path);
    return 0;
}
long ImageInfo::getJPGSize(std::string path)
{
    FILE *fid;
    long int size;
    if((fid = fopen(path.c_str(),"rb+")) == NULL) {
        qDebug()<<"打开文件失败";
        return 0;
    }
    fseek(fid, 0, SEEK_END);
    size = ftell(fid);
    fclose(fid);
    qDebug()<<"size="<<size;
    return size;
}

//BMP文件头的第10、11字为文件宽度信息
//BMP文件头的第12、13字为文件高度信息
QSize ImageInfo::getBMPDimension(std::string path)
{
    FILE *fid;
    if((fid=fopen(path.c_str(),"rb+"))==NULL) {
        qDebug()<<"打开文件失败";
        return QSize(0, 0);
    }
    long int width;
    long int height;
    //读取宽度和高度
    fseek(fid, 18, SEEK_SET); //偏移18个字节
    fread(&width, sizeof(long), 1, fid);
    fread(&height, sizeof(long), 1, fid);
    qDebug()<<"width="<<width;
    qDebug()<<"height="<<height;
    fclose(fid);
    return QSize(width, height);
}

//参考： http://mcljc.blog.163.com/blog/static/83949820102239610974/
//http://download.csdn.net/download/chp845/4255011
QSize ImageInfo::getJPGDimension(std::string path)
{
    FILE *fid;
    if((fid = fopen(path.c_str(),"rb+")) == NULL) {
        qDebug()<<"打开文件失败";
        return QSize(0, 0);
    }
    long int width;
    long int height;
    fseek(fid,0,SEEK_END);
    long length = ftell(fid);
    unsigned char *buffer = new unsigned char[length];
    unsigned char *buffer_bakup = buffer;
    fseek(fid, 0, SEEK_SET);
    fread(buffer, length, 1, fid);
    fclose(fid);
    unsigned char *temp = buffer + length;
    unsigned char *temp_ori = buffer;
    unsigned char ff;
    unsigned char type=0xff;
    int m_pos = 0;
    //跳过文件头中标志文件类型的两个字节
    for(int i=0; i<2; i++) {
        buffer++;
    }
    while((temp > buffer) && (type != 0xDA)) {
        do{
            ff = *buffer++;
        } while(ff != 0xff);
        do{
            type = *buffer++;
        } while(type == 0xff);
        switch(type) {
        case 0x00:
        case 0x01:
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7:
            break;
        case 0xC0://SOF0段
            temp_ori = buffer;
            m_pos = (*buffer++)<<8;
            m_pos += *buffer++;
            buffer++; //舍弃精度值
            height = (*buffer++)<<8;
            height += *buffer++;
            width = (*buffer++)<<8;
            width += *buffer;
            break;
        case 0xE0: //APP0段
            qDebug()<<"APP0段";
            temp_ori = buffer;
            m_pos = (*buffer++)<<8;
            m_pos += *buffer++;
            buffer = buffer + 12;
            //丢弃APP0标记(5bytes)、主版本号(1bytes)、次版本号(1bytes)、像素点单位(1bytes)、垂直像素点(2bytes)、 水平像素点(2bytes)
            break;
        default:
            temp_ori = buffer;
            m_pos = (*buffer++)<<8;
            m_pos += *buffer++;
            break;
        }
        buffer = temp_ori + m_pos;
    }
    qDebug()<<"width="<<width;
    qDebug()<<"height="<<height;
    //记得释放内存
    delete[] buffer_bakup;
    return QSize(width, height);
}

//PNG文件头的第9字为文件宽度信息
//PNG文件头的第10字为文件高度信息
//参考：http://blog.chinaunix.net/uid-25799257-id-3358174.html
QSize ImageInfo::getPNGDimension(std::string path)
{
    FILE *fid = NULL;
    if((fid=fopen(path.c_str(),"rb+"))==NULL) {
        qDebug()<<"打开文件失败";
        return QSize(0, 0);
    }
    long int width;
    long int height;
    unsigned char wtmp[4]={'0'};   //宽度
    unsigned char htmp[4]={'0'};   //高度
    fseek(fid, 16, SEEK_SET);
    fread(wtmp, 4, 1, fid);         // example 00000080
    fread(htmp, 4, 1, fid);         // example 00000080
    fclose(fid);
    width = ((int)(unsigned char)wtmp[2]) * 256 + (int)(unsigned char)wtmp[3];
    height = ((int)(unsigned char)htmp[2]) * 256 + (int)(unsigned char)htmp[3];
    qDebug()<<"width="<<width;
    qDebug()<<"height="<<height;
    return QSize(width, height);
}

//GIF文件头的第4字为文件宽度信息
//GIF文件头的第5字为文件高度信息
//参考：http://blog.csdn.net/zhaoweikid/article/details/156422
//参考：http://blog.csdn.net/asaasa66/article/details/5875340
QSize ImageInfo::getGIFDimension(std::string path)
{
    std::ifstream ffin(path.c_str(), std::ios::binary);
    if (!ffin){
        std::cout<<"Can not open this file."<<std::endl;
        return QSize(0, 0);
    }
    long int width;
    long int height;
    char s1[2] = {0}, s2[2] = {0};
    ffin.seekg(6);
    ffin.read(s1, 2);
    ffin.read(s2, 2);
    width = (unsigned int)(s1[1])<<8|(unsigned int)(s1[0]);
    height = (unsigned int)(s2[1])<<8|(unsigned int)(s2[0]);
    ffin.close();
    qDebug()<<"width="<<width;
    qDebug()<<"height="<<height;
    return QSize(width, height);
}

QSize ImageInfo::getImageDimension(QString imageUrl)
{
    QSize dimension;
    if(!imageUrl.isEmpty()) {
        QUrl fileUrl(imageUrl);
        QString filePath = fileUrl.toLocalFile();
        if(QFile::exists(filePath)) {
            std::string path = filePath.toStdString();
            int iFormat = getImageFormat(path);
            switch(iFormat) {
            case BMP_FORMAT:
                dimension = getBMPDimension(path);
                break;
            case JPG_FORMAT:
                dimension = getJPGDimension(path);
                break;
            case GIF_FORMAT:
                dimension = getGIFDimension(path);
                break;
            case PNG_FORMAT:
                dimension = getPNGDimension(path);
                break;
            default:
                break;
            }
        }
    }
    qDebug()<<"图片尺寸:"<<dimension;
    return dimension;
}

QString ImageInfo::getImageTitle(QString imageUrl)
{
    QString title;
    if(!imageUrl.isEmpty()) {
        QUrl fileUrl(imageUrl);
        QString filePath = fileUrl.toLocalFile();
        if(QFile::exists(filePath)) {
            QFileInfo fileinfo(filePath);
            title = fileinfo.baseName();
        }
    }
    return title;
}    


#endif // IMAGEINFO_H