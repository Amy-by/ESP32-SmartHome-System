PROJECT_DIR := $(CURDIR)
OUT_DIR := $(PROJECT_DIR)/out
PREVIEW_HTML := $(OUT_DIR)/index.html
MOCK_HTML := $(OUT_DIR)/mock.html

.PHONY: all build upload monitor upload-monitor clean frontend-preview frontend-mock test lint update-libs

# 默认目标
all: build

# PlatformIO 构建目标
build:
	@platformio run

# 上传固件到设备
upload:
	@platformio run --target upload

# 启动串口监控
monitor:
	@platformio device monitor

# 上传并监控
upload-monitor:
	@platformio run --target upload --target monitor

# 前端预览目标
frontend-preview:
	@mkdir -p $(OUT_DIR)
	@python3 scripts/gen_web_preview.py $(PREVIEW_HTML)
	@echo "生成前端预览: $(PREVIEW_HTML)"

frontend-mock:
	@mkdir -p $(OUT_DIR)
	@python3 scripts/gen_mock_preview.py $(MOCK_HTML)
	@echo "生成模拟前端: $(MOCK_HTML)"

# 测试目标
test:
	@platformio test

# 代码检查目标
lint:
	@platformio check

# 更新依赖库
update-libs:
	@platformio lib update

# 清理目标
clean:
	@platformio run --target clean
	@rm -rf $(OUT_DIR)
	@rm -rf .pio

