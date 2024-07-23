# 常用命令



### git 在命令行常用的操作


* git log --pretty=format:"%h %s"


### apt 命令

* apt-cache search curl-dev ，根据关键字查找需要的包

* dpkg -l | grep curl , 查看已安装的包，curl 关键字相关的

* apt-get autoremove package_name
* refer : https://blog.csdn.net/get_set/article/details/51276609



### 常用命令

* 后台跑所有的数据， nouhp command > log_path &
* nohup ./data/JoTorch --img_txt_path data/uc.txt  --model_path ./data/prebase_yolo5_0_4_0.torchscript --save_dir ./res/ --config_path ./data/config.ini --save_xml > prebase.log &* 


### 产品相关的命令

* 跑prebase 和 prebasepd
* cd /home/disk2
* nohup ./data/JoTorch --img_txt_path data/uc.txt  --model_path ./data/prebase_yolo5_0_4_0.torchscript --save_dir ./res/ --config_path ./data/config.ini --save_xml > prebase.log &
* nohup ./data/JoTorch --model_path ./data/prebasepd_yolov5_0_0_1.torchscript --img_txt_path ./data/uc.txt  --config_path ./data/config_prebasepd.ini --save_dir ./res_pd/ --save_xml > prebasepd.log &

