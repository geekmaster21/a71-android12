#!/bin/bash

DEFCONFIG=sm7150_sec_a71_eur_open_caliban_defconfig

# Declare your CLANG n GCC Location HERE!
CLANG_PATH=/home/ubuntu/Pictures/Kernel/llvm-sdclang
GCC_PATH=/home/ubuntu/Pictures/Kernel/gcc
KERNEL_MAKE_ENV="DTC_EXT=$HOME/Music/Kernel/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y"
export ANDROID_MAJOR_VERSION=R

make O=out ARCH=arm64 $DEFCONFIG

PATH="$CLANG_PATH/bin:$GCC_PATH/bin:${PATH}" \
                      make -j8 O=out $KERNEL_MAKE_ENV \
                      ARCH=arm64 \
                      CC=clang \
                      CLANG_TRIPLE=aarch64-linux-gnu- \
                      CROSS_COMPILE=$GCC_PATH/bin/aarch64-linux-android- \
                      AR=llvm-ar \
                      NM=llvm-nm \
                      OBJCOPY=llvm-objcopy \
                      OBJDUMP=llvm-objdump \
                      STRIP=llvm-strip
