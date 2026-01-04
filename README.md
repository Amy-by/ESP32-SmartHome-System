# ESP32智能家居控制系统

## 项目简介

ESP32智能家居控制系统是一个基于ESP32开发板的开源智能家居解决方案，提供设备控制、环境监测、自动化规则和Web界面等功能。系统采用模块化设计，易于扩展和定制，适合物联网爱好者和开发者学习与使用。

## 功能特性

### 🎛️ 设备控制
- **智能灯**：支持开关控制和亮度调节（0-100%）
- **智能开关**：支持开关控制，可用于控制其他设备
- **蜂鸣器报警**：支持报警触发和手动控制
- **状态同步**：所有设备状态实时更新到Web界面

### 🌡️ 环境监测
- **温度监测**：实时显示当前温度（℃）
- **湿度监测**：实时显示当前湿度（%）
- **光照监测**：实时显示当前光照强度（lx）
- **烟雾监测**：实时显示烟雾浓度（ppm）

### 🤖 自动化功能
- **规则引擎**：支持基于环境数据的自动化规则
- **规则持久化**：基于SPIFFS的规则文件存储，系统重启后规则不丢失
- **规则管理API**：完整的RESTful API接口，支持规则的创建、查询、更新和删除
- **告警系统**：支持烟雾浓度超标告警
- **联动控制**：支持告警时自动执行设备操作

### 📊 数据管理
- **数据记录**：支持环境数据和系统日志记录
- **数据持久化**：使用SPIFFS存储配置和日志数据

### 🖥️ Web界面
- **响应式设计**：适配桌面和移动设备
- **实时数据**：通过WebSocket实时更新数据
- **设备控制**：通过Web界面控制所有设备
- **配置管理**：支持系统参数和WiFi配置

## 硬件要求

| 材料名称 | 数量 | 用途 | 推荐型号 |
|---------|------|------|---------|
| ESP32开发板 | 1 | 系统核心 | ESP32-DevKitC |
| USB-C数据线 | 1 | 供电和上传代码 | 支持数据传输的线 |
| DHT11温湿度传感器 | 1 | 测量温湿度 | DHT11 |
| 光敏电阻 | 1 | 测量光照强度 | 5516 |
| MQ-2烟雾传感器 | 1 | 测量烟雾浓度 | MQ-2 |
| 蜂鸣器 | 1 | 告警输出 | 有源蜂鸣器 |
| LED灯 | 1 | 智能灯功能 | 普通3mm或5mm LED |
| 220Ω电阻 | 1 | 保护LED | 色环：红-红-棕-金 |
| 杜邦线 | 若干 | 连接传感器 | 公对母、公对公各5根 |

## 硬件连接

### 引脚定义
| 设备 | 引脚 |
|------|------|
| DHT11温湿度传感器 | GPIO14 |
| 光敏电阻 | GPIO32 |
| MQ-2烟雾传感器 | GPIO33 |
| 蜂鸣器 | GPIO17 |
| 智能灯 | GPIO2 |
| 智能开关 | GPIO12 |

### 连接说明
- 所有传感器需要接3.3V电源和GND
- LED灯通过220Ω电阻连接到GPIO2
- 蜂鸣器直接连接到GPIO17和GND

## 软件安装

### 开发环境
- **PlatformIO**：推荐使用PlatformIO进行开发
- **VSCode**：推荐使用VSCode作为开发编辑器

