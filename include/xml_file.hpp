#ifndef _XML_FILE_HPP_
#define _XML_FILE_HPP_


#include <iostream>
#include <time.h>
#include <algorithm>
#include <chrono>
#include <typeinfo>
#include "./tinyxml2.h"

using namespace std;

// 正框结构
struct ObjInfo
{
    string tag;
    float conf;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
};

// XML信息结构
struct XMLInfo
{
    string file_name;
    string folder;
    string path;
    int height;
    int width;
    int depth;
    vector<ObjInfo> objects;
};

// 打印 xml 信息
void print_xml_info(XMLInfo xml_info)
{

    std::cout << "file_name : " << xml_info.file_name << std::endl;
    std::cout << "folder : " << xml_info.folder << std::endl;
    std::cout << "path : " << xml_info.path << std::endl;

    vector<ObjInfo> objects = xml_info.objects;

    for(int i=0; i<objects.size(); i++)
    {
        std::cout << "tag  : " << objects[i].tag  << std::endl;
        std::cout << "conf : " << objects[i].conf << std::endl;
        std::cout << "xmin : " << objects[i].xmin << std::endl;
        std::cout << "xmax : " << objects[i].xmax << std::endl;
        std::cout << "ymin : " << objects[i].ymin << std::endl;
        std::cout << "ymax : " << objects[i].ymax << std::endl;
        std::cout << "----------------" << std::endl;
    }
    std::cout << "--------------------------" << std::endl;
}

// 读取 xml 信息
XMLInfo read_xml_info(string xml_path)
{
    XMLInfo xml_info;
    tinyxml2::XMLDocument doc;
	doc.LoadFile( "/home/ldq/input_dir/img_dir/fzc/Dsm082k.xml" );

    tinyxml2::XMLElement* root = doc.RootElement();
    tinyxml2::XMLElement* objects = root->FirstChildElement("object");
    tinyxml2::XMLElement* img_size = root->FirstChildElement("size");

    // xml attr 
    xml_info.folder = root->FirstChildElement("folder")->GetText();
    xml_info.path = root->FirstChildElement("path")->GetText();
    xml_info.file_name = root->FirstChildElement( "filename" )->GetText();
    xml_info.height = std::stoi(img_size->FirstChildElement("height")->GetText()); 
    xml_info.width = std::stoi(img_size->FirstChildElement("width")->GetText()); 
    xml_info.depth = std::stoi(img_size->FirstChildElement("depth")->GetText()); 

    // object info
    while(objects){

        ObjInfo obj_info;

        // attr
        obj_info.tag = objects->FirstChildElement("name")->GetText();
        obj_info.conf = std::stof(objects->FirstChildElement("prob")->GetText());

        // bndbox
        tinyxml2::XMLElement* bndbox = objects->FirstChildElement("bndbox");
        obj_info.xmin = std::stoi(bndbox->FirstChildElement("xmin")->GetText());
        obj_info.xmax = std::stoi(bndbox->FirstChildElement("xmax")->GetText());
        obj_info.ymin = std::stoi(bndbox->FirstChildElement("ymin")->GetText());
        obj_info.ymax = std::stoi(bndbox->FirstChildElement("ymax")->GetText());
        xml_info.objects.push_back(obj_info);
        objects = objects->NextSiblingElement();
    }
    return xml_info;
}

// // 在一个节点下面添加一个节点 
// void add_element(tinyxml2::XMLDocument doc, tinyxml2::XMLElement father, tinyxml2::XMLElement child, string key, string value)
// {
//     tinyxml2::XMLElement* node = doc.NewElement("key");
//     tinyxml2::XMLText* node_text = doc.NewText("value");
//     child.InsertEndChild(node_text);
//     father.InsertEndChild(node);
// }

