#!/bin/bash

dir="/usr/load_ucd_dir"
if [ ! -d "$dir" ];then
mkdir $dir
fi


for ((i=1; i<=${MULTI_COUNT}; i++))
do
    ./dete_server /usr/save_xml_dir /usr/ucd_cache_dir /usr/dete_log_dir "/usr/load_ucd_dir/load_ucd_dir_${i}" prebase_yolo5_0_5_0 ./config.ini prebase_0_5_0 192.168.3.111 11101 & 
done


while true
do 
    sleep 5
done


# --restart=always

# -e MULTI_COUNT=3

# docker run --gpus device=0 -v /home/disk2/prebase_dete/save_xml_dir:/usr/save_xml_dir -v /home/disk2/prebase_dete/ucd_cache:/usr/ucd_cache_dir -v /home/disk2/prebase_dete/dete_logs:/usr/dete_log_dir -v /home/disk2/prebase_dete/load_ucd_dir:/usr/load_ucd_dir --restart=always  -e MULTI_COUNT=4  -d dete_server:v0.1.6




