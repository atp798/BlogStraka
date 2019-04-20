SET(CROSS_COMPILE_ROOT /home/raspberrypi3b+/rootfs)
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_C_COMPILER ${CROSS_COMPILE_ROOT}/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER ${CROSS_COMPILE_ROOT}/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++)
SET(CMAKE_FIND_ROOT_PATH ${CROSS_COMPILE_ROOT})
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_CXX_LINK_FLAGS "--sysroot=${CROSS_COMPILE_ROOT}")

include_directories(${CROSS_COMPILE_ROOT}/usr/include)
include_directories(${CROSS_COMPILE_ROOT}/usr/include/arm-linux-gnueabihf)

