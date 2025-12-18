// scripts.h - JavaScript脚本定义头文件
// 用于存储ESP32智能家居控制系统的网页交互逻辑

#ifndef SCRIPTS_H
#define SCRIPTS_H

const char* scriptsContent = R"SCRIPT(
// WebSocket连接
let ws;
let _brightnessTimers = {};

// 初始化
document.addEventListener('DOMContentLoaded', function() {
    initializeWebSocket();
    refreshAll();
    
    // 定期更新数据
    setInterval(refreshAll, 5000);
});

// 初始化WebSocket
function initializeWebSocket() {
    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = protocol + '//' + window.location.host + '/ws';
    
    ws = new WebSocket(wsUrl);
    
    ws.onopen = function() {
        console.log('WebSocket连接已建立');
    };
    
    ws.onmessage = function(event) {
        const data = JSON.parse(event.data);
        handleWebSocketMessage(data);
    };
    
    ws.onclose = function() {
        console.log('WebSocket连接已关闭');
        // 5秒后重连
        setTimeout(initializeWebSocket, 5000);
    };
    
    ws.onerror = function(error) {
        console.error('WebSocket错误:', error);
    };
}

// 处理WebSocket消息
function handleWebSocketMessage(data) {
    if (data.type === 'pong') {
        // 心跳响应
    } else if (data.type === 'deviceUpdate') {
        // 设备状态更新
        updateDevices();
    } else if (data.type === 'environmentUpdate') {
        // 环境数据更新
        updateEnvironmentData();
    }
}

// 刷新所有数据
function refreshAll() {
    updateDevices();
    updateEnvironmentData();
    updateWiFiStatus();
    updateLastUpdateTime();
}

// 更新设备列表
function updateDevices() {
    fetch('/api/devices?t=' + new Date().getTime())
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                const deviceList = document.getElementById('device-list');
                const newDeviceIds = new Set();

                data.devices.forEach(device => {
                    newDeviceIds.add(device.id);
                    let deviceItem = document.getElementById('device-item-' + device.id);

                    if (deviceItem) {
                        // 更新现有设备
                        // 更新开关状态
                        const checkbox = deviceItem.querySelector('input[type="checkbox"]');
                        if (checkbox && checkbox.checked !== device.status) {
                            checkbox.checked = device.status;
                        }

                        // 如果是灯，更新亮度
                        if (device.type === 'light') {
                            const rangeInput = deviceItem.querySelector('input[type="range"]');
                            const label = document.getElementById('brightness-' + device.id);
                            
                            // 只有当用户没有正在拖动该滑块时才更新
                            if (rangeInput && document.activeElement !== rangeInput) {
                                if (parseInt(rangeInput.value) !== device.brightness) {
                                    rangeInput.value = device.brightness;
                                }
                            }
                            
                            if (label) {
                                label.textContent = device.brightness + '%';
                            }
                        }
                    } else {
                        // 创建新设备
                        deviceItem = document.createElement('li');
                        deviceItem.className = 'device-item';
                        deviceItem.id = 'device-item-' + device.id;
                        
                        let deviceHTML = `
                            <div class="device-info">
                                <div class="device-name">${device.name}</div>
                                <div class="device-type">${device.type}</div>
                            </div>
                            <div class="device-controls">
                        `;
                        
                        // 添加开关控制
                        deviceHTML += `
                            <label class="toggle-switch">
                                <input type="checkbox" ${device.status ? 'checked' : ''} 
                                       onchange="toggleDevice('${device.id}', this.checked)">
                                <span class="slider"></span>
                            </label>
                        `;
                        
                        // 如果是智能灯，添加亮度控制
                        if (device.type === 'light') {
                            deviceHTML += `
                                <input type="range" class="brightness-control" 
                                       min="0" max="100" value="${device.brightness}"
                                       oninput="handleBrightnessInput('${device.id}', this.value)"
                                       onchange="setBrightness('${device.id}', this.value)">
                                <span id="brightness-${device.id}">${device.brightness}%</span>
                            `;
                        }
                        
                        deviceHTML += '</div>';
                        deviceItem.innerHTML = deviceHTML;
                        deviceList.appendChild(deviceItem);
                    }
                });

                // 移除已删除的设备
                Array.from(deviceList.children).forEach(child => {
                    const idPart = child.id.replace('device-item-', '');
                    // 确保是设备项且不在新列表中
                    if (child.id.startsWith('device-item-') && !newDeviceIds.has(idPart)) {
                        deviceList.removeChild(child);
                    }
                });
            }
        })
        .catch(error => {
            console.error('获取设备数据失败:', error);
        });
}

