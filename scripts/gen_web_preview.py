import re
import sys
from pathlib import Path

def extract_block(content, tag):
    pattern = rf'R"{tag}\((.*?)\){tag}"'
    m = re.search(pattern, content, re.DOTALL)
    return m.group(1) if m else ""

def main():
    if len(sys.argv) < 2:
        print("Usage: gen_web_preview.py <output_html>")
        sys.exit(1)
    out_path = Path(sys.argv[1])

    root = Path(__file__).resolve().parents[1]
    wp_h = (root / "src/web/webpage.h").read_text(encoding="utf-8")
    styles_h = (root / "src/web/styles.h").read_text(encoding="utf-8")
    scripts_h = (root / "src/web/scripts.h").read_text(encoding="utf-8")

    html = extract_block(wp_h, "HTML")
    styles = extract_block(styles_h, "CSS")
    scripts = extract_block(scripts_h, "SCRIPT")

    html = html.replace("%STYLES%", styles).replace("%SCRIPTS%", scripts)

    out_path.write_text(html, encoding="utf-8")

if __name__ == "__main__":
    main()
