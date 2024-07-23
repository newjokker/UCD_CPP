
# UCD 1.5.9 使用文档


### 主要更新

* 完善的帮助文档

* 使用进度条实现进度监控

* 鲁棒性更强，使用更多的 try-catch 结构，捕获常出现的异常

* 更流畅的更新体验

* 更快的速度，object_info, size_info, uc_list, label_used 包含了数据集中的几乎所有数据，运算几乎不用读取缓存数据，效率更快

* 更完善 关键字组( analysis, cache, convert, exec, filter, fun, info, opt, rename, sync)


### 功能模块介绍

* ucd 功能主要分为下面几个主要模块
  * analysis    分析
  * cache       缓存
  * convert     转换
  * exec        脚本执行
  * filter      过滤
  * info        信息
  * opt         操作
  * rename      重命名（后续会删除）
  * sync        同步


### 帮助

* ucd           查看所有 ucd 支持的关键字
* ucd help      查看所有 ucd 支持的关键字及其中文解释
* ucd help key  查看关键字 key 的详细解释

* [demo] 下面是实例

### debug

* ucd 使用之前是需要手动指定缓存文件夹的路径的(cache_dir)

* 当执行了 ucd 命令之后没有效果，查看是否有对应文件和文件夹的权限

* ucd 处理对象需要使用 uc 命名，如果机器没有联网，使用关键字 fake_uc 给文件赋予临时未入库的 uc


### 实战 

* 其实后续要是实际效果还行的话可以作为标准流程，这样不容易报错

* 周末安全帽数据标注出现问题的原因，就是我们做一件事没有标准流程和统一的工具（有了流程和工具之后要错就错一样的，是系统化的错误比较容易改正，损失也比较容易估计）


#### 安全帽的标注

* ucd 分成几部分 (load, devide, upload)

* 获取待标注图片 (save)

* 标注后结果生成 ucd (from_xml)

* 标注后随机抽样检查 (sub)

* 标注后结果进行合并 (merge)

* 标注结果上传 (upload)


#### 安全帽效果查看

* 标签对齐
  * 去掉多余的标签 (filter_by_tags)
  * 重命名标签 (update_tags)

* 数据集对比(acc)

* 结果修改(drop, filter_by_tags)

* 截图查看(cut_small_img)

* 转为 xml，使用 labelImg 查看 (parse_xml, parse_json)
