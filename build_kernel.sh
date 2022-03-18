#!/bin/bash

git clone https://github.com/proprietary-stuff/llvm-arm-toolchain-ship-10.0.git llvm-sdclang
git clone https://github.com/LineageOS/android_prebuilts_gcc_linux-x86_aarch64_aarch64-linux-android-4.9.git gcc

DEFCONFIG=a71_defconfig

# Declare your CLANG n GCC Location HERE!
CLANG_PATH=/workspace/a71-android12/llvm-sdclang
GCC_PATH=/workspace/a71-android12/gcc
KERNEL_MAKE_ENV="DTC_EXT=/workspace/a71-android12/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y"
export ANDROID_MAJOR_VERSION=R

make O=out ARCH=arm64 $DEFCONFIG

PATH="$CLANG_PATH/bin:$GCC_PATH/bin:${PATH}" \
                      make O=out $KERNEL_MAKE_ENV \
                      ARCH=arm64 \
                      CC=clang \
                      LD=ld.lld \
                      CROSS_COMPILE=aarch64-linux-android- \
                      CLANG_TRIPLE=aarch64-linux-gnu- \
                      AR=llvm-ar \
                      NM=llvm-nm \
                      OBJCOPY=llvm-objcopy \
                      OBJDUMP=llvm-objdump \
                      STRIP=llvm-strip
