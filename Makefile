.PHONY: build clean rename

prepare:
	docker build --platform linux/amd64 . -t psn00bsdk

build:
	docker run --platform linux/amd64 --rm -v $(PWD)/src:/workspace/src -v $(PWD)/out:/workspace/build -w /workspace/src psn00bsdk sh -c "cmake --preset default . && cmake --build /workspace/build"

clean:
	rm -rf out/

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