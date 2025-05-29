
TARGET=betasort

J=1

all: $(TARGET)


JOBS := $(shell echo $(MAKEFLAGS) | sed -n 's/.*-j\([0-9][0-9]*\).*/\1/p')
TEST := "test"


$(TARGET):  CMakeLists.txt
	@echo "making... J=${JOBS} | $(MAKEFLAGS)"
	#@echo "making... ${JOBS} $(MAKEFLAGS) ${TEST} "
	@if [ ! -d "./build" ]; then mkdir build; fi
	@cmake -S ./ -B ./build || cmake3 -S ./ -B ./build
	@make -C ./build
	@if [ ! -d "./bin" ]; then mkdir bin; fi
	@cp -p ./build/bin/*  ./bin


clean: 
	@echo "cleaning..."
	@if [ -d "./build" ]; then rm -rf build; fi
	@if [ -d "./bin" ]; then rm -rf bin; fi



