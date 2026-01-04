#!/usr/bin/env python3
import requests
import json

# 测试登录API
print("测试登录API...")
try:
    response = requests.post(
        "http://localhost:8080/api/admin/login",
        headers={"Content-Type": "application/json"},
        json={"username": "admin", "password": "admin123"}
    )
    print(f"状态码: {response.status_code}")
    print(f"响应内容: {response.text}")
except Exception as e:
    print(f"错误: {e}")

# 测试获取设备列表
print("\n测试获取设备列表...")
try:
    response = requests.get("http://localhost:8080/api/admin/devices")
    print(f"状态码: {response.status_code}")
    print(f"响应内容: {response.text}")
except Exception as e:
    print(f"错误: {e}")
