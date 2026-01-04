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
    // 检查是否需要登录权限
    if (pageId === 'admin') {
        // 检查是否已登录
        const isLoggedIn = localStorage.getItem('isLoggedIn') === 'true';
        if (!isLoggedIn) {
            // 未登录，跳转到登录页面
            const pages = document.querySelectorAll('.page');
            pages.forEach(page => {
                page.style.display = 'none';
            });
            document.getElementById('login').style.display = 'block';
            return;
        }
    }
    
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
            alert('WiFi配置保存成功！');
        } else {
            alert('WiFi配置保存失败：' + (data.message || '未知错误'));
        }
    })
    .catch(err => {
        console.error('保存WiFi配置失败:', err);
        alert('WiFi配置保存失败，请检查网络连接！');
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

// 环境数据更新函数
function updateEnvironmentData() {
    fetch('/api/environment', { cache: 'no-store' })
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                const environmentData = document.getElementById('environment-data');
                if (!environmentData) return;
                
                environmentData.innerHTML = '';
                const env = data.environment;
                
                // 温度
                if (env.temperature !== null && env.temperature !== undefined) {
                    const tempEl = document.createElement('div');
                    tempEl.className = 'data-item';
                    tempEl.innerHTML = `
                        <div class="data-label">温度</div>
                        <div class="data-value" id="temperature">${env.temperature.toFixed(1)}°C</div>
                    `;
                    environmentData.appendChild(tempEl);
                }
                
                // 湿度
                if (env.humidity !== null && env.humidity !== undefined) {
                    const humiEl = document.createElement('div');
                    humiEl.className = 'data-item';
                    humiEl.innerHTML = `
                        <div class="data-label">湿度</div>
                        <div class="data-value" id="humidity">${env.humidity.toFixed(1)}%</div>
                    `;
                    environmentData.appendChild(humiEl);
                }
                
                // 光照强度
                if (env.light !== null && env.light !== undefined) {
                    const lightEl = document.createElement('div');
                    lightEl.className = 'data-item';
                    lightEl.innerHTML = `
                        <div class="data-label">光照</div>
                        <div class="data-value" id="light">${env.light.toFixed(0)} lux</div>
                    `;
                    environmentData.appendChild(lightEl);
                }
                
                // 烟雾浓度
                if (env.smoke !== null && env.smoke !== undefined) {
                    const smokeEl = document.createElement('div');
                    smokeEl.className = 'data-item';
                    smokeEl.innerHTML = `
                        <div class="data-label">烟雾</div>
                        <div class="data-value" id="smoke">${env.smoke.toFixed(1)} ppm</div>
                    `;
                    environmentData.appendChild(smokeEl);
                }
            }
        })
        .catch(err => console.error('获取环境数据失败:', err));
}

// 最后更新时间
function updateLastUpdateTime() {
    const timeEl = document.getElementById('last-update-time');
    if (timeEl) {
        timeEl.textContent = new Date().toLocaleString('zh-CN');
    }
}

// 告警设置更新
function updateAlarmSettings() {
    fetch('/api/alarm', { cache: 'no-store' })
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                const alarm = data.alarm;
                const minEl = document.getElementById('smoke-min');
                const maxEl = document.getElementById('smoke-max');
                const enabledEl = document.getElementById('smoke-enabled-select');
                const currentEl = document.getElementById('smoke-current');
                
                if (minEl) minEl.value = alarm.minThreshold || 100;
                if (maxEl) maxEl.value = alarm.maxThreshold || 1000;
                if (enabledEl) enabledEl.value = alarm.enabled ? '1' : '0';
                
                // 更新当前设置状态
                if (currentEl) {
                    const enabled = alarm.enabled ? '已启用' : '已禁用';
                    currentEl.textContent = `最小: ${alarm.minThreshold || 0}, 最大: ${alarm.maxThreshold || 500}, 状态: ${enabled}`;
                }
            }
        })
        .catch(err => console.error('获取告警设置失败:', err));
}

// 烟雾阈值保存
function saveSmokeThreshold() {
    const minInput = document.getElementById('smoke-min');
    const maxInput = document.getElementById('smoke-max');
    const enSelect = document.getElementById('smoke-enabled-select');
    const minThreshold = Number(minInput?.value);
    const maxThreshold = Number(maxInput?.value);
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
    .catch(err => console.error('保存烟雾阈值失败:', err));
}

// 登录功能
function login() {
    const username = document.getElementById('login-username').value;
    const password = document.getElementById('login-password').value;
    const errorDiv = document.getElementById('login-error');
    
    // 检查输入
    if (!username || !password) {
        errorDiv.textContent = '请输入用户名和密码';
        return;
    }
    
    // 发送登录请求
    fetch('/api/admin/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username, password })
    })
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            // 登录成功
            localStorage.setItem('isLoggedIn', 'true');
            localStorage.setItem('username', username);
            
            // 跳转到数据库管理页面
            showPage('admin');
            
            // 初始化数据库管理页面
            initAdminPage();
        } else {
            // 登录失败
            errorDiv.textContent = data.message || '登录失败，请检查用户名和密码';
        }
    })
    .catch(err => {
        console.error('登录请求失败:', err);
        errorDiv.textContent = '登录失败，请检查网络连接';
    });
}

// 退出功能
function logout() {
    // 清除登录状态
    localStorage.removeItem('isLoggedIn');
    localStorage.removeItem('username');
    
    // 跳转到登录页面
    showPage('devices');
}

