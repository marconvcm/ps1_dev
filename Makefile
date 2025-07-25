.PHONY: prepare build clean rename run run-fast build-run build-run-fast

# Default DuckStation path for macOS
DUCKSTATION ?= /Applications/DuckStation.app/Contents/MacOS/DuckStation

prepare:
	docker build --platform linux/amd64 . -t psn00bsdk

build:
	docker run --platform linux/amd64 --rm -v $(PWD)/src:/workspace/src -v $(PWD)/out:/workspace/build -w /workspace/src psn00bsdk sh -c "cmake --preset default . && cmake --build /workspace/build"

clean:
	rm -rf out/

# Run the game with normal boot sequence
run:
	@if [ ! -f "$(DUCKSTATION)" ]; then \
		echo "Error: DuckStation not found at $(DUCKSTATION)"; \
		echo "You can specify a custom path with DUCKSTATION=/path/to/duckstation"; \
		exit 1; \
	fi
	@PROJECT_NAME=$$(cat .project 2>/dev/null || echo "my_ps1_game"); \
	if [ ! -f "out/$${PROJECT_NAME}.bin" ]; then \
		echo "Error: Game binary not found. Run 'make build' first."; \
		exit 1; \
	fi; \
	echo "Running $${PROJECT_NAME} with DuckStation..."; \
	"$(DUCKSTATION)" -batch "out/$${PROJECT_NAME}.bin"

# Run the game with fast boot sequence
run-fast:
	@if [ ! -f "$(DUCKSTATION)" ]; then \
		echo "Error: DuckStation not found at $(DUCKSTATION)"; \
		echo "You can specify a custom path with DUCKSTATION=/path/to/duckstation"; \
		exit 1; \
	fi
	@PROJECT_NAME=$$(cat .project 2>/dev/null || echo "my_ps1_game"); \
	if [ ! -f "out/$${PROJECT_NAME}.bin" ]; then \
		echo "Error: Game binary not found. Run 'make build' first."; \
		exit 1; \
	fi; \
	echo "Running $${PROJECT_NAME} with DuckStation (fast boot)..."; \
	"$(DUCKSTATION)" -batch -fastboot "out/$${PROJECT_NAME}.bin"

# Build and run the game in one command
build-run: build run

# Build and run the game with fast boot in one command
build-run-fast: build run-fast

rename:
	@if [ -z "$(NEW_NAME)" ]; then \
		echo "Error: Please provide NEW_NAME parameter. Usage: make rename NEW_NAME=my_new_project"; \
		exit 1; \
	fi
	@if ! echo "$(NEW_NAME)" | grep -q '^[a-z][a-z0-9_]*$$'; then \
		echo "Error: NEW_NAME must be in snake_case format (lowercase letters, numbers, and underscores only, starting with a letter)"; \
		exit 1; \
	fi
	@OLD_NAME=$$(cat .project 2>/dev/null || echo "my_ps1_game"); \
	if [ "$$OLD_NAME" = "$(NEW_NAME)" ]; then \
		echo "Project name is already '$(NEW_NAME)'"; \
		exit 0; \
	fi; \
	echo "Renaming project from '$$OLD_NAME' to '$(NEW_NAME)'..."; \
	echo "$(NEW_NAME)" > .project; \
	sed -i.bak "s/$$OLD_NAME/$(NEW_NAME)/g" src/CMakeLists.txt && rm src/CMakeLists.txt.bak; \
	sed -i.bak "s/$$OLD_NAME/$(NEW_NAME)/g" src/iso.xml && rm src/iso.xml.bak; \
	if [ -f src/system.cnf ]; then \
		sed -i.bak "s/$${OLD_NAME}/$(NEW_NAME)/g" src/system.cnf && rm src/system.cnf.bak; \
	fi; \
	echo "Project successfully renamed to '$(NEW_NAME)'"