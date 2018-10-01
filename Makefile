.PHONY: all clean

CMAKE_GENERATOR?=Unix Makefiles
CMAKE_BUILD_TYPE?=Release

all:
	@mkdir -p build
	cd build && cmake -G "$(CMAKE_GENERATOR)" -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) .. && cmake --build . && ctest --output-on-failure && ./examples
clean:
	@rm -rf build
