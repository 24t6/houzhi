# 后智 (Houzhi) — 雷达融合预警系统

基于 **STM32F4** 的雷达传感器融合与预警系统，实现多传感器数据融合、障碍物检测和远程告警。

## 功能特性

### 传感器融合
- 多雷达数据融合，统一环境感知
- 周期采集各传感器数据

### 雷达处理
- 雷达串口数据解析
- 障碍物距离和方位计算
- 动态更新预警状态

### 告警系统
- 碰撞风险紧急告警
- 蜂鸣器分级报警

### 数据上云
- ESP8266 WiFi 联网
- 周期上传传感器数据和告警信息

## 软件架构

main() -> App_InitSystem()
       -> while(1) App_SampleSensors()
                  -> App_ProcessRadar()
                  -> App_UpdateWarningState()
                  -> App_ProcessEmergencyAlarm()
                  -> App_ProcessUpload()

## 目录结构

APP/src/     — 应用源文件
APP/inc/     — 应用头文件
BSP/         — 板级支持包 (STM32F4)
STM32/comm/  — 串口通信
STM32/display/ — LCD 显示
STM32/drivers/ — 基础驱动
STM32/network/ — ESP8266
STM32/sensors/ — 传感器驱动
STM32/timer/   — 定时器

## 开发环境

MCU: STM32F4 | IDE: Keil MDK