// 保存为 xml 
bool save_xml_info(XMLInfo xml_info, string save_path)
{

    // refer : https://blog.csdn.net/K346K346/article/details/48750417

    // 声明
    const char* declaration ="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
    tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
    doc->Parse(declaration);

    // root element
    tinyxml2::XMLElement* root = doc->NewElement("annotation");
    doc->InsertEndChild(root);

    // insert attr
    // file name
    tinyxml2::XMLElement* file_name = doc->NewElement("file_name");
    tinyxml2::XMLText* file_name_text = doc->NewText(xml_info.file_name.c_str());
    file_name->InsertEndChild(file_name_text);
    root->InsertEndChild(file_name);
    // path
    tinyxml2::XMLElement* path = doc->NewElement("path");
    tinyxml2::XMLText* path_text = doc->NewText(xml_info.path.c_str());
    path->InsertEndChild(path_text);
    root->InsertEndChild(path);
    // folder
    tinyxml2::XMLElement* folder = doc->NewElement("folder");
    tinyxml2::XMLText* folder_text = doc->NewText(xml_info.folder.c_str());
    folder->InsertEndChild(folder_text);
    root->InsertEndChild(folder);

    // size
    tinyxml2::XMLElement* size = doc->NewElement("size");
    root->InsertEndChild(size);
    // height
    tinyxml2::XMLElement* height = doc->NewElement("height");
    tinyxml2::XMLText* height_text = doc->NewText(std::to_string(xml_info.height).c_str());
    height->InsertEndChild(height_text);
    size->InsertEndChild(height);
    // width
    tinyxml2::XMLElement* width = doc->NewElement("width");
    tinyxml2::XMLText* width_text = doc->NewText(std::to_string(xml_info.width).c_str());
    width->InsertEndChild(width_text);
    size->InsertEndChild(width);
    // depth
    tinyxml2::XMLElement* depth = doc->NewElement("depth");
    tinyxml2::XMLText* depth_text = doc->NewText(std::to_string(xml_info.width).c_str());
    depth->InsertEndChild(depth_text);
    size->InsertEndChild(depth);

    for(int i=0; i<xml_info.objects.size();i++)
    {
        tinyxml2::XMLElement* object = doc->NewElement("object");
        root->InsertEndChild(object);

        // tag
        tinyxml2::XMLElement* tag = doc->NewElement("name");
        tinyxml2::XMLText* tag_text = doc->NewText(xml_info.objects[i].tag.c_str());
        tag->InsertEndChild(tag_text);
        object->InsertEndChild(tag);
        // name
        tinyxml2::XMLElement* conf = doc->NewElement("prob");
        tinyxml2::XMLText* conf_text = doc->NewText(to_string(xml_info.objects[i].conf).c_str());
        conf->InsertEndChild(conf_text);
        object->InsertEndChild(conf);
        // bndbox
        tinyxml2::XMLElement* bndbox = doc->NewElement("bndbox");
        object->InsertEndChild(bndbox);
        // xmin
        tinyxml2::XMLElement* xmin = doc->NewElement("xmin");
        tinyxml2::XMLText* xmin_text = doc->NewText(to_string(xml_info.objects[i].xmin).c_str());
        xmin->InsertEndChild(xmin_text);
        bndbox->InsertEndChild(xmin);
        // xmax
        tinyxml2::XMLElement* xmax = doc->NewElement("xmax");
        tinyxml2::XMLText* xmax_text = doc->NewText(to_string(xml_info.objects[i].xmax).c_str());
        xmax->InsertEndChild(xmax_text);
        bndbox->InsertEndChild(xmax);
        // ymin
        tinyxml2::XMLElement* ymin = doc->NewElement("ymin");
        tinyxml2::XMLText* ymin_text = doc->NewText(to_string(xml_info.objects[i].ymin).c_str());
        ymin->InsertEndChild(ymin_text);
        bndbox->InsertEndChild(ymin);
        // ymax
        tinyxml2::XMLElement* ymax = doc->NewElement("ymax");
        tinyxml2::XMLText* ymax_text = doc->NewText(to_string(xml_info.objects[i].ymax).c_str());
        ymax->InsertEndChild(ymax_text);
        bndbox->InsertEndChild(ymax);
    }
    // save
    doc->SaveFile(save_path.c_str());
    return true;

}


// int main()
// {


//     // print_xml_info(xml_info);

//     // save_xml_info(xml_info, "/home/ldq/del/tinyxml2/test.xml");

//     // XMLInfo xml_info = read_xml_info("/home/ldq/input_dir/img_dir/fzc/Dsm082k.xml");
//     XMLInfo xml_info = read_xml_info("/home/ldq/del/tinyxml2/test.xml");

//     print_xml_info(xml_info);
    
    
//     return 0;

// }




#endif
