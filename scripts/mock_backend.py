import json
import random
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

class Handler(SimpleHTTPRequestHandler):
    def _serve_html_file(self, file_name: str):
        root = Path(self.directory)
        target = root / file_name
        if target.exists():
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.end_headers()
            self.wfile.write(target.read_bytes())
            return True
        return False

    def do_GET(self):
        if self.path in ("/", "/index", "/index.html", "/mock", "/mock.html"):
            # 优先展示模拟页面，其次为原始预览
            if not self._serve_html_file("mock.html"):
                self._serve_html_file("index.html")
            return
        if self.path == "/api/devices":
            devices = [
                {"id": "light_001", "name": "客厅灯", "type": "light", "status": True, "brightness": 75},
                {"id": "switch_001", "name": "风扇开关", "type": "switch", "status": False}
            ]
            doc = {"status": "success", "message": "获取设备状态成功", "devices": devices}
            self.send_response(200)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.end_headers()
            self.wfile.write(json.dumps(doc).encode("utf-8"))
            return
        if self.path == "/api/environment":
            env = {
                "temperature": 24.6,
                "humidity": 56.2,
                "lightIntensity": 320.0,
                "smokeDensity": 2.3
            }
            doc = {"status": "success", "message": "获取环境数据成功", "environment": env}
            self.send_response(200)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.end_headers()
            self.wfile.write(json.dumps(doc).encode("utf-8"))
            return
        if self.path == "/api/wifi":
            wifi = {"connected": True, "ip": "127.0.0.1", "ssid": "LOCAL_PREVIEW", "rssi": -45}
            doc = {"status": "success", "message": "获取WiFi状态成功", "wifi": wifi}
            self.send_response(200)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.end_headers()
            self.wfile.write(json.dumps(doc).encode("utf-8"))
            return
        return super().do_GET()

def main():
    root = Path(__file__).resolve().parents[1] / "out"
    Handler.directory = str(root)
    import os
    port = int(os.environ.get("PORT", "8000"))
    server = ThreadingHTTPServer(("0.0.0.0", port), Handler)
    server.serve_forever()

if __name__ == "__main__":
    main()
