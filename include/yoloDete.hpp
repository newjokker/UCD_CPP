#ifndef _YOLODETE_HPP_
#define _YOLODETE_HPP_


#include <opencv2/opencv.hpp>
#include <torch/script.h>
#include <torch/torch.h>
#include <torch/all.h>
#include <algorithm>
#include <iostream>
#include <time.h>
#include <chrono>
#include <getopt.h>
#include <unistd.h>
#include "./deteRes.hpp"
#include "./config_file.hpp"
// #include "./tools.hpp"


auto device = at::kCUDA;
torch::NoGradGuard no_grad;

std::vector<torch::Tensor> non_max_suppression(torch::Tensor preds, float score_thresh = 0.5, float iou_thresh = 0.5)
{
 
	std::vector<torch::Tensor> output;
	for (size_t i = 0; i < preds.sizes()[0]; ++i)
	{
 
		torch::Tensor pred = preds.select(0, i);
 
		// Filter by scores
		torch::Tensor scores = pred.select(1, 4) * std::get<0>(torch::max(pred.slice(1, 5, pred.sizes()[1]), 1));
		pred = torch::index_select(pred, 0, torch::nonzero(scores > score_thresh).select(1, 0));
		if (pred.sizes()[0] == 0) continue;
 
		// (center_x, center_y, w, h) to (left, top, right, bottom)
		pred.select(1, 0) = pred.select(1, 0) - pred.select(1, 2) / 2;
		pred.select(1, 1) = pred.select(1, 1) - pred.select(1, 3) / 2;
		pred.select(1, 2) = pred.select(1, 0) + pred.select(1, 2);
		pred.select(1, 3) = pred.select(1, 1) + pred.select(1, 3);
 
		// Computing scores and classes
		std::tuple<torch::Tensor, torch::Tensor> max_tuple = torch::max(pred.slice(1, 5, pred.sizes()[1]), 1);
		pred.select(1, 4) = pred.select(1, 4) * std::get<0>(max_tuple);
		pred.select(1, 5) = std::get<1>(max_tuple);
		torch::Tensor  dets = pred.slice(1, 0, 6);
		torch::Tensor keep = torch::empty({ dets.sizes()[0] });
		torch::Tensor areas = (dets.select(1, 3) - dets.select(1, 1)) * (dets.select(1, 2) - dets.select(1, 0));
		std::tuple<torch::Tensor, torch::Tensor> indexes_tuple = torch::sort(dets.select(1, 4), 0, 1);
		torch::Tensor v = std::get<0>(indexes_tuple);
		torch::Tensor indexes = std::get<1>(indexes_tuple);
		int count = 0;
		while (indexes.sizes()[0] > 0)
		{
			keep[count] = (indexes[0].item().toInt());
			count += 1;
 
			// Computing overlaps
			torch::Tensor lefts = torch::empty(indexes.sizes()[0] - 1);
			torch::Tensor tops = torch::empty(indexes.sizes()[0] - 1);
			torch::Tensor rights = torch::empty(indexes.sizes()[0] - 1);
			torch::Tensor bottoms = torch::empty(indexes.sizes()[0] - 1);
			torch::Tensor widths = torch::empty(indexes.sizes()[0] - 1);
			torch::Tensor heights = torch::empty(indexes.sizes()[0] - 1);
			
            for (size_t i = 0; i < indexes.sizes()[0] - 1; ++i){
				lefts[i] = std::max(dets[indexes[0]][0].item().toFloat(), dets[indexes[i + 1]][0].item().toFloat());
				tops[i] = std::max(dets[indexes[0]][1].item().toFloat(), dets[indexes[i + 1]][1].item().toFloat());
				rights[i] = std::min(dets[indexes[0]][2].item().toFloat(), dets[indexes[i + 1]][2].item().toFloat());
				bottoms[i] = std::min(dets[indexes[0]][3].item().toFloat(), dets[indexes[i + 1]][3].item().toFloat());
				widths[i] = std::max(float(0), rights[i].item().toFloat() - lefts[i].item().toFloat());
				heights[i] = std::max(float(0), bottoms[i].item().toFloat() - tops[i].item().toFloat());
			}
			torch::Tensor overlaps = widths * heights;
 
			// FIlter by IOUs
			torch::Tensor ious = overlaps / (areas.select(0, indexes[0].item().toInt()) + torch::index_select(areas, 0, indexes.slice(0, 1, indexes.sizes()[0])) - overlaps);
			indexes = torch::index_select(indexes, 0, torch::nonzero(ious <= iou_thresh).select(1, 0) + 1);
		}
		keep = keep.toType(torch::kInt64);
		output.push_back(torch::index_select(dets, 0, keep.slice(0, 0, count)));
	}
	return output;
}


