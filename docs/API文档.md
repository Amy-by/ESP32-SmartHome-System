# ESP32智能家居控制系统API接口文档

## 1. 概述

本文档详细描述了ESP32智能家居控制系统提供的RESTful API接口，用于设备控制、环境数据获取和系统状态查询。

### 1.1 API基础信息

- **API前缀**：`/api/`
- **响应格式**：JSON
- **支持的HTTP方法**：GET, POST
- **服务器地址**：ESP32的IP地址，端口80
- **示例请求URL**：`http://192.168.1.100/api/devices`

### 1.2 响应状态码

| 状态码 | 描述 |
|--------|------|
| 200    | 请求成功 |
| 400    | 请求参数错误 |
| 404    | 资源未找到 |
| 500    | 服务器内部错误 |

### 1.3 响应格式示例

```json
{
    "status": "success",
    "message": "操作成功",
    "data": {}
}
```

## 2. 设备控制API

### 2.1 获取所有设备状态

**接口路径**：`/api/devices`
**请求方法**：GET
**功能**：获取系统中所有设备的当前状态

**响应示例**：

```json
{
    "status": "success",
    "message": "获取设备状态成功",
    "devices": [
        {
            "id": "light_1",
            "name": "客厅灯",
            "type": "SmartLight",
            "status": true,
            "brightness": 75
        },
        {
            "id": "switch_1",
            "name": "插座",
            "type": "SmartSwitch",
            "status": false
        }
    ]
}
```

### 2.2 获取单个设备状态

**接口路径**：`/api/devices/{device_id}`
**请求方法**：GET
**功能**：获取指定设备的当前状态

**路径参数**：
- `device_id`：设备唯一标识符

**响应示例**：

```json
{
    "status": "success",
    "message": "获取设备状态成功",
    "device": {
        "id": "light_1",
        "name": "客厅灯",
        "type": "SmartLight",
        "status": true,
        "brightness": 75
    }
}
```

### 2.3 控制设备

**接口路径**：`/api/devices/control`
**请求方法**：POST
**功能**：控制指定设备的状态

**请求体**：

```json
{
    "id": "light_1",
    "status": true,  // 可选，设备开关状态
    "brightness": 50  // 可选，仅智能灯支持（0-100）
}
```

**参数说明**：
- `id`：设备唯一标识符（必填）
- `status`：设备开关状态（true为开启，false为关闭，对于SmartSwitch为必填；对于SmartLight，当不提供brightness时必填）
- `brightness`：亮度值（0-100，仅SmartLight类型设备支持，可选，提供此参数时会忽略status）

**响应示例**：

```json
{
    "status": "success",
    "message": "设备控制成功"
}
```

## 3. 环境监测API

### 3.1 获取环境数据

**接口路径**：`/api/environment`
**请求方法**：GET
**功能**：获取所有环境传感器的当前数据

**响应示例**：

```json
{
    "status": "success",
    "message": "获取环境数据成功",
    "environment": {
        "temperature": 25.5,
        "humidity": 45.2,
        "lightIntensity": 800,
        "smokeDensity": 0.1
    }
}
```

## 4. WiFi状态API

### 4.1 获取WiFi状态

**接口路径**：`/api/wifi`
**请求方法**：GET
**功能**：获取WiFi连接状态、IP地址、SSID和信号强度

**响应示例**：

```json
{
    "status": "success",
    "message": "获取WiFi状态成功",
    "wifi": {
        "connected": true,
        "ssid": "MyWiFi",
        "ip": "192.168.1.100",
        "rssi": -45
    }
}

**响应示例**：

```json
{
    "status": "success",
    "message": "获取WiFi状态成功",
    "wifi": {
        "connected": true,
        "ssid": "MyWiFi",
        "ip": "192.168.1.100",
        "rssi": -55
    }
}
```

## 5. WebSocket API

### 5.1 WebSocket连接

**连接地址**：`ws://{ip_address}/ws`
**功能**：实时推送设备状态和环境数据更新

### 5.2 支持的消息类型

#### 5.2.1 客户端发送消息

**心跳请求**：
```json
{
    "type": "ping"
}
```

