# 后智 (Houzhi) — 雷达融合预警系统

<p align="center">
  <img src="https://img.shields.io/badge/MCU-STM32F4-03234B?style=for-the-badge&logo=stmicroelectronics">
  <img src="https://img.shields.io/badge/IDE-Keil%20MDK-0071C5?style=for-the-badge&logo=arm">
  <img src="https://img.shields.io/badge/Language-C-555555?style=for-the-badge&logo=c">
  <img src="https://img.shields.io/badge/WiFi-ESP8266-E7352C?style=for-the-badge&logo=espressif">
</p>

<p align="center">
  <b>基于 STM32F4 的多雷达传感器融合预警系统</b><br>
  障碍物检测 · 紧急告警 · 远程数据上云
</p>

---

## 📋 目录

- [项目概述](#项目概述)
- [功能特性](#功能特性)
- [软件架构](#软件架构)
- [快速开始](#快速开始)
- [目录结构](#目录结构)
- [开发环境](#开发环境)

---

## 🚗 项目概述

**后智** 是 STM32F4 平台的雷达融合预警系统，作为车辆辅助安全系统的后端数据处理单元。系统采集多路雷达传感器数据，融合分析后输出障碍物预警信息，并通过 ESP8266 上传至云平台。

与 [**前智 (Qianzhi)**](https://github.com/24t6/qianzhi) 门禁系统配合使用，形成完整的车辆出入+安全预警方案。

---

## ✨ 功能特性

### 📡 传感器融合
- **多雷达数据融合** — 融合前/后多路雷达数据，统一环境感知结果
- **周期采样** — 定时采集各传感器数据，确保实时性

### ⚡ 雷达处理
- **串口数据解析** — 实时解析雷达串口协议
- **障碍物计算** — 计算相对距离、方位角
- **状态机** — 动态维护预警状态，避免误报

### 🔔 告警系统
| 级别 | 触发条件 | 动作 |
|------|---------|------|
| 提示 | 障碍物 > 2m | 蜂鸣器慢响 |
| 预警 | 障碍物 1-2m | 蜂鸣器快响 |
| 紧急 | 障碍物 < 1m | 持续蜂鸣 + 紧急标志上报 |

### ☁️ 数据上云
- **ESP8266** WiFi 联网
- 周期上传传感器数据 + 告警状态
- 支持远程配置下发

---

## 🧠 软件架构

### 主循环流程

```c
int main(void)
{
    App_InitSystem();           // 系统初始化

    while (1)
    {
        App_SampleSensors();         // 采集传感器数据
        App_ProcessRadar();          // 雷达数据解析
        App_UpdateWarningState();    // 预警状态更新
        App_ProcessEmergencyAlarm(); // 紧急告警处理
        App_ProcessUpload();         // 数据上传
        delay_ms(LOOP_DELAY_MS);     // 周期控制
    }
}
```

### 模块调用链

```
App_SampleSensors()
  ├── 读取雷达串口缓冲区
  ├── 读取 GPS 数据
  └── 读取 IO 状态

App_ProcessRadar()
  ├── 校验雷达数据帧
  ├── 解析距离/方位
  └── 存入融合队列

App_UpdateWarningState()
  ├── 读取融合队列
  ├── 状态机判定
  └── 更新预警标志位

App_ProcessEmergencyAlarm()
  ├── 判断紧急标志
  └── 控制蜂鸣器 PWM

App_ProcessUpload()
  ├── 打包 JSON
  ├── ESP8266 MQTT 发布
  └── 日志打印
```

---

## 🚀 快速开始

### 环境要求

- **IDE:** Keil MDK v5.38+
- **芯片:** STM32F4 系列
- **调试器:** ST-Link / J-Link

### 编译与下载

1. 打开 `USER/WHEELTEC.uvprojx` 工程文件
2. 编译 (F7)
3. 下载 (F8)

---

## 📁 目录结构

```
├── APP/                       # 应用层
│   ├── inc/                   # 头文件
│   └── src/                   # 源文件
│       ├── main.c             # 主循环入口
│       ├── app_alarm.c        # 告警管理
│       ├── app_context.c      # 系统上下文
│       ├── app_esp8266.c      # WiFi 通信
│       ├── app_init.c         # 系统初始化
│       ├── app_sensor_fusion.c # 传感器融合
│       └── app_upload.c       # 数据上传
├── BSP/                       # 板级支持包 (STM32F4)
├── CORE/                      # CMSIS 内核
├── FWLIB/                     # STM32F4 标准库
├── FreeRTOS/                  # FreeRTOS 系统
├── STM32/                     # 外设驱动层
│   ├── comm/                  # 串口通信
│   ├── display/               # LCD 显示
│   ├── drivers/               # GPIO/TIM/ADC
│   ├── network/               # ESP8266 驱动
│   ├── sensors/               # 传感器驱动
│   └── timer/                 # 定时器
└── USER/                      # 工程配置
```

---

## 🛠 开发环境

| 工具 | 版本 |
|------|------|
| MCU | STM32F4 系列 |
| IDE | Keil MDK v5.38+ |
| 编译器 | ARMCC v6+ |
| 调试器 | ST-Link / J-Link |
| WiFi | ESP8266 + MQTT |

---

<p align="center">
  <sub>Built with ❤️ using STM32 · 配合 <a href="https://github.com/24t6/qianzhi">前智 (Qianzhi)</a> 使用</sub>
</p>