### 安装步骤
1. 安装VSCode：[https://code.visualstudio.com/](https://code.visualstudio.com/)
2. 在VSCode中安装PlatformIO扩展
3. 克隆或下载本项目
4. 使用VSCode打开项目文件夹
5. PlatformIO会自动安装项目依赖

### 项目配置
编辑`platformio.ini`文件可以配置开发板型号、上传速率等参数：

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200
upload_speed = 921600
```

## 使用说明

### 上传代码
1. 连接ESP32开发板到电脑
2. 在VSCode的PlatformIO面板中点击「Upload」按钮
3. 等待代码上传完成

### 配置WiFi
首次使用或需要修改WiFi配置时：

1. 上传代码后，ESP32会创建一个名为「ESP32-SmartHome」的WiFi热点
2. 连接该热点
3. 在浏览器中输入「192.168.4.1」
4. 进入WiFi配置页面，输入您的WiFi名称和密码
5. 保存后，ESP32会自动重启并连接到您的WiFi网络

### 访问Web界面
1. 上传完成后，ESP32会自动连接到配置的WiFi网络
2. 通过串口监视器（在PlatformIO面板中点击「Monitor」）查看ESP32获取的IP地址
3. 在浏览器中输入该IP地址访问Web管理界面

## 开发说明

### 配置文件
- `include/config.h`：包含系统基本配置参数
- `platformio.ini`：PlatformIO项目配置
- `src/utils/ConfigManager.cpp/h`：配置管理相关代码

### 添加新设备
1. 在`src/devices/`目录下创建新设备类
2. 继承`Device`基类并实现必要的方法
3. 在`src/main.cpp`的`initializeDevices()`函数中注册新设备

### 添加新传感器
1. 在`src/sensors/`目录下创建新传感器类
2. 继承`EnvironmentSensor`基类并实现必要的方法
3. 在`src/main.cpp`的`initializeSensors()`函数中注册新传感器

### 添加新功能
1. 在`src/core/`目录下创建新功能模块
2. 在`src/main.cpp`中初始化并使用新模块

## 故障排除

### 常见问题
1. **无法上传代码**
   - 检查开发板是否正确连接
   - 检查`platformio.ini`中的开发板型号是否正确
   - 尝试按下开发板上的「Boot」按钮再上传

2. **无法连接WiFi**
   - 检查WiFi名称和密码是否正确
   - 确保WiFi是2.4GHz频段（ESP32不支持5GHz WiFi）
   - 尝试通过AP模式重新配置WiFi

3. **Web界面无法访问**
   - 检查ESP32是否已连接到WiFi
   - 确保电脑/手机与ESP32在同一WiFi网络
   - 尝试重启ESP32开发板

4. **传感器数据异常**
   - 检查传感器连接是否正确
   - 检查传感器电源是否正常
   - 尝试重新校准传感器

## 贡献指南

欢迎对本项目进行贡献！如果您有任何改进或新功能想法，可以通过以下方式参与：

1. Fork本项目
2. 创建您的特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交您的更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开一个Pull Request

## 项目结构

```
├── data/                  # SPIFFS文件系统数据
├── docs/                  # 项目文档
├── errors/                # 错误日志
├── include/               # 头文件
├── src/                   # 源代码
│   ├── core/              # 核心功能模块
│   │   ├── AlarmManager.* # 告警管理器
│   │   ├── DataProcessor.* # 数据处理器
│   │   ├── RuleEngine.*   # 规则引擎
│   │   └── Rule.*         # 规则定义
│   ├── devices/           # 设备模块
│   │   ├── Device.*       # 设备基类
│   │   ├── DeviceManager.* # 设备管理器
│   │   ├── SmartLight.*   # 智能灯
│   │   └── SmartSwitch.*  # 智能开关
│   ├── sensors/           # 传感器模块
│   │   ├── EnvironmentManager.* # 环境管理器
│   │   ├── EnvironmentSensor.* # 传感器基类
│   │   ├── TemperatureSensor.* # 温度传感器
│   │   ├── HumiditySensor.* # 湿度传感器
│   │   ├── LightSensor.*  # 光照传感器
│   │   └── SmokeSensor.*  # 烟雾传感器
│   ├── utils/             # 工具模块
│   │   ├── ConfigManager.* # 配置管理器
│   │   ├── DataLogger.*   # 数据记录器
│   │   ├── EEPROMStorage.* # EEPROM存储
│   │   ├── GPIOController.* # GPIO控制器
│   │   ├── SPIFFSStorage.* # SPIFFS存储
│   │   └── WiFiManager.*  # WiFi管理器
│   ├── web/               # Web模块
│   │   ├── WebServer.*    # Web服务器
│   │   ├── scripts.h      # JavaScript脚本
│   │   ├── styles.h       # CSS样式
│   │   └── webpage.h      # Web页面
│   └── main.cpp           # 主程序入口
├── test/                  # 测试代码
├── Makefile               # Makefile
├── platformio.ini         # PlatformIO配置
└── README.md              # 项目说明文档
```

## 许可证

本项目采用MIT许可证，可自由使用和修改。

## 联系方式

如有问题或建议，请通过GitHub Issues反馈。