# Lslidar_non_ros
改代码为镭神16线激光雷达官方驱动去ros化的版本，旨在供不愿意或没办法安装ros的人提供调试激光雷达的替代方案。

目前该代码只实现了接收雷达数据，将数据转化为2D激光雷达标准格式，并用openCV可视化雷达信息的功能

其他功能，如设置雷达转速、ip等暂未实现

注：官方驱动代码：https://github.com/Lslidar/Lslidar_ROS1_driver.git
## 编译
```sh
mkdir build && cd build
cmake ..
make
```
## 运行
```sh
cd build
./lslidar_cx_driver_node 
```