// webpage.h - ESP32智能家居控制系统网页定义
// 主HTML页面模板

#ifndef WEBPAGE_H
#define WEBPAGE_H

#include "styles.h"
#include "scripts.h"

const char* webpageContent = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32智能家居控制系统</title>
    <style>
        %STYLES%
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>ESP32智能家居控制系统</h1>
            <div class="wifi-status">
                <span>WiFi状态: <span id="wifi-status" class="offline">未连接</span></span>
                <span style="margin-left: 20px;">IP: <span id="ip-address">-</span></span>
            </div>
        </div>
        
        <button class="update-button" onclick="refreshAll()">手动刷新</button>
        
        <div class="dashboard">
            <!-- 设备控制区域 -->
            <div class="card">
                <h2>设备控制</h2>
                <ul id="device-list" class="device-list">
                    <!-- 设备列表将通过JavaScript动态生成 -->
                </ul>
            </div>
            
            <!-- 环境监测区域 -->
            <div class="card">
                <h2>环境监测</h2>
                <div id="environment-data" class="environment-data">
                    <!-- 环境数据将通过JavaScript动态生成 -->
                </div>
            </div>
        </div>
        
        <div class="status-bar">
            最后更新时间: <span id="last-update">-</span>
        </div>
    </div>
    
    <script>
        %SCRIPTS%
    </script>
</body>
</html>
)HTML";

#endif /* WEBPAGE_H */