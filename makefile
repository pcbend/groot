
TARGET=betasort


all: $(TARGET)

TEST := "test"


$(TARGET):  CMakeLists.txt
	#@echo "making... ${JOBS} $(MAKEFLAGS) ${TEST} "
	@if [ ! -d "./build" ]; then mkdir build; fi
	@cmake -S ./ -B ./build || cmake3 -S ./ -B ./build
	@make -j4 -C ./build
	@if [ ! -d "./bin" ]; then mkdir bin; fi
	@cp -p ./build/bin/*  ./bin


clean: 
	@echo "cleaning..."
	@if [ -d "./build" ]; then rm -rf build; fi
	@if [ -d "./bin" ]; then rm -rf bin; fi



