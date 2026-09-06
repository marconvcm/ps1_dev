.PHONY: prepare build clean rename run run-fast build-run emulate up dist zip png2tim compile

# Cross-platform DuckStation emulator detection (macOS, Linux native, or Flatpak)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
DUCKSTATION ?= /Applications/DuckStation.app/Contents/MacOS/DuckStation
else
DUCKSTATION ?= $(shell which duckstation 2>/dev/null || which duckstation-qt 2>/dev/null || echo "flatpak run org.duckstation.DuckStation")
endif

# User mapping so Docker outputs files with host permissions
DOCKER_USER := --user $(shell id -u):$(shell id -g)

prepare:
	docker build --platform linux/amd64 . -t psn00bsdk

build: png2tim compile

compile:
	@mkdir -p out
	@docker run --platform linux/amd64 $(DOCKER_USER) --rm -v $(PWD)/src:/workspace/src -v $(PWD)/out:/workspace/build -w /workspace/src psn00bsdk sh -c "cmake --preset default . && cmake --build /workspace/build"

clean:
	@rm -rf out/ dist/ 2>/dev/null || docker run --rm -v $(PWD):/workspace psn00bsdk rm -rf /workspace/out /workspace/dist

# Run the game with normal boot sequence
run:
	@if ! command -v $(firstword $(DUCKSTATION)) >/dev/null 2>&1 && [ ! -f "$(DUCKSTATION)" ]; then \
		echo "Error: DuckStation not found at '$(DUCKSTATION)'"; \
		echo "You can specify a custom path with DUCKSTATION=/path/to/duckstation"; \
		exit 1; \
	fi
	@PROJECT_NAME=$$(cat .project 2>/dev/null || echo "hello_pong"); \
	if [ ! -f "out/$${PROJECT_NAME}.bin" ]; then \
		echo "Error: Game binary not found. Run 'make build' first."; \
		exit 1; \
	fi; \
	echo "Running $${PROJECT_NAME} with DuckStation..."; \
	$(DUCKSTATION) -batch "out/$${PROJECT_NAME}.bin"

# Run the game with fast boot sequence
run-fast:
	@if ! command -v $(firstword $(DUCKSTATION)) >/dev/null 2>&1 && [ ! -f "$(DUCKSTATION)" ]; then \
		echo "Error: DuckStation not found at '$(DUCKSTATION)'"; \
		echo "You can specify a custom path with DUCKSTATION=/path/to/duckstation"; \
		exit 1; \
	fi
	@PROJECT_NAME=$$(cat .project 2>/dev/null || echo "hello_pong"); \
	if [ ! -f "out/$${PROJECT_NAME}.bin" ]; then \
		echo "Error: Game binary not found. Run 'make build' first."; \
		exit 1; \
	fi; \
	echo "Running $${PROJECT_NAME} with DuckStation (fast boot)..."; \
	$(DUCKSTATION) -batch -fastboot "out/$${PROJECT_NAME}.bin"

# Build and run the game in one command
build-run: build run

# Build and run the game with fast boot in one command
emulate: build run-fast

# Build image and run the game with fast boot
up: prepare emulate

dist: clean build zip

png2tim:
	@mkdir -p out
	@docker run --platform linux/amd64 $(DOCKER_USER) --rm -v $(PWD)/src:/workspace/src -v $(PWD)/out:/workspace/build -w /workspace/src psn00bsdk sh -c "cd /workspace/src/assets && /opt/png2tim/png2tim.sh"

rename:
	@if [ -z "$(NEW_NAME)" ]; then \
		echo "Error: Please provide NEW_NAME parameter. Usage: make rename NEW_NAME=my_new_project"; \
		exit 1; \
	fi
	@if ! echo "$(NEW_NAME)" | grep -q '^[a-z][a-z0-9_]*$$'; then \
		echo "Error: NEW_NAME must be in snake_case format (lowercase letters, numbers, and underscores only, starting with a letter)"; \
		exit 1; \
	fi
	@OLD_NAME=$$(cat .project 2>/dev/null || echo "hello_pong"); \
	if [ "$$OLD_NAME" = "$(NEW_NAME)" ]; then \
		echo "Project name is already '$(NEW_NAME)'"; \
		exit 0; \
	fi; \
	echo "Renaming project from '$$OLD_NAME' to '$(NEW_NAME)'..."; \
	echo "$(NEW_NAME)" > .project; \
	sed "s/$$OLD_NAME/$(NEW_NAME)/g" src/CMakeLists.txt > src/CMakeLists.txt.tmp && mv src/CMakeLists.txt.tmp src/CMakeLists.txt; \
	echo "Project successfully renamed to '$(NEW_NAME)'"

zip:
	@PROJECT_NAME=$$(cat .project 2>/dev/null || echo "hello_pong"); \
	echo "Creating distribution package..."; \
	mkdir -p dist; \
	zip -Xj "dist/$${PROJECT_NAME}.zip" "out/$${PROJECT_NAME}.bin" "out/$${PROJECT_NAME}.cue"; \
	echo "Distribution package created successfully."