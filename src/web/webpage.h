// webpage.h - ESP32智能家居控制系统网页定义
// 主HTML页面模板

#ifndef WEBPAGE_H
#define WEBPAGE_H

#include "styles.h"

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
    <nav class="navbar">
        <div class="container">
            <div class="navbar-brand">
                <h1>ESP32智能家居控制系统</h1>
            </div>
            
            <div class="navbar-toggle" id="navbarToggle">
                <span></span>
                <span></span>
                <span></span>
            </div>
            
            <div class="navbar-menu" id="navbarMenu">
                <div class="navbar-nav">
                    <a href="#devices" class="nav-link active">设备控制</a>
                    <a href="#environment" class="nav-link">环境监测</a>
                    <a href="#rules" class="nav-link">规则管理</a>
                    <a href="#alarms" class="nav-link">告警设置</a>
                </div>
                
                <div class="navbar-status">
                    <div class="wifi-status">
                        <span>WiFi状态: <span id="wifi-status" class="offline">未连接</span></span>
                        <span style="margin-left: 20px;">IP: <span id="ip-address">-</span></span>
                    </div>
                </div>
            </div>
        </div>
    </nav>
    
    <div class="container">
        
        <button class="update-button" onclick="refreshAll()">手动刷新</button>
        
        <div class="dashboard">
            <!-- 设备控制区域 -->
            <div id="devices" class="card">
                <h2>设备控制</h2>
                <ul id="device-list" class="device-list">
                    <!-- 设备列表将通过JavaScript动态生成 -->
                </ul>
            </div>
            
            <!-- 环境监测区域 -->
            <div id="environment" class="card">
                <h2>环境监测</h2>
                <div id="environment-data" class="environment-data">
                    <!-- 环境数据将通过JavaScript动态生成 -->
                </div>
            </div>
            
            <!-- 告警设置区域 -->
            <div id="alarms" class="card">
                <h2>告警设置</h2>
                <div id="alarm-settings">
                    <div class="device-item" style="align-items: flex-start;">
                        <div class="device-info" style="min-width: 140px; padding-top: 8px;">
                            <div class="device-name">烟雾报警阈值设置</div>
                            <div class="device-type">传感器：smoke</div>
                        </div>
                        
                        <div style="flex: 1; margin-left: 20px;">
                            <!-- 第一行：当前设置状态 -->
                            <div class="status-bar" style="text-align: left; padding: 10px 15px; margin-bottom: 15px; background-color: #e8f5e9; color: #2e7d32; border: 1px solid #c8e6c9;">
                                <strong>当前生效配置：</strong> <span id="smoke-current">加载中...</span>
                            </div>

                            <!-- 第二行：编辑控制区域 -->
                            <div style="display: flex; align-items: flex-end; gap: 15px; flex-wrap: wrap;">
                                <div style="display: flex; flex-direction: column;">
                                    <label class="control-label" style="font-size: 12px; color: #666;">最小阈值 (Min)</label>
                                    <input type="number" id="smoke-min" min="0" max="4095" step="1" class="full-width-input" placeholder="无" style="width: 100px; padding: 8px;">
                                </div>
                                
                                <div style="display: flex; flex-direction: column;">
                                    <label class="control-label" style="font-size: 12px; color: #666;">最大阈值 (Max)</label>
                                    <input type="number" id="smoke-max" min="0" max="4095" step="1" class="full-width-input" placeholder="500" style="width: 100px; padding: 8px;">
                                </div>

                                <div style="display: flex; flex-direction: column; align-items: flex-start; padding-bottom: 5px;">
                                    <label class="control-label" style="font-size: 12px; color: #666; margin-bottom: 5px;">启用告警</label>
                                    <select id="smoke-enabled-select" class="full-width-input" style="width: 100px; padding: 8px;">
                                        <option value="1">启用</option>
                                        <option value="0">禁用</option>
                                    </select>
                                </div>

                                <button class="update-button" onclick="saveSmokeThreshold()" style="margin: 0; height: 38px; margin-bottom: 2px;">保存设置</button>
                                
                                <div style="font-size: 12px; color: #888; margin-bottom: 10px; flex: 1; min-width: 200px; text-align: right; align-self: center;">
                                    建议阈值500 (ADC/ppm)
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
            
            <!-- 规则管理区域 -->
            <div id="rules" class="card">
                <h2>规则管理</h2>
                
                <!-- 规则列表 -->
                <div id="rules-list-container" class="rules-list-container">
                    <h3>现有规则</h3>
                    <ul id="rules-list" class="rules-list">
                        <!-- 规则列表将通过JavaScript动态生成 -->
                    </ul>
                </div>
                
                <!-- 规则编辑表单 -->
                <div id="rule-form-container" class="rule-form-container">
                    <h3>创建/编辑规则</h3>
                    <form id="rule-form" class="rule-form">
                        <input type="hidden" id="rule-id" value="">
                        
                        <div class="form-row">
                            <label for="rule-name">规则名称:</label>
                            <input type="text" id="rule-name" name="name" placeholder="输入规则名称" required>
                        </div>
                        
                        <div class="form-row">
                            <label for="rule-enabled">启用规则:</label>
                            <input type="checkbox" id="rule-enabled" name="enabled" checked>
                        </div>
                        
                        <div class="form-row">
                            <label>条件:</label>
                            <div id="rule-conditions" class="rule-conditions">
                                <!-- 条件将通过JavaScript动态添加 -->
                            </div>
                            <button type="button" class="add-button" onclick="addCondition()">添加条件</button>
                        </div>
                        
                        <div class="form-row">
                            <label>动作:</label>
                            <div id="rule-actions" class="rule-actions">
                                <!-- 动作将通过JavaScript动态添加 -->
                            </div>
                            <button type="button" class="add-button" onclick="addAction()">添加动作</button>
                        </div>
                        
                        <div class="form-row">
                            <button type="submit" class="update-button">保存规则</button>
                            <button type="button" class="cancel-button" onclick="clearRuleForm()">清空</button>
                        </div>
                    </form>
                </div>
            </div>
        </div>
        
        <div class="status-bar">
            最后更新时间: <span id="last-update">-</span>
        </div>
    </div>
    
    <script>
        // ===== JS 开始（直接内联） =====
        // WebSocket连接
        let ws;
        let _brightnessTimers = {};
        let _recentControls = {};
        let MINIMAL_SWITCH_MODE = false;
        document.addEventListener('DOMContentLoaded', function() {
            console.log('JS loaded');
            initializeWebSocket();
            refreshAll();
            setInterval(refreshAll, 5000);
            
            // 导航栏交互功能
            initNavbar();
            
            const enSelect = document.getElementById('smoke-enabled-select');
            if (enSelect) {
                enSelect.addEventListener('change', () => {
                    saveSmokeThreshold();
                });
            }
            const minInput = document.getElementById('smoke-min');
            const maxInput = document.getElementById('smoke-max');
            let _alarmSaveTimer = null;
            const scheduleSave = () => {
                if (_alarmSaveTimer) clearTimeout(_alarmSaveTimer);
                _alarmSaveTimer = setTimeout(() => { saveSmokeThreshold(); _alarmSaveTimer = null; }, 500);
            };
            [minInput, maxInput].forEach(el => {
                if (!el) return;
                el.addEventListener('input', scheduleSave);
                el.addEventListener('change', () => saveSmokeThreshold());
                el.addEventListener('blur', () => saveSmokeThreshold());
            });
        });
        
        // 导航栏初始化函数
        function initNavbar() {
            const navbarToggle = document.getElementById('navbarToggle');
            const navbarMenu = document.getElementById('navbarMenu');
            const navLinks = document.querySelectorAll('.nav-link');
            
            // 移动端菜单切换
            if (navbarToggle && navbarMenu) {
                navbarToggle.addEventListener('click', () => {
                    navbarMenu.classList.toggle('active');
                    
                    // 旋转汉堡菜单图标
                    const spans = navbarToggle.querySelectorAll('span');
                    spans[0].style.transform = navbarMenu.classList.contains('active') ? 'rotate(45deg) translate(5px, 5px)' : 'none';
                    spans[1].style.opacity = navbarMenu.classList.contains('active') ? '0' : '1';
                    spans[2].style.transform = navbarMenu.classList.contains('active') ? 'rotate(-45deg) translate(7px, -6px)' : 'none';
                });
            }
            
            // 导航链接点击事件
            navLinks.forEach(link => {
                link.addEventListener('click', (e) => {
                    // 移除所有链接的active类
                    navLinks.forEach(l => l.classList.remove('active'));
                    // 添加当前链接的active类
                    link.classList.add('active');
                    
                    // 移动端点击后关闭菜单
                    if (navbarMenu && navbarMenu.classList.contains('active')) {
                        navbarMenu.classList.remove('active');
                        const spans = navbarToggle.querySelectorAll('span');
                        spans.forEach(span => {
                            span.style.transform = 'none';
                            span.style.opacity = '1';
                        });
                    }
                    
                    // 平滑滚动到目标位置
                    const targetId = link.getAttribute('href');
                    if (targetId.startsWith('#')) {
                        e.preventDefault();
                        const targetElement = document.querySelector(targetId);
                        if (targetElement) {
                            const navbarHeight = document.querySelector('.navbar').offsetHeight;
                            const targetPosition = targetElement.getBoundingClientRect().top + window.pageYOffset - navbarHeight;
                            
                            window.scrollTo({
                                top: targetPosition,
                                behavior: 'smooth'
                            });
                        }
                    }
                });
            });
            
            // 滚动时高亮当前导航项
            window.addEventListener('scroll', () => {
                const sections = document.querySelectorAll('#devices, #environment, #rules, #alarms');
                const navbarHeight = document.querySelector('.navbar').offsetHeight;
                
                let currentSection = '';
                sections.forEach(section => {
                    const sectionTop = section.offsetTop - navbarHeight - 100;
                    const sectionHeight = section.offsetHeight;
                    if (window.pageYOffset >= sectionTop) {
                        currentSection = '#' + section.getAttribute('id');
                    }
                });
                
                navLinks.forEach(link => {
                    link.classList.remove('active');
                    if (link.getAttribute('href') === currentSection) {
                        link.classList.add('active');
                    }
                });
            });
        }
        function initializeWebSocket() {
            const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
            const wsUrl = protocol + '//' + window.location.host + '/ws';
            ws = new WebSocket(wsUrl);
            ws.onopen = function() { console.log('WebSocket连接已建立'); };
            ws.onmessage = function(event) {
                const data = JSON.parse(event.data);
                handleWebSocketMessage(data);
            };
            ws.onclose = function() {
                console.log('WebSocket连接已关闭');
                setTimeout(initializeWebSocket, 5000);
            };
        }
        function handleWebSocketMessage(data) {
            if (data.type === 'deviceUpdate') {
                const dev = data.device || {};
                _recentControls[dev.id] = Date.now();
                const li = document.getElementById('device-item-' + dev.id);
                if (li) updateDeviceElement(li, dev, 'ws');
                else updateDevices();
            }
        }
        const UISwitch = {
            norm(v) { return v === true || v === 'true' || v === 1; },
            set(li, deviceId, status, source) {
                const checkbox = document.getElementById('toggle-' + deviceId) || li.querySelector('input[type=\"checkbox\"]');
                const toggle = li.querySelector('label.toggle-switch');
                const slider = li.querySelector('.slider');
                const last = _recentControls[deviceId] || 0;
                const isRecent = (Date.now() - last) < 1500;
                if (checkbox) {
                    if (!(source === 'fetch' && isRecent && checkbox.checked !== status)) {
                        checkbox.checked = status;
                    }
                }
                if (toggle) {
                    toggle.classList.toggle('on', status);
                }
                if (slider) {
                    slider.style.backgroundColor = status ? '#4CAF50' : '#ccc';
                }
            },
            attach(toggle, deviceId) {
                const checkbox = toggle.querySelector('input');
                checkbox.addEventListener('change', (e) => {
                    const checked = !!e.target.checked;
                    UISwitch.set(toggle.parentElement.parentElement, deviceId, checked, 'user');
                    toggleDevice(deviceId, checked);
                });
            }
        };
        function refreshAll() {
            updateDevices();
            updateEnvironmentData();
            updateWiFiStatus();
            updateLastUpdateTime();
            updateAlarmSettings();
            updateRules();
        }
        function updateWiFiStatus() {
            return fetch('/api/wifi', { cache: 'no-store' })
                .then(response => response.json())
                .then(data => {
                    if (data.status === 'success') {
                        const wifi = data.wifi;
                        const wifiStatus = document.getElementById('wifi-status');
                        const ipAddress = document.getElementById('ip-address');
                        wifiStatus.textContent = wifi.connected ? '已连接' : '未连接';
                        wifiStatus.className = wifi.connected ? 'online' : 'offline';
                        ipAddress.textContent = wifi.ip || '-';
                    }
                })
                .catch(err => {
                    console.warn('updateWiFiStatus failed:', err);
                    const wifiStatus = document.getElementById('wifi-status');
                    const ipAddress = document.getElementById('ip-address');
                    if (wifiStatus) { wifiStatus.textContent = '未知'; wifiStatus.className = 'offline'; }
                    if (ipAddress) { ipAddress.textContent = '-'; }
                });
        }
        function updateDevices() {
            fetch('/api/devices?t=' + new Date().getTime(), { cache: 'no-store' })
                .then(r => r.json())
                .then(data => {
                    if (data.status !== 'success') return;
                    const list = document.getElementById('device-list');
                    const currentIds = new Set();
                    
                    data.devices.forEach(device => {
                        const itemId = 'device-item-' + device.id;
                        currentIds.add(itemId);
                        let li = document.getElementById(itemId);
                        
                        if (li) {
                            updateDeviceElement(li, device, 'fetch');
                        } else {
                            // 创建新设备
                            li = createDeviceElement(device);
                            li.id = itemId;
                            list.appendChild(li);
                        }
                    });
                    
                    // 移除已删除的设备
                    Array.from(list.children).forEach(child => {
                        if (child.id && child.id.startsWith('device-item-') && !currentIds.has(child.id)) {
                            list.removeChild(child);
                        }
                    });
                })
                .catch(err => console.error('获取设备数据失败:', err));
        }
        
        function createDeviceElement(device) {
            const li = document.createElement('li');
            li.className = 'device-item';
            
            const info = document.createElement('div');
            info.className = 'device-info';
            const displayName = (() => {
                const id = device.id || '';
                const type = device.type || '';
                const parts = id.split('_');
                const suffix = parts.length > 1 ? parts[1] : '';
                if (id.startsWith('buzzer_') || type === 'buzzer') return '蜂鸣器_' + suffix;
                if (id.startsWith('light_') || type === 'light') return '智能灯_' + suffix;
                if (id.startsWith('switch_') || type === 'switch') return '智能开关_' + suffix;
                return id;
            })();
            
            // 保存设备ID到 dataset 方便后续访问
            li.dataset.deviceId = device.id;
            
            info.innerHTML = `
                <div class="device-name">${displayName}</div>
                <div class="device-type">${device.type}</div>
            `;
            
            const controls = document.createElement('div');
            controls.className = 'device-controls';
            
            // 开关控件
            const toggle = document.createElement('label');
            toggle.className = 'toggle-switch';
            toggle.innerHTML = `
                <input type="checkbox">
                <span class="slider"></span>
            `;
            const checkbox = toggle.querySelector('input');
            checkbox.id = 'toggle-' + device.id;
            UISwitch.set(li, device.id, UISwitch.norm(device.status), 'init');
            UISwitch.attach(toggle, device.id);
            controls.appendChild(toggle);
            
            // 智能灯亮度控件
            if (device.type === 'light') {
                const b = (device.brightness !== undefined && device.brightness !== null) ? Number(device.brightness) : 0;
                
                const range = document.createElement('input');
                range.type = 'range';
                range.className = 'brightness-control';
                range.min = '0';
                range.max = '100';
                range.value = b;
                
                const label = document.createElement('span');
                label.className = 'brightness-label';
                label.textContent = b + '\x25';
                
                controls.appendChild(range);
                controls.appendChild(label);
                
                range.addEventListener('input', (e) => {
                    const val = Number(e.target.value);
                    label.textContent = val + '\x25';
                    if (_brightnessTimers[device.id]) {
                        clearTimeout(_brightnessTimers[device.id]);
                    }
                    _brightnessTimers[device.id] = setTimeout(() => {
                        controlDevice(device.id, { brightness: val });
                        delete _brightnessTimers[device.id];
                    }, 100);
                });
                range.addEventListener('change', (e) => {
                    const val = Number(e.target.value);
                    label.textContent = val + '\x25';
                    controlDevice(device.id, { brightness: val });
                });
            }
            
            if ((device.id || '').startsWith('buzzer_')) {
                const container = document.createElement('div');
                container.style.cssText = 'display:flex; flex-direction:column; align-items:flex-start; margin-left:12px;';
                
                const row1 = document.createElement('div');
                row1.style.cssText = 'display:flex; align-items:center;';
                
                const durationInput = document.createElement('input');
                durationInput.type = 'number';
                durationInput.value = '500';
                durationInput.min = '100';
                durationInput.max = '5000';
                durationInput.step = '100';
                durationInput.style.cssText = 'width:70px; padding:5px; border-radius:4px; border:1px solid #ccc;';
                
                const unitLabel = document.createElement('span');
                unitLabel.textContent = 'ms';
                unitLabel.style.cssText = 'margin-left:5px; font-size:14px;';
                
                const confirmBtn = document.createElement('button');
                confirmBtn.textContent = '鸣叫';
                confirmBtn.className = 'update-button';
                confirmBtn.style.cssText = 'padding:5px 12px; margin-left:10px; margin-bottom:0;';
                confirmBtn.onclick = () => {
                    const val = parseInt(durationInput.value);
                    if (val > 0) beepOnce(device.id, val);
                };
                
                row1.appendChild(durationInput);
                row1.appendChild(unitLabel);
                row1.appendChild(confirmBtn);
                
                const hint = document.createElement('div');
                hint.textContent = '建议时长: 300-1000ms';
                hint.style.cssText = 'font-size:12px; color:#888; margin-top:4px;';
                
                container.appendChild(row1);
                container.appendChild(hint);
                controls.appendChild(container);
            }
            
            li.appendChild(info);
            li.appendChild(controls);
            return li;
        }
        
        function updateDeviceElement(li, device, source) {
            const status = UISwitch.norm(device && device.status);
            UISwitch.set(li, device.id, status, source);
            
            // 更新亮度
            if (device.type === 'light') {
                const range = li.querySelector('input.brightness-control');
                const label = li.querySelector('.brightness-label');
                
                // 只有当用户没有正在拖动滑块时才更新，避免冲突
                if (range && document.activeElement !== range) {
                    const b = (device.brightness !== undefined && device.brightness !== null) ? Number(device.brightness) : 0;
                    if (parseInt(range.value) !== b) {
                        range.value = b;
                    }
                }
                
                if (label) {
                    const b = (device.brightness !== undefined && device.brightness !== null) ? Number(device.brightness) : 0;
                    label.textContent = b + '\x25';
                }
            }
        }

        function controlDevice(deviceId, params) {
            const body = Object.assign({ id: deviceId }, params);
            fetch('/api/devices/control', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(body)
            })
            .then(r => r.json())
            .then(() => {
                console.log('Control success:', deviceId);
                _recentControls[deviceId] = Date.now();
            })
            .catch(err => {
                console.error('设备控制失败:', err);
                // 失败时刷新列表以恢复正确状态
                updateDevices();
            });
        }
        
        function toggleDevice(deviceId, status) {
            controlDevice(deviceId, { status: status });
        }
        
        function beepOnce(deviceId, duration) {
            controlDevice(deviceId, { status: true });
            setTimeout(() => {
                controlDevice(deviceId, { status: false });
            }, typeof duration === 'number' ? duration : 500);
        }
        function updateEnvironmentData() {
            return fetch('/api/environment', { cache: 'no-store' })
                .then(r => r.json())
                .then(data => {
                    if (data.status !== 'success') return;
                    const env = data.environment || {};
                    const box = document.getElementById('environment-data');
                    if (!box) return;
                    
                    // 强制应用容器样式 (Flex Column)
                    box.style.display = 'flex';
                    box.style.flexDirection = 'column';
                    box.style.gap = '10px';
                    
                    box.innerHTML = '';
                    const fmt = (v, digits, unit) =>
                        (v === null || v === undefined || Number.isNaN(v)) ? '--' : (Number(v).toFixed(digits) + unit);
                    
                    // 注意：这里使用 \x25 代替 % 以避免模板引擎冲突
                    addEnvironmentItem(box, '温度', fmt(env.temperature, 1, '°C'), '🌡️');
                    addEnvironmentItem(box, '湿度', fmt(env.humidity, 1, '\x25'), '💧');
                    addEnvironmentItem(box, '光照', fmt(env.lightIntensity, 0, ' adc'), '☀️');
                    addEnvironmentItem(box, '烟雾', fmt(env.smokeDensity, 0, ''), '🚬');
                })
                .catch(err => console.error('updateEnvironmentData failed:', err));
        }

        function addEnvironmentItem(container, label, value, emoji) {
            const div = document.createElement('div');
            div.className = 'data-item';
            
            // 使用内联样式强制覆盖，确保左右布局生效
            // justify-content: space-between 确保 label 在左，value 在右
            div.style.cssText = 'display:flex; justify-content:space-between; align-items:center; padding:15px 20px; background-color:#f9f9f9; border-radius:8px; margin-bottom:0; width:100\x25; box-sizing:border-box; transition: background-color 0.2s;';
            
            div.onmouseover = function() { this.style.backgroundColor = '#f0f0f0'; };
            div.onmouseout = function() { this.style.backgroundColor = '#f9f9f9'; };

            // 交换 label 和 value 的 DOM 顺序：Label在前(左)，Value在后(右)
            div.innerHTML = `
                <div class="data-label" style="font-size:16px; color:#555; font-weight:500;">${label}</div>
                <div class="data-value" style="font-size:18px; color:#333; font-weight:bold; font-family:monospace;">${emoji} ${value}</div>
            `;
            container.appendChild(div);
        }
        
        function updateAlarmSettings() {
            return fetch('/api/alarm', { cache: 'no-store' })
                .then(r => r.json())
                .then(data => {
                    if (data.status !== 'success') return;
                    const alarm = data.alarm || {};
                    const minInput = document.getElementById('smoke-min');
                    const maxInput = document.getElementById('smoke-max');
                    const enSelect = document.getElementById('smoke-enabled-select');
                    const curr = document.getElementById('smoke-current');
                    if (minInput && document.activeElement !== minInput) {
                        minInput.value = (alarm.minThreshold !== undefined && alarm.minThreshold !== -1) ? Number(alarm.minThreshold) : '';
                    }
                    if (maxInput && document.activeElement !== maxInput) {
                        maxInput.value = (alarm.maxThreshold !== undefined && alarm.maxThreshold !== -1) ? Number(alarm.maxThreshold) : '';
                    }
                    if (enSelect) { enSelect.value = alarm.enabled ? '1' : '0'; }
                    if (curr) {
                        const minText = (alarm.minThreshold !== undefined && alarm.minThreshold !== -1) ? String(alarm.minThreshold) : '无下限';
                        const maxText = (alarm.maxThreshold !== undefined && alarm.maxThreshold !== -1) ? String(alarm.maxThreshold) : '无上限';
                        curr.textContent = `最小=${minText}，最大=${maxText}，启用=${alarm.enabled ? '是' : '否'}`;
                    }
                })
                .catch(err => console.error('updateAlarmSettings failed:', err));
        }
        
        function saveSmokeThreshold() {
            const minInput = document.getElementById('smoke-min');
            const maxInput = document.getElementById('smoke-max');
            const enSelect = document.getElementById('smoke-enabled-select');
            const minThreshold = Number(minInput.value);
            const maxThreshold = Number(maxInput.value);
            const enabled = (enSelect && enSelect.value === '1');
            const body = {
                sensorId: 'smoke',
                minThreshold: isNaN(minThreshold) ? -1 : minThreshold,
                maxThreshold: isNaN(maxThreshold) ? -1 : maxThreshold,
                enabled: enabled
            };
            fetch('/api/alarm', {
                method: 'PUT',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(body)
            })
            .then(r => r.json())
            .then(() => updateAlarmSettings())
            .catch(err => console.error('saveSmokeThreshold failed:', err));
        }
        function updateLastUpdateTime() {
            document.getElementById('last-update').textContent =
                new Date().toLocaleString();
        }
        
        // ===== 规则管理相关函数 =====
        function updateRules() {
            fetch('/api/rules', { cache: 'no-store' })
                .then(r => r.json())
                .then(data => {
                    if (data.status !== 'success') return;
                    
                    const rulesList = document.getElementById('rules-list');
                    rulesList.innerHTML = '';
                    
                    data.rules.forEach(rule => {
                        const li = document.createElement('li');
                        li.className = 'rule-item';
                        
                        const ruleInfo = document.createElement('div');
                        ruleInfo.className = 'rule-info';
                        ruleInfo.innerHTML = `
                            <div class="rule-name">${rule.name}</div>
                            <div class="rule-status">${rule.enabled ? '启用' : '禁用'}</div>
                            <div class="rule-description">
                                条件: ${rule.conditions.length}个 | 动作: ${rule.actions.length}个
                            </div>
                        `;
                        
                        const ruleActions = document.createElement('div');
                        ruleActions.className = 'rule-actions-buttons';
                        
                        const editBtn = document.createElement('button');
                        editBtn.className = 'edit-button';
                        editBtn.textContent = '编辑';
                        editBtn.onclick = () => editRule(rule);
                        
                        const deleteBtn = document.createElement('button');
                        deleteBtn.className = 'delete-button';
                        deleteBtn.textContent = '删除';
                        deleteBtn.onclick = () => deleteRule(rule.id);
                        
                        ruleActions.appendChild(editBtn);
                        ruleActions.appendChild(deleteBtn);
                        
                        li.appendChild(ruleInfo);
                        li.appendChild(ruleActions);
                        rulesList.appendChild(li);
                    });
                })
                .catch(err => console.error('updateRules failed:', err));
        }
        
        function addCondition() {
            const conditionsContainer = document.getElementById('rule-conditions');
            const conditionIndex = conditionsContainer.children.length;
            
            const conditionDiv = document.createElement('div');
            conditionDiv.className = 'condition-item';
            conditionDiv.innerHTML = `
                <div class="condition-inputs">
                    <input type="text" name="condition_device_${conditionIndex}" placeholder="设备ID (如: light1)" required>
                    <select name="condition_type_${conditionIndex}" required>
                        <option value="value">数值比较</option>
                        <option value="state">状态比较</option>
                    </select>
                    <input type="text" name="condition_value_${conditionIndex}" placeholder="比较值" required>
                    <select name="condition_operator_${conditionIndex}" required>
                        <option value=">">大于</option>
                        <option value="<">小于</option>
                        <option value="==">等于</option>
                        <option value=">=">大于等于</option>
                        <option value="<=">小于等于</option>
                    </select>
                </div>
                <button type="button" class="remove-button" onclick="removeCondition(this)">移除</button>
            `;
            
            conditionsContainer.appendChild(conditionDiv);
        }
        
        function removeCondition(button) {
            const conditionItem = button.parentElement;
            conditionItem.remove();
        }
        
        function addAction() {
            const actionsContainer = document.getElementById('rule-actions');
            const actionIndex = actionsContainer.children.length;
            
            const actionDiv = document.createElement('div');
            actionDiv.className = 'action-item';
            actionDiv.innerHTML = `
                <div class="action-inputs">
                    <input type="text" name="action_device_${actionIndex}" placeholder="设备ID (如: light1)" required>
                    <input type="text" name="action_value_${actionIndex}" placeholder="目标值 (如: 1或0)" required>
                </div>
                <button type="button" class="remove-button" onclick="removeAction(this)">移除</button>
            `;
            
            actionsContainer.appendChild(actionDiv);
        }
        
        function removeAction(button) {
            const actionItem = button.parentElement;
            actionItem.remove();
        }
        
        function clearRuleForm() {
            document.getElementById('rule-form').reset();
            document.getElementById('rule-id').value = '';
            document.getElementById('rule-conditions').innerHTML = '';
            document.getElementById('rule-actions').innerHTML = '';
        }
        
        function editRule(rule) {
            clearRuleForm();
            
            document.getElementById('rule-id').value = rule.id;
            document.getElementById('rule-name').value = rule.name;
            document.getElementById('rule-enabled').checked = rule.enabled;
            
            const conditionsContainer = document.getElementById('rule-conditions');
            rule.conditions.forEach(condition => {
                addCondition();
                const conditionIndex = conditionsContainer.children.length - 1;
                
                document.querySelector(`[name="condition_device_${conditionIndex}"]`).value = condition.deviceId;
                document.querySelector(`[name="condition_type_${conditionIndex}"]`).value = condition.type;
                document.querySelector(`[name="condition_value_${conditionIndex}"]`).value = condition.value;
                document.querySelector(`[name="condition_operator_${conditionIndex}"]`).value = condition.operator;
            });
            
            const actionsContainer = document.getElementById('rule-actions');
            rule.actions.forEach(action => {
                addAction();
                const actionIndex = actionsContainer.children.length - 1;
                
                document.querySelector(`[name="action_device_${actionIndex}"]`).value = action.deviceId;
                document.querySelector(`[name="action_value_${actionIndex}"]`).value = action.value;
            });
            
            // 滚动到规则表单
            document.getElementById('rules').scrollIntoView({ behavior: 'smooth' });
        }
        
        function deleteRule(ruleId) {
            if (!confirm('确定要删除这条规则吗？')) return;
            
            fetch('/api/rules', {
                method: 'DELETE',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: ruleId })
            })
            .then(r => r.json())
            .then(() => updateRules())
            .catch(err => console.error('deleteRule failed:', err));
        }
        
        // 规则表单提交事件
        document.getElementById('rule-form').addEventListener('submit', function(e) {
            e.preventDefault();
            
            const ruleId = document.getElementById('rule-id').value;
            const name = document.getElementById('rule-name').value;
            const enabled = document.getElementById('rule-enabled').checked;
            
            const conditions = [];
            const conditionItems = document.querySelectorAll('.condition-item');
            conditionItems.forEach((item, index) => {
                const deviceId = item.querySelector(`[name="condition_device_${index}"]`).value;
                const type = item.querySelector(`[name="condition_type_${index}"]`).value;
                const value = item.querySelector(`[name="condition_value_${index}"]`).value;
                const operator = item.querySelector(`[name="condition_operator_${index}"]`).value;
                
                conditions.push({ deviceId, type, value, operator });
            });
            
            const actions = [];
            const actionItems = document.querySelectorAll('.action-item');
            actionItems.forEach((item, index) => {
                const deviceId = item.querySelector(`[name="action_device_${index}"]`).value;
                const value = item.querySelector(`[name="action_value_${index}"]`).value;
                
                actions.push({ deviceId, value });
            });
            
            const ruleData = {
                id: ruleId,
                name,
                enabled,
                conditions,
                actions
            };
            
            const method = ruleId ? 'PUT' : 'POST';
            
            fetch('/api/rules', {
                method: method,
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(ruleData)
            })
            .then(r => r.json())
            .then(() => {
                clearRuleForm();
                updateRules();
            })
            .catch(err => console.error('saveRule failed:', err));
        });
        
        // 初始添加一个条件和一个动作
        document.addEventListener('DOMContentLoaded', function() {
            const ruleFormContainer = document.getElementById('rule-form-container');
            if (ruleFormContainer) {
                addCondition();
                addAction();
            }
        });
        
        // ===== JS 结束 =====
    </script>
</body>
</html>
)HTML";

#endif /* WEBPAGE_H */
