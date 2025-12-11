// styles.h - CSS样式定义头文件
// 用于存储ESP32智能家居控制系统的网页样式

#ifndef STYLES_H
#define STYLES_H

const char* stylesContent = R"CSS(
body {
    font-family: Arial, sans-serif;
    margin: 0;
    padding: 20px;
    background-color: #f5f5f5;
}
.container {
    max-width: 1200px;
    margin: 0 auto;
}
h1 {
    color: #333;
    text-align: center;
    margin-bottom: 30px;
}
.header {
    background-color: #4CAF50;
    color: white;
    padding: 20px;
    border-radius: 10px;
    margin-bottom: 30px;
    display: flex;
    justify-content: space-between;
    align-items: center;
}
.wifi-status {
    font-size: 14px;
}
.online {
    color: #4CAF50;
}
.offline {
    color: #f44336;
}
.dashboard {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
    gap: 20px;
    margin-bottom: 30px;
}
.card {
    background-color: white;
    padding: 20px;
    border-radius: 10px;
    box-shadow: 0 0 10px rgba(0,0,0,0.1);
}
.card h2 {
    color: #333;
    margin-top: 0;
    margin-bottom: 20px;
    font-size: 20px;
}
.device-list {
    list-style: none;
    padding: 0;
    margin: 0;
}
.device-item {
    margin-bottom: 15px;
    padding: 15px;
    background-color: #f9f9f9;
    border-radius: 5px;
    display: flex;
    justify-content: space-between;
    align-items: center;
}
.device-info {
    flex: 1;
}
.device-name {
    font-weight: bold;
    margin-bottom: 5px;
}
.device-type {
    font-size: 12px;
    color: #666;
}
.device-controls {
    display: flex;
    gap: 10px;
    align-items: center;
}
.toggle-switch {
    position: relative;
    display: inline-block;
    width: 60px;
    height: 34px;
}
.toggle-switch input {
    opacity: 0;
    width: 0;
    height: 0;
}
.slider {
    position: absolute;
    cursor: pointer;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background-color: #ccc;
    transition: .4s;
    border-radius: 34px;
}
.slider:before {
    position: absolute;
    content: "";
    height: 26px;
    width: 26px;
    left: 4px;
    bottom: 4px;
    background-color: white;
    transition: .4s;
    border-radius: 50%;
}
input:checked + .slider {
    background-color: #4CAF50;
}
input:focus + .slider {
    box-shadow: 0 0 1px #4CAF50;
}
input:checked + .slider:before {
    transform: translateX(26px);
}
.brightness-control {
    width: 80px;
}
.environment-data {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    gap: 15px;
}
.data-item {
    text-align: center;
    padding: 15px;
    background-color: #f9f9f9;
    border-radius: 5px;
}
.data-value {
    font-size: 24px;
    font-weight: bold;
    margin-bottom: 5px;
}
.data-label {
    font-size: 12px;
    color: #666;
}
.status-bar {
    background-color: white;
    padding: 15px;
    border-radius: 10px;
    box-shadow: 0 0 10px rgba(0,0,0,0.1);
    text-align: center;
    font-size: 14px;
    color: #666;
}
.update-button {
    background-color: #2196F3;
    color: white;
    border: none;
    padding: 10px 20px;
    border-radius: 5px;
    cursor: pointer;
    font-size: 14px;
    margin-bottom: 20px;
}
.update-button:hover {
    background-color: #0b7dda;
}
)CSS";

#endif /* STYLES_H */