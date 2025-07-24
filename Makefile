.PHONY: build clean

prepare:
	docker build --platform linux/amd64 . -t psn00bsdk

build:
	docker run --platform linux/amd64 --rm -v $(PWD)/src:/workspace/src -v $(PWD)/out:/workspace/build -w /workspace/src psn00bsdk sh -c "cmake --preset default . && cmake --build /workspace/build"

clean:
	rm -rf out/