// 更新环境数据
function updateEnvironmentData() {
    fetch('/api/environment')
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                const environmentData = document.getElementById('environment-data');
                environmentData.innerHTML = '';
                
                const env = data.environment;
                
                // 温度
                if (env.temperature !== null && env.temperature !== undefined) {
                    addEnvironmentItem(environmentData, '温度', env.temperature.toFixed(1) + '°C', '🌡️');
                }
                
                // 湿度
                if (env.humidity !== null && env.humidity !== undefined) {
                    addEnvironmentItem(environmentData, '湿度', env.humidity.toFixed(1) + '%', '💧');
                }
                
                // 光照强度
                if (env.lightIntensity !== null && env.lightIntensity !== undefined) {
                    addEnvironmentItem(environmentData, '光照', env.lightIntensity.toFixed(0) + ' lux', '☀️');
                }
                
                // 烟雾浓度
                if (env.smokeDensity !== null && env.smokeDensity !== undefined) {
                    addEnvironmentItem(environmentData, '烟雾', env.smokeDensity.toFixed(1) + '%', '🚬');
                }
            }
        })
        .catch(error => {
            console.error('获取环境数据失败:', error);
        });
}

// 添加环境数据项
function addEnvironmentItem(container, label, value, emoji) {
    const dataItem = document.createElement('div');
    dataItem.className = 'data-item';
    dataItem.innerHTML = `
        <div class="data-label">${label}</div>
        <div class="data-value">${emoji} ${value}</div>
    `;
    container.appendChild(dataItem);
}

// 更新WiFi状态
function updateWiFiStatus() {
    fetch('/api/wifi?t=' + new Date().getTime())
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                const wifi = data.wifi;
                const wifiStatus = document.getElementById('wifi-status');
                const ipAddress = document.getElementById('ip-address');
                
                wifiStatus.textContent = wifi.connected ? '已连接' : '未连接';
                wifiStatus.className = wifi.connected ? 'online' : 'offline';
                ipAddress.textContent = wifi.ip;
            }
        })
        .catch(error => {
            console.error('获取WiFi状态失败:', error);
        });
}

// 更新最后更新时间
function updateLastUpdateTime() {
    const now = new Date();
    const formattedTime = now.toLocaleString('zh-CN');
    document.getElementById('last-update').textContent = formattedTime;
}

// 切换设备状态
function toggleDevice(deviceId, status) {
    const deviceData = {
        id: deviceId,
        status: status
    };
    
    fetch('/api/devices/control', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(deviceData)
    })
    .then(response => response.json())
    .then(data => {
        if (data.status !== 'success') {
            // 如果控制失败，恢复原来的状态
            updateDevices();
        }
    })
    .catch(error => {
        console.error('设备控制失败:', error);
        // 如果控制失败，恢复原来的状态
        updateDevices();
    });
}

// 设置智能灯亮度
function setBrightness(deviceId, brightness) {
    const deviceData = {
        id: deviceId,
        brightness: parseInt(brightness)
    };
    
    fetch('/api/devices/control', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(deviceData)
    }).then(() => {
        const label = document.getElementById(`brightness-${deviceId}`);
        if (label) label.textContent = `${parseInt(brightness)}%`;
    }).catch(error => {
        console.error('亮度控制失败:', error);
        // 如果控制失败，恢复原来的亮度
        updateDevices();
    });
}

function handleBrightnessInput(deviceId, brightness) {
    const label = document.getElementById(`brightness-${deviceId}`);
    if (label) label.textContent = `${parseInt(brightness)}%`;
    if (_brightnessTimers[deviceId]) {
        clearTimeout(_brightnessTimers[deviceId]);
    }
    _brightnessTimers[deviceId] = setTimeout(() => {
        setBrightness(deviceId, brightness);
        delete _brightnessTimers[deviceId];
    }, 200);
}
)SCRIPT";

#endif /* SCRIPTS_H */
