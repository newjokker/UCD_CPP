
# 混合编程


* refer : https://blog.csdn.net/Lxh19920114/article/details/121878596


### 步骤

* 使用python .pt 转为 torchscript
  * 设置参数可转为 cpu 运行和 gpu 运行两种
  * 需要指定 img_size 貌似指定后无法修改？ 
  
  * 命令 
    * 环境 221 py37
    * python3 export.py --weights ./models/prebase_yolo5_0_4_0.pt --img 100 --include torchscript onnx coreml --device 0

* c++ libtorch + cv + torchscript 跑通 yolo5 模型




