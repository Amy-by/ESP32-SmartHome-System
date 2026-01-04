#!/usr/bin/env python3
import urllib.request
import urllib.parse
import json

# 测试登录API
def test_login():
    print("测试登录API...")
    url = "http://localhost:8080/api/admin/login"
    data = json.dumps({"username": "admin", "password": "admin123"}).encode("utf-8")
    headers = {"Content-Type": "application/json"}
    
    try:
        req = urllib.request.Request(url, data=data, headers=headers, method="POST")
        with urllib.request.urlopen(req) as response:
            print(f"状态码: {response.status}")
            response_text = response.read().decode("utf-8")
            print(f"响应内容: {response_text}")
            return response_text
    except Exception as e:
        print(f"错误: {e}")
        import traceback
        traceback.print_exc()
        return None

# 测试获取设备列表
def test_get_devices():
    print("\n测试获取设备列表...")
    url = "http://localhost:8080/api/admin/devices"
    
    try:
        with urllib.request.urlopen(url) as response:
            print(f"状态码: {response.status}")
            response_text = response.read().decode("utf-8")
            print(f"响应内容: {response_text}")
            return response_text
    except Exception as e:
        print(f"错误: {e}")
        import traceback
        traceback.print_exc()
        return None

if __name__ == "__main__":
    test_login()
    test_get_devices()