class Yolov5
{
    
	public:
		//
		std::string model_name;
		//
		std::string model_path;
		std::string config_path;
		int img_size;
		float conf_threshold;
		std::vector<std::string> classes;
		torch::jit::script::Module model;
		//
		Yolov5(std::string config_path, std::string option);
		void model_restore();
		DeteRes dete(cv::Mat image);
};


Yolov5::Yolov5(std::string config_path, std::string option)
{   
	// read config
    ConfigFile config_file;
    config_file.parse_config(config_path, option);
	config_file.print_info();
	//
	Yolov5::img_size = config_file.img_size;
	Yolov5::conf_threshold = config_file.conf_threshold;
	Yolov5::model_path = config_file.model_path;
	Yolov5::classes = config_file.classes;
    
    // load model
    Yolov5::model = torch::jit::load(Yolov5::model_path);
	Yolov5::model.to(device);
}


void Yolov5::model_restore()
{	
	
	cv::Mat image(3000, 3000, CV_8UC3, cv::Scalar(0, 0, 0));
	
	std::cout << image.size() << std::endl;
	Yolov5::dete(image);
	std::cout << "* model restore success" << std::endl;
}

DeteRes Yolov5::dete(cv::Mat image_region)
{

	// get dete_res
    DeteRes dete_res;
		
    // resize img 
    cv::Mat image;

	try
	{
    	cv::resize(image_region, image, cv::Size(Yolov5::img_size, Yolov5::img_size));
	}
	catch(std::exception e)
	{
        std::cout<< "resize error" << std::endl;
		return dete_res;
    }
	
    // img -> tensor
    torch::Tensor imgTensor = torch::from_blob(image.data, { 1, image.rows, image.cols, image.channels() }, torch::kByte);
    imgTensor = imgTensor.to(device).permute({ 0,3,1,2 }).contiguous().toType(torch::kFloat).div(255);

    // model dete
    std::vector<torch::jit::IValue> inputs;
    inputs.emplace_back(imgTensor);
    auto output = Yolov5::model.forward(inputs);

    // dete res to cpu
    torch::Tensor preds = output.toTuple()->elements()[0].toTensor();
    preds = preds.to(at::kCPU);
	
    // do nms
    auto dets = non_max_suppression(preds, Yolov5::conf_threshold, 0.5); // 0.35s

    DeteObj dete_obj;
    if (dets.size() > 0)
    {
        // Visualize result
        for (size_t i = 0; i < dets[0].sizes()[0]; ++i)
        {
            float left = dets[0][i][0].item().toFloat();
            float top = dets[0][i][1].item().toFloat(); 
            float right = dets[0][i][2].item().toFloat();
            float bottom = dets[0][i][3].item().toFloat();
            float score = dets[0][i][4].item().toFloat();
            int classID = dets[0][i][5].item().toInt();

            // reshape
            left = left * image_region.cols / 1024;
            top = top * image_region.rows / 1024;
            right = right * image_region.cols / 1024;
            bottom = bottom * image_region.rows / 1024;

			// add dete obj
            dete_obj.tag = Yolov5::classes[classID];
            dete_obj.conf = score;
            dete_obj.x1 = left;
            dete_obj.x2 = right;
            dete_obj.y1 = top;
            dete_obj.y2 = bottom;
			dete_res.add_dete_obj(dete_obj);
			

            // print dete res
            //std::cout << left << ", " << top << ", " << right << ", "  << bottom << ", "  << score  << ", "  << classID << std::endl;
        }
	}
	return dete_res;
}



#endif