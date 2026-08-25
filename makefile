

all: unix



xcode:
	#@echo "making... ${JOBS} $(MAKEFLAGS) "
	@if [ ! -d "./build_xcode" ]; then mkdir build_xcode; fi
	@cmake -G Xcode -S ./ -B ./build_xcode || cmake3 -G Xcode -S ./ -B ./build_xcode
	#@make -j4 -C ./build
	#@if [ ! -d "./bin" ]; then mkdir bin; fi
	#@cp -p ./build/bin/*  ./bin
	


unix:  CMakeLists.txt
	#@echo "making... ${JOBS} $(MAKEFLAGS) "
	@if [ ! -d "./build" ]; then mkdir build; fi
	@cmake -S ./ -B ./build || cmake3 -S ./ -B ./build
	@make -j4 -C ./build
	@if [ ! -d "./bin" ]; then mkdir bin; fi
	@cp -p ./build/bin/*  ./bin

tests: CMakeLists.txt 
	@if [ ! -d "./build" ]; then mkdir build; fi
	@cmake -S ./ -B ./build -DGROOT_BUILD_TESTS=ON || cmake3 -S ./ -B ./build -DGROOT_BUILD_TEST=ON
	@make -j4 -C ./build
	@if [ ! -d "./bin" ]; then mkdir bin; fi
	@cp -p ./build/bin/*  ./bin
	@ctest --test-dir ./build/tests --output-on-failure



clean: 
	@echo "cleaning..."
	@if [ -d "./build" ]; then rm -rf build; fi
	@if [ -d "./build_xcode" ]; then rm -rf build_xcode; fi
	@if [ -d "./bin" ]; then rm -rf bin; fi
	


