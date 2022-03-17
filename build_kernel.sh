#!/bin/bash

DEFCONFIG=arch/arm64/configs/a71_defconfig

# Declare your CLANG n GCC Location HERE!
CLANG_PATH=/workspace/a71-android12/llvm-sdclang
GCC_PATH=/workspace/a71-android12/gcc
KERNEL_MAKE_ENV="DTC_EXT=/workspace/a71-android12/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y"
export ANDROID_MAJOR_VERSION=R

make O=out ARCH=arm64 $DEFCONFIG

PATH="$CLANG_PATH/bin:${PATH}" \
                      make -j8 O=out $KERNEL_MAKE_ENV \
                      ARCH=arm64 \
                      CC=clang \
                      LD=ld.lld \
                      CROSS_COMPILE=aarch64-linux-gnu- \
                      NM=llvm-nm \
                      OBJCOPY=llvm-objcopy \
                      OBJDUMP=llvm-objdump \
                      STRIP=llvm-strip
