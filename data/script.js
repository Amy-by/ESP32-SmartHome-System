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
    
    // 配置页面事件监听
    const dhcpEnabled = document.getElementById('dhcp-enabled');
    if (dhcpEnabled) {
        dhcpEnabled.addEventListener('change', () => {
            const staticIpConfig = document.getElementById('static-ip-config');
            if (staticIpConfig) {
                staticIpConfig.style.display = dhcpEnabled.checked ? 'none' : 'block';
            }
        });
    }
    
    // 加载当前配置
    loadConfig();
});

// 页面切换功能
function showPage(pageId) {
    // 隐藏所有页面
    const pages = document.querySelectorAll('.page');
    pages.forEach(page => {
        page.style.display = 'none';
    });
    
    // 显示选中的页面
    const selectedPage = document.getElementById(pageId);
    if (selectedPage) {
        selectedPage.style.display = 'block';
    }
    
    // 更新导航按钮状态
    const navButtons = document.querySelectorAll('.nav-button');
    navButtons.forEach(button => {
        button.classList.remove('active');
    });
    
    const activeButton = document.querySelector(`[onclick="showPage('${pageId}')"]`);
    if (activeButton) {
        activeButton.classList.add('active');
    }
}

// 加载配置功能
function loadConfig() {
    fetch('/api/config', {
        method: 'GET',
        headers: { 'Content-Type': 'application/json' }
    })
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            const config = data.config;
            
            // 加载WiFi配置
            document.getElementById('wifi-ssid').value = config.wifi.ssid || '';
            document.getElementById('wifi-password').value = config.wifi.password || '';
            document.getElementById('dhcp-enabled').checked = config.wifi.dhcp_enabled || false;
            
            // 加载静态IP配置
            const staticIpConfig = document.getElementById('static-ip-config');
            const dhcpEnabled = document.getElementById('dhcp-enabled');
            if (staticIpConfig && dhcpEnabled) {
                staticIpConfig.style.display = dhcpEnabled.checked ? 'none' : 'block';
            }
            
            document.getElementById('static-ip').value = config.wifi.static_ip.ip || '';
            document.getElementById('gateway').value = config.wifi.static_ip.gateway || '';
            document.getElementById('subnet-mask').value = config.wifi.static_ip.subnet_mask || '';
            document.getElementById('dns-server').value = config.wifi.static_ip.dns_server || '';
            
            // 加载传感器配置
            document.getElementById('sensor-update-interval').value = config.sensors.update_interval || 1000;
            document.getElementById('temperature-threshold').value = config.sensors.temperature_threshold || 28.0;
            document.getElementById('humidity-threshold').value = config.sensors.humidity_threshold || 70.0;
            document.getElementById('light-threshold').value = config.sensors.light_threshold || 500;
            document.getElementById('smoke-threshold').value = config.sensors.smoke_threshold || 500;
            
            // 加载设备配置
            document.getElementById('device-polling-interval').value = config.devices.polling_interval || 500;
            
            // 加载存储配置
            document.getElementById('max-log-size').value = config.storage.max_log_size || 102400;
            
            // 加载Web服务器配置
            document.getElementById('web-server-port').value = config.web.port || 80;
            
            // 加载告警配置
            document.getElementById('alarm-duration').value = config.alarm.duration || 30;
            document.getElementById('alarm-enabled').checked = config.alarm.enabled || false;
            
            // 加载调试配置
            document.getElementById('debug-mode').checked = config.debug.mode || false;
        }
    })
    .catch(err => {
        console.error('加载配置失败:', err);
    });
}

// 保存配置功能
function saveConfig() {
    const config = {
        wifi: {
            ssid: document.getElementById('wifi-ssid').value,
            password: document.getElementById('wifi-password').value,
            dhcp_enabled: document.getElementById('dhcp-enabled').checked,
            static_ip: {
                ip: document.getElementById('static-ip').value,
                gateway: document.getElementById('gateway').value,
                subnet_mask: document.getElementById('subnet-mask').value,
                dns_server: document.getElementById('dns-server').value
            }
        },
        sensors: {
            update_interval: parseInt(document.getElementById('sensor-update-interval').value),
            temperature_threshold: parseFloat(document.getElementById('temperature-threshold').value),
            humidity_threshold: parseFloat(document.getElementById('humidity-threshold').value),
            light_threshold: parseInt(document.getElementById('light-threshold').value),
            smoke_threshold: parseInt(document.getElementById('smoke-threshold').value)
        },
        devices: {
            polling_interval: parseInt(document.getElementById('device-polling-interval').value)
        },
        storage: {
            max_log_size: parseInt(document.getElementById('max-log-size').value)
        },
        web: {
            port: parseInt(document.getElementById('web-server-port').value)
        },
        alarm: {
            duration: parseInt(document.getElementById('alarm-duration').value),
            enabled: document.getElementById('alarm-enabled').checked
        },
        debug: {
            mode: document.getElementById('debug-mode').checked
        }
    };
    
    fetch('/api/config', {
        method: 'PUT',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(config)
    })
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            alert('配置保存成功！');
        } else {
            alert('配置保存失败：' + (data.message || '未知错误'));
        }
    })
    .catch(err => {
        console.error('保存配置失败:', err);
        alert('配置保存失败，请检查网络连接！');
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
        const checkbox = document.getElementById('toggle-' + deviceId) || li.querySelector('input[type="checkbox"]');
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
        label.textContent = b + '%';
        
        controls.appendChild(range);
        controls.appendChild(label);
        
        range.addEventListener('input', (e) => {
            const val = Number(e.target.value);
            label.textContent = val + '%';
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
            label.textContent = val + '%';
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
            label.textContent = b + '%';
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