// 显示添加设备表单
function showAddDeviceForm() {
    document.getElementById('add-device-form').style.display = 'block';
}

// 隐藏添加设备表单
function hideAddDeviceForm() {
    document.getElementById('add-device-form').style.display = 'none';
}

// 添加设备
function addDevice() {
    const name = document.getElementById('new-device-name').value;
    const type = document.getElementById('new-device-type').value;
    const pin = document.getElementById('new-device-pin').value;
    
    // 检查输入
    if (!name || !type || !pin) {
        alert('请填写所有字段');
        return;
    }
    
    // 发送添加设备请求
    fetch('/api/admin/devices', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ name, type, pin })
    })
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            // 添加成功
            alert('设备添加成功');
            hideAddDeviceForm();
            
            // 刷新设备列表
            refreshAdminDevices();
        } else {
            // 添加失败
            alert('设备添加失败: ' + (data.message || '未知错误'));
        }
    })
    .catch(err => {
        console.error('添加设备请求失败:', err);
        alert('设备添加失败，请检查网络连接');
    });
}

// 筛选传感器数据
function filterSensorData() {
    const sensorType = document.getElementById('sensor-type-filter').value;
    
    // 发送筛选请求
    fetch('/api/admin/sensor-data?type=' + (sensorType || ''), {
        method: 'GET',
        headers: { 'Content-Type': 'application/json' }
    })
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            // 更新传感器数据列表
            updateSensorDataList(data.sensorData);
        } else {
            alert('获取传感器数据失败: ' + (data.message || '未知错误'));
        }
    })
    .catch(err => {
        console.error('获取传感器数据请求失败:', err);
        alert('获取传感器数据失败，请检查网络连接');
    });
}

// 初始化数据库管理页面
function initAdminPage() {
    // 加载设备列表
    refreshAdminDevices();
    
    // 加载传感器数据
    filterSensorData();
}

// 刷新设备列表
function refreshAdminDevices() {
    fetch('/api/admin/devices', {
        method: 'GET',
        headers: { 'Content-Type': 'application/json' }
    })
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            updateDeviceListAdmin(data.devices);
        } else {
            alert('获取设备列表失败: ' + (data.message || '未知错误'));
        }
    })
    .catch(err => {
        console.error('获取设备列表请求失败:', err);
        alert('获取设备列表失败，请检查网络连接');
    });
}

// 更新设备列表
function updateDeviceListAdmin(devices) {
    const deviceList = document.getElementById('device-list-admin');
    deviceList.innerHTML = '';
    
    if (devices.length === 0) {
        deviceList.innerHTML = '<div class="empty-list">暂无设备</div>';
        return;
    }
    
    devices.forEach(device => {
        const deviceItem = document.createElement('div');
        deviceItem.className = 'device-item-admin';
        
        deviceItem.innerHTML = `
            <div class="device-info-admin">
                <div class="device-name-admin">${device.name || device.id}</div>
                <div class="device-type-admin">类型: ${device.type}</div>
                <div class="device-pin-admin">GPIO引脚: ${device.pin}</div>
                <div class="device-status-admin">状态: ${device.status ? '开启' : '关闭'}</div>
            </div>
            <div class="device-actions-admin">
                <button class="edit-button" onclick="editDevice('${device.id}')">编辑</button>
                <button class="delete-button" onclick="deleteDevice('${device.id}')">删除</button>
            </div>
        `;
        
        deviceList.appendChild(deviceItem);
    });
}

// 更新传感器数据列表
function updateSensorDataList(data) {
    const sensorDataList = document.getElementById('sensor-data-list');
    sensorDataList.innerHTML = '';
    
    if (data.length === 0) {
        sensorDataList.innerHTML = '<div class="empty-list">暂无传感器数据</div>';
        return;
    }
    
    // 创建表格
    const table = document.createElement('table');
    table.className = 'sensor-data-table';
    
    // 创建表头
    const thead = document.createElement('thead');
    thead.innerHTML = `
        <tr>
            <th>ID</th>
            <th>类型</th>
            <th>值</th>
            <th>单位</th>
            <th>时间</th>
        </tr>
    `;
    table.appendChild(thead);
    
    // 创建表体
    const tbody = document.createElement('tbody');
    data.forEach(item => {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${item.id}</td>
            <td>${item.type}</td>
            <td>${item.value}</td>
            <td>${item.unit || ''}</td>
            <td>${new Date(item.timestamp).toLocaleString('zh-CN')}</td>
        `;
        tbody.appendChild(row);
    });
    table.appendChild(tbody);
    
    sensorDataList.appendChild(table);
}

// 编辑设备
function editDevice(deviceId) {
    // 这里可以实现编辑设备的功能
    alert('编辑设备功能将在后续实现');
}

// 删除设备
function deleteDevice(deviceId) {
    if (confirm('确定要删除此设备吗？')) {
        fetch(`/api/admin/devices/${deviceId}`, {
            method: 'DELETE',
            headers: { 'Content-Type': 'application/json' }
        })
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                alert('设备删除成功');
                refreshAdminDevices();
            } else {
                alert('设备删除失败: ' + (data.message || '未知错误'));
            }
        })
        .catch(err => {
            console.error('删除设备请求失败:', err);
            alert('设备删除失败，请检查网络连接');
        });
    }
}
