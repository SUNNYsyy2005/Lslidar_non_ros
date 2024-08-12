#pragma once

extern "C"
{
#include "./dora/node_api.h"
}

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <chrono>
#include <thread>
#include <boost/shared_ptr.hpp>

#include "msg.h"

#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <atomic>


namespace Ros {
    
    class Time{
        public:
            static Stamp now() {
                auto now = std::chrono::system_clock::now();
                auto duration = now.time_since_epoch();
                auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
                auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration) - std::chrono::duration_cast<std::chrono::nanoseconds>(seconds);
                return Stamp(seconds.count(), nanoseconds.count());
            }
    };
    static Stamp Time(double t){
        auto seconds = (int)t;
        auto nanoseconds = (int)((t-seconds)*1e9);
        return Stamp(seconds, nanoseconds);
    }
    class Rate{
    public:
        
        Rate(double rate) : packet_rate_(std::chrono::milliseconds(static_cast<int>(rate * 1000))) {
            printf("rate: %f\n", rate);
        }
        Rate() : packet_rate_(std::chrono::milliseconds(100)) {
            printf("rate: 100ms\n");
        }
        void sleep() { 
            std::this_thread::sleep_for(packet_rate_);
        }
    private:
        std::chrono::milliseconds packet_rate_;
    };
    class Publisher {
    public:
        int seq = 0;
        cv::Mat image;
        bool windowCreated = false;
        std::string windowName = "Lidar Scan";
        void drawLidarScan(boost::shared_ptr<sensor_msgs::LaserScan> pkt) {
            auto scan = pkt;
            const int imageSize = 800;
            image = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);
            cv::Point center(imageSize / 2, imageSize / 2);
            double scale = 50.0; // 比例因子，根据需要调整

            // 绘制激光雷达数据
            for (size_t i = 0; i < scan->ranges.size(); ++i) {
                double angle = scan->angle_min + i * scan->angle_increment;
                double range = scan->ranges[i];

                if (range >= scan->range_min && range <= scan->range_max) {
                    int x = static_cast<int>(center.x + range * cos(angle) * scale);
                    int y = static_cast<int>(center.y - range * sin(angle) * scale); // y轴向下为正，逆时针为正方向
                    cv::circle(image, cv::Point(x, y), 2, cv::Scalar(0, 255, 0), -1);
                }
            }
            if(!windowCreated){
                cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
                windowCreated = true;
            }
            // 更新窗口名称
            std::string newWindowName = "Lidar Scan " + std::to_string(seq);
            cv::setWindowTitle(windowName, newWindowName);
            
            cv::imshow(windowName, image);
            cv::waitKey(30);
        }
        void publish(boost::shared_ptr<sensor_msgs::LaserScan> pkt) {    
            // sensor_msgs::LaserSca scan = pkt.px;
            auto scan = pkt;
            
            drawLidarScan(scan);
            seq = scan->header.seq;
            printf("seq: %d\n", scan->header.seq);
            printf("stamp: %lld.%lld\n", scan->header.stamp.sec, scan->header.stamp.nsec);
            printf("frame_id: %s\n", scan->header.frame_id.c_str());
            printf("angle_min: %f\n", scan->angle_min);
            printf("angle_max: %f\n", scan->angle_max);
            printf("angle_increment: %f\n", scan->angle_increment);
            printf("time_increment: %f\n", scan->time_increment);
            printf("scan_time: %f\n", scan->scan_time);
            printf("range_min: %f\n", scan->range_min);
            printf("range_max: %f\n", scan->range_max);
            //printf("ranges: ");
            for (float range : scan->ranges) {
                //printf("%f ", range);
            }
            printf("\n");
            //printf("intensities: ");
            for (float intensity : scan->intensities) {
                //printf("%f ", intensity);
            }
            printf("\n");
            //printf("publish packet\n");
            //std::vector<unsigned char> out_vec = scan->to_json();
            std::string out_id = "scan";
        }
        void publish(boost::shared_ptr< sensor_msgs::PointCloud2> pkt) {
            printf("publish packet\n");
        }
        
    private:
        void* packets_status_pub;
    };
    class ServiceServer {
    public:
        ServiceServer() : packets_status_pub(nullptr) {}
    private:
        void* packets_status_pub;
    };
/*
        pnh.param("pcap", dump_file, std::string(""));
        pnh.param("packet_rate", packet_rate, 1695.0);
        pnh.param<std::string>("frame_id", frame_id, "laser_link");
        pnh.param<bool>("add_multicast", add_multicast, false);
        pnh.param<bool>("pcl_type", pcl_type, false);
        pnh.param("group_ip", group_ip_string, std::string("234.2.3.2"));
        pnh.param("device_ip", lidar_ip_string, std::string("192.168.1.200"));
        pnh.param("msop_port", msop_udp_port, (int) MSOP_DATA_PORT_NUMBER);
        pnh.param("difop_port", difop_udp_port, (int) DIFOP_DATA_PORT_NUMBER);
        pnh.param("point_num", point_num, 2000);
        pnh.param("scan_num", scan_num, 8);
        pnh.param("distance_min", min_range, 0.15);
        pnh.param("distance_max", max_range, 200.0);
        pnh.param("distance_unit", distance_unit, 0.40);
        pnh.param("angle_disable_min", angle_disable_min, 0);
        pnh.param("angle_disable_max", angle_disable_max, 0);
        pnh.param("horizontal_angle_resolution", horizontal_angle_resolution, 0.2);
        pnh.param<bool>("use_time_service", use_time_service, false);
        pnh.param<bool>("publish_scan", publish_scan, false);
        pnh.param<bool>("coordinate_opt", coordinate_opt, false);
        pnh.param<std::string>("pointcloud_topic", pointcloud_topic, "lslidar_point_cloud");
*/
    class NodeHandle {
    public:
        NodeHandle() {
            params_["device_ip"] = std::make_shared<ParamValue<std::string>>("192.168.1.200");
            params_["add_multicast"] = std::make_shared<ParamValue<bool>>(false);
            params_["group_ip"] = std::make_shared<ParamValue<std::string>>("224.1.1.2");
            params_["read_once"] = std::make_shared<ParamValue<bool>>(false);
            params_["read_fast"] = std::make_shared<ParamValue<bool>>(false);
            params_["repeat_delay"] = std::make_shared<ParamValue<double>>(0.0);
            
        }

        template <typename T>
        void param(const std::string& param_name, T& param_val, const T& default_val) {
            auto it = params_.find(param_name);
            if (it != params_.end() && it->second->type() == typeid(T)) {
                param_val = std::static_pointer_cast<ParamValue<T>>(it->second)->value;
            } else {
                param_val = default_val;
            }
        }

    private:
        class IParamValue {
        public:
            virtual ~IParamValue() = default;
            virtual const std::type_info& type() const = 0;
        };

        template <typename T>
        struct ParamValue : IParamValue {
            ParamValue(const T& value) : value(value) {}
            const std::type_info& type() const override { return typeid(T); }
            T value;
        };

        std::unordered_map<std::string, std::shared_ptr<IParamValue>> params_;
    };
}