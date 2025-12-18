PROJECT_DIR := $(CURDIR)
OUT_DIR := $(PROJECT_DIR)/out
PREVIEW_HTML := $(OUT_DIR)/index.html
MOCK_HTML := $(OUT_DIR)/mock.html

.PHONY: frontend-preview clean

frontend-preview:
	@mkdir -p $(OUT_DIR)
	@python3 scripts/gen_web_preview.py $(PREVIEW_HTML)
	@echo "生成前端预览: $(PREVIEW_HTML)"

frontend-mock:
	@mkdir -p $(OUT_DIR)
	@python3 scripts/gen_mock_preview.py $(MOCK_HTML)
	@echo "生成模拟前端: $(MOCK_HTML)"

clean:
	@rm -rf $(OUT_DIR)
