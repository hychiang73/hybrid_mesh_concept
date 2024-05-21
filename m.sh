#!/bin/sh
JETSON_NANO_KERNEL_SOURCE=/home/parallels/Workplace/nvidia/kernel_out/build
#JETSON_NANO_KERNEL_SOURCE=/home/parallels/workspace/jetson-dev/kernel_out/build
TOOLCHAIN_PREFIX=/home/parallels/Workplace/nvidia/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
#TOOLCHAIN_PREFIX=/home/parallels/workspace/jetson-dev/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-

make KLIB_BUILD=$JETSON_NANO_KERNEL_SOURCE ARCH=arm64 CROSS_COMPILE=${TOOLCHAIN_PREFIX} -j8 clean
make KLIB_BUILD=$JETSON_NANO_KERNEL_SOURCE ARCH=arm64 CROSS_COMPILE=${TOOLCHAIN_PREFIX} -j8
