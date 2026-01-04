#!/usr/bin/env python3
"""
模拟后端服务器，用于测试管理员登录和数据库CRUD功能
"""

from http.server import HTTPServer, BaseHTTPRequestHandler
import json
import re

class MockRequestHandler(BaseHTTPRequestHandler):
    # 默认管理员凭证
    ADMIN_USERNAME = "admin"
    ADMIN_PASSWORD = "admin123"
    
    # 模拟设备数据
    devices = [
        {"id": 1, "name": "客厅灯", "type": "light", "pin": 13, "status": True, "brightness": 50},
        {"id": 2, "name": "卧室灯", "type": "light", "pin": 12, "status": False, "brightness": 0},
        {"id": 3, "name": "温湿度传感器", "type": "sensor", "pin": 14, "status": True, "brightness": 0}
    ]
    
    # 模拟传感器数据
    sensor_data = [
        {"id": 1, "type": "temperature", "value": 25.5, "unit": "°C", "timestamp": "2024-01-15 14:30:00"},
        {"id": 2, "type": "humidity", "value": 60.2, "unit": "%", "timestamp": "2024-01-15 14:30:00"},
        {"id": 3, "type": "temperature", "value": 26.1, "unit": "°C", "timestamp": "2024-01-15 14:35:00"},
        {"id": 4, "type": "humidity", "value": 58.5, "unit": "%", "timestamp": "2024-01-15 14:35:00"},
        {"id": 5, "type": "temperature", "value": 25.8, "unit": "°C", "timestamp": "2024-01-15 14:40:00"},
        {"id": 6, "type": "humidity", "value": 59.3, "unit": "%", "timestamp": "2024-01-15 14:40:00"}
    ]
    
    def _set_headers(self, content_type="text/html", status_code=200):
        self.send_response(status_code)
        self.send_header("Content-type", content_type)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()
    
    def do_OPTIONS(self):
        self._set_headers()
    
    def do_POST(self):
        if self.path == "/api/admin/login":
            self._handle_login()
        elif self.path == "/api/admin/devices":
            self._handle_create_device()
        else:
            self._set_headers(status_code=404)
            self.wfile.write(json.dumps({"status": "error", "message": "Not Found"}).encode("utf-8"))
    
    def do_DELETE(self):
        if self.path.startswith("/api/admin/devices/"):
            device_id = int(self.path.split("/")[-1])
            self._handle_delete_device(device_id)
        else:
            self._set_headers(status_code=404)
            self.wfile.write(json.dumps({"status": "error", "message": "Not Found"}).encode("utf-8"))
    
    def do_PUT(self):
        if self.path.startswith("/api/admin/devices/"):
            device_id = int(self.path.split("/")[-1])
            self._handle_edit_device(device_id)
        else:
            self._set_headers(status_code=404)
            self.wfile.write(json.dumps({"status": "error", "message": "Not Found"}).encode("utf-8"))
    
    def do_GET(self):
        # 去掉查询参数
        path = self.path.split('?')[0]
        if path == "/api/admin/sensor-data":
            self._handle_get_sensor_data()
        elif path == "/api/admin/devices":
            self._handle_get_devices()
        else:
            self._set_headers(status_code=404)
            self.wfile.write(json.dumps({"status": "error", "message": "Not Found"}).encode("utf-8"))
    
    def _handle_login(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length).decode('utf-8')
        data = json.loads(post_data)
        
        username = data.get('username')
        password = data.get('password')
        
        if username == self.ADMIN_USERNAME and password == self.ADMIN_PASSWORD:
            response = {"status": "success", "message": "登录成功"}
        else:
            response = {"status": "error", "message": "用户名或密码错误"}
        
        self._set_headers("application/json")
        self.wfile.write(json.dumps(response).encode("utf-8"))
    
    def _handle_create_device(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length).decode('utf-8')
        data = json.loads(post_data)
        
        # 创建新设备
        new_device = {
            "id": len(self.devices) + 1,
            "name": data.get("name"),
            "type": data.get("type"),
            "pin": data.get("pin"),
            "status": data.get("status", False),
            "brightness": data.get("brightness", 0)
        }
        
        self.devices.append(new_device)
        
        response = {"status": "success", "message": "设备创建成功", "device": new_device}
        self._set_headers("application/json")
        self.wfile.write(json.dumps(response).encode("utf-8"))
    
    def _handle_delete_device(self, device_id):
        # 检查设备是否存在
        device_exists = any(device["id"] == device_id for device in self.devices)
        if not device_exists:
            response = {"status": "error", "message": "设备不存在"}
            self._set_headers("application/json", 404)
            self.wfile.write(json.dumps(response).encode("utf-8"))
            return
        
        # 删除设备
        self.devices = [device for device in self.devices if device["id"] != device_id]
        
        response = {"status": "success", "message": "设备删除成功"}
        self._set_headers("application/json")
        self.wfile.write(json.dumps(response).encode("utf-8"))
    
    def _handle_edit_device(self, device_id):
        content_length = int(self.headers['Content-Length'])
        put_data = self.rfile.read(content_length).decode('utf-8')
        data = json.loads(put_data)
        
        # 更新设备
        for device in self.devices:
            if device["id"] == device_id:
                device["name"] = data.get("name", device["name"])
                device["type"] = data.get("type", device["type"])
                device["pin"] = data.get("pin", device["pin"])
                device["status"] = data.get("status", device["status"])
                device["brightness"] = data.get("brightness", device["brightness"])
                
                response = {"status": "success", "message": "设备更新成功", "device": device}
                self._set_headers("application/json")
                self.wfile.write(json.dumps(response).encode("utf-8"))
                return
        
        response = {"status": "error", "message": "设备不存在"}
        self._set_headers("application/json", 404)
        self.wfile.write(json.dumps(response).encode("utf-8"))
    
    def _handle_get_devices(self):
        response = {"status": "success", "devices": self.devices}
        self._set_headers("application/json")
        self.wfile.write(json.dumps(response).encode("utf-8"))
    
    def _handle_get_sensor_data(self):
        # 获取查询参数
        query = self.path.split('?')[1] if '?' in self.path else ''
        params = {}
        if query:
            for param in query.split('&'):
                key, value = param.split('=')
                params[key] = value
        
        # 过滤数据
        filtered_data = self.sensor_data
        if 'type' in params and params['type']:
            filtered_data = [data for data in filtered_data if data["type"] == params["type"]]
        if 'startDate' in params:
            filtered_data = [data for data in filtered_data if data["timestamp"] >= params["startDate"]]
        if 'endDate' in params:
            filtered_data = [data for data in filtered_data if data["timestamp"] <= params["endDate"]]
        
        response = {"status": "success", "sensorData": filtered_data}
        self._set_headers("application/json")
        self.wfile.write(json.dumps(response).encode("utf-8"))

def run_server(port=8080):
    server_address = ('', port)
    httpd = HTTPServer(server_address, MockRequestHandler)
    print(f"模拟服务器运行在 http://localhost:{port}")
    print(f"使用以下凭证登录：")
    print(f"用户名: {MockRequestHandler.ADMIN_USERNAME}")
    print(f"密码: {MockRequestHandler.ADMIN_PASSWORD}")
    print("\nAPI端点：")
    print("POST /api/admin/login - 管理员登录")
    print("GET /api/admin/devices - 获取设备列表")
    print("POST /api/admin/devices - 创建设备")
    print("PUT /api/admin/devices/{id} - 编辑设备")
    print("DELETE /api/admin/devices/{id} - 删除设备")
    print("GET /api/admin/sensor-data - 获取传感器数据")
    httpd.serve_forever()

if __name__ == "__main__":
    run_server(8080)
