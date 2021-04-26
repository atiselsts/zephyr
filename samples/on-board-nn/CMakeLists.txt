# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(on_board_rf)

zephyr_include_directories(/home/atis/source/tensorflow)
zephyr_include_directories(/home/atis/source/tensorflow/tensorflow/lite/micro/tools/make/downloads/flatbuffers/include)

zephyr_include_directories(/opt/zephyr-sdk-0.12.3/arm-zephyr-eabi/arm-zephyr-eabi/include/c++/10.2.0)
zephyr_include_directories(/opt/zephyr-sdk-0.12.3/arm-zephyr-eabi/arm-zephyr-eabi/include/c++/10.2.0/arm-zephyr-eabi/)
zephyr_include_directories(/opt/zephyr-sdk-0.12.3/arm-zephyr-eabi/arm-zephyr-eabi/include/)

zephyr_compile_options(-fno-strict-aliasing)

add_library(libtensorflow STATIC IMPORTED GLOBAL)
set_target_properties(libtensorflow PROPERTIES IMPORTED_LOCATION /home/atis/source/tensorflow/tensorflow/lite/micro/tools/make/gen/zephyr_nrf_cortex-m4_micro/lib/libtensorflow-microlite.a)
target_link_libraries(app PUBLIC libtensorflow)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/nn.cpp)
