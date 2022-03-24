#!/bin/bash

git clone --depth=1 https://github.com/proprietary-stuff/llvm-arm-toolchain-ship-10.0.git llvm-sdclang
git clone --depth=1 https://github.com/LineageOS/android_prebuilts_gcc_linux-x86_aarch64_aarch64-linux-android-4.9.git gcc

DEFCONFIG=a71_defconfig

# Declare your CLANG n GCC Location HERE!
export CLANG_PATH=/workspace/a71-android12/llvm-sdclang
export GCC_PATH=/workspace/a71-android12/gcc
KERNEL_MAKE_ENV="DTC_EXT=/workspace/a71-android12/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y"
export ANDROID_MAJOR_VERSION=S

make O=out ARCH=arm64 $DEFCONFIG

export PATH="$CLANG_PATH/bin:$GCC_PATH/bin:${PATH}"
                      make -j16  O=out $KERNEL_MAKE_ENV \
                      ARCH=arm64 \
                      CC=clang \
                      CROSS_COMPILE=$GCC_PATH/bin/aarch64-linux-android- \
                      CLANG_TRIPLE=aarch64-linux-gnu- \
                      AR=$CLANG_PATH/bin/llvm-ar \
                      NM=$CLANG_PATH/bin/llvm-nm \
                      OBJCOPY=$CLANG_PATH/bin/llvm-objcopy \
                      OBJDUMP=$CLANG_PATH/bin/llvm-objdump \
                      STRIP=$CLANG_PATH/bin/llvm-strip
