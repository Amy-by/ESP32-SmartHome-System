import re
import sys
from pathlib import Path

def extract_block(content, tag):
    m = re.search(rf'R"{tag}\((.*?)\){tag}"', content, re.DOTALL)
    return m.group(1) if m else ""

def build_mock_script():
    return """
const originalFetch = window.fetch;
// 内存态模拟设备
window.__mockState = {
  devices: [
    {id:'light_001', name:'客厅灯', type:'light', status:true, brightness:75},
    {id:'switch_001', name:'风扇开关', type:'switch', status:false}
  ],
  environment: {temperature:24.6, humidity:56.2, lightIntensity:320.0, smokeDensity:2.3},
  wifi: {connected:true, ip:'127.0.0.1', ssid:'LOCAL_PREVIEW', rssi:-45}
};
window.fetch = async function(url, options) {
  if (typeof url === 'string' && url.startsWith('/api/')) {
    if (url === '/api/devices') {
      const { devices } = window.__mockState;
      return new Response(JSON.stringify({status:'success', message:'获取设备状态成功', devices}), {headers:{'Content-Type':'application/json'}});
    }
    if (url === '/api/environment') {
      const { environment } = window.__mockState;
      return new Response(JSON.stringify({status:'success', message:'获取环境数据成功', environment}), {headers:{'Content-Type':'application/json'}});
    }
    if (url === '/api/wifi') {
      const { wifi } = window.__mockState;
      return new Response(JSON.stringify({status:'success', message:'获取WiFi状态成功', wifi}), {headers:{'Content-Type':'application/json'}});
    }
    if (url === '/api/devices/control') {
      // 处理设备控制，更新内存态
      try {
        const body = options && options.body ? JSON.parse(options.body) : {};
        const dev = window.__mockState.devices.find(d => d.id === body.id);
        if (dev) {
          if (typeof body.status === 'boolean') dev.status = body.status;
          if (typeof body.brightness === 'number' && dev.type === 'light') dev.brightness = Math.max(0, Math.min(100, body.brightness|0));
        }
        if (window.__mockWS && typeof window.__mockWS.onmessage === 'function') {
          window.__mockWS.onmessage({ data: JSON.stringify({type:'deviceUpdate'}) });
        }
        return new Response(JSON.stringify({status:'success'}), {headers:{'Content-Type':'application/json'}});
      } catch (e) {
        return new Response(JSON.stringify({status:'error', message:String(e)}), {status:400, headers:{'Content-Type':'application/json'}});
      }
    }
  }
  return originalFetch(url, options);
};
class MockWebSocket {
  constructor(url) { this.url = url; setTimeout(()=>{this.onopen && this.onopen();}, 0);
    this._timer = setInterval(()=>{ this.onmessage && this.onmessage({ data: JSON.stringify({type:'environmentUpdate'}) }); }, 5000);
    window.__mockWS = this;
  }
  send(_) {}
  close() { clearInterval(this._timer); this.onclose && this.onclose(); }
}
window.WebSocket = MockWebSocket;
"""

def main():
    if len(sys.argv) < 2:
        print("Usage: gen_mock_preview.py <output_html>")
        sys.exit(1)
    out_path = Path(sys.argv[1])
    root = Path(__file__).resolve().parents[1]
    wp_h = (root / "src/web/webpage.h").read_text(encoding="utf-8")
    styles_h = (root / "src/web/styles.h").read_text(encoding="utf-8")
    scripts_h = (root / "src/web/scripts.h").read_text(encoding="utf-8")
    html = extract_block(wp_h, "HTML")
    styles = extract_block(styles_h, "CSS")
    
    # Inject mock script before the first <script> tag
    mock_script = "<script>" + build_mock_script() + "</script>"
    if "<script>" in html:
        html = html.replace("<script>", mock_script + "\n<script>", 1)
    else:
        html = html.replace("</body>", mock_script + "\n</body>")
        
    html = html.replace("%STYLES%", styles)
    out_path.write_text(html, encoding="utf-8")

if __name__ == "__main__":
    main()