#### 5.2.2 服务器推送消息

**心跳响应**：
```json
{
    "type": "pong"
}
```

**设备状态更新**：
```json
{
    "type": "deviceUpdate"
}
```

**环境数据更新**：
```json
{
    "type": "environmentUpdate"
}
```

## 6. API使用示例

### 6.1 使用JavaScript获取设备列表

```javascript
fetch('http://192.168.1.100/api/devices')
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            console.log('设备列表:', data.devices);
        }
    })
    .catch(error => {
        console.error('获取设备列表失败:', error);
    });
```

### 6.2 使用JavaScript获取单个设备状态

```javascript
fetch('http://192.168.1.100/api/devices/light_1')
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            console.log('设备状态:', data.device);
        }
    })
    .catch(error => {
        console.error('获取设备状态失败:', error);
    });
```

### 6.3 使用JavaScript控制设备

```javascript
fetch('http://192.168.1.100/api/devices/control', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json'
    },
    body: JSON.stringify({
        id: 'light_1',
        status: true,
        brightness: 75
    })
})
.then(response => response.json())
.then(data => {
    if (data.status === 'success') {
        console.log('设备控制成功');
    }
})
.catch(error => {
    console.error('设备控制失败:', error);
});
```

### 6.4 使用JavaScript获取环境数据

```javascript
fetch('http://192.168.1.100/api/environment')
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            console.log('环境数据:', data.environment);
        }
    })
    .catch(error => {
        console.error('获取环境数据失败:', error);
    });
```

### 6.5 使用JavaScript获取WiFi状态

```javascript
fetch('http://192.168.1.100/api/wifi')
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            console.log('WiFi状态:', data.wifi);
        }
    })
    .catch(error => {
        console.error('获取WiFi状态失败:', error);
    });
```

### 6.6 使用JavaScript建立WebSocket连接

```javascript
// 建立WebSocket连接
const ws = new WebSocket('ws://192.168.1.100/ws');

// 连接建立
ws.onopen = () => {
    console.log('WebSocket连接已建立');
    // 发送心跳
    setInterval(() => {
        ws.send(JSON.stringify({ type: 'ping' }));
    }, 30000);
};

// 接收消息
ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    switch (data.type) {
        case 'pong':
            console.log('收到心跳响应');
            break;
        case 'deviceUpdate':
            console.log('设备状态更新，重新获取设备列表');
            // 重新获取设备列表
            fetchDevices();
            break;
        case 'environmentUpdate':
            console.log('环境数据更新，重新获取环境数据');
            // 重新获取环境数据
            fetchEnvironmentData();
            break;
    }
};

// 连接关闭
ws.onclose = () => {
    console.log('WebSocket连接已关闭');
    // 尝试重新连接
    setTimeout(() => {
        console.log('尝试重新连接WebSocket');
        // 重新建立连接
    }, 5000);
};

// 连接错误
ws.onerror = (error) => {
    console.error('WebSocket连接错误:', error);
};
```

## 7. 注意事项

1. **设备ID唯一性**：每个设备ID必须唯一，由系统在设备添加时自动生成。
2. **权限控制**：当前版本API不包含权限控制，所有客户端均可访问所有接口。
3. **请求频率**：建议客户端控制请求频率，避免对ESP32造成过大负担，推荐间隔不小于1秒。
4. **错误处理**：客户端应妥善处理API返回的错误状态码，并在用户界面给予适当提示。
5. **WebSocket连接**：如果WebSocket连接断开，客户端应实现自动重连机制。
6. **设备类型**：目前支持两种设备类型：
   - `SmartLight`：智能灯，支持开关和亮度调节
   - `SmartSwitch`：智能开关，仅支持开关控制
7. **亮度范围**：智能灯的亮度值范围为0-100，0表示关闭，100表示最亮。

## 8. API版本控制

当前API版本为V1，未来如有重大变更，将通过URL路径区分不同版本，例如：`/api/v2/devices`。

---

**文档版本**：1.0  
**更新日期**：2024年1月20日  
**作者**：ESP32智能家居控制系统开发团队