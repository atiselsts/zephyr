cmake_minimum_required(VERSION 3.13.1)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(on_board_nn)

# export TENSORFLOW=/home/atis/source/tensorflow
zephyr_include_directories($ENV{TENSORFLOW})
zephyr_include_directories($ENV{TENSORFLOW}/tensorflow/lite/micro/tools/make/downloads/flatbuffers/include)

#zephyr_include_directories(/opt/zephyr-sdk-0.12.3/arm-zephyr-eabi/arm-zephyr-eabi/include/c++/10.2.0)
#zephyr_include_directories(/opt/zephyr-sdk-0.12.3/arm-zephyr-eabi/arm-zephyr-eabi/include/c++/10.2.0/arm-zephyr-eabi/)
#zephyr_include_directories(/opt/zephyr-sdk-0.12.3/arm-zephyr-eabi/arm-zephyr-eabi/include/)

target_compile_options(app PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics -fno-use-cxa-atexit>)

add_library(libtensorflow STATIC IMPORTED GLOBAL)
set_target_properties(libtensorflow PROPERTIES IMPORTED_LOCATION $ENV{ZEPHYR_BASE}/samples/on-board-nn/libtensorflow-microlite.a)
target_link_libraries(app PUBLIC libtensorflow)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/nn.cpp)
