# CMake Toolchain File for Cross-Compiling AppliGPS to NXP i.MX8 (ARM64 / AArch64 Linux)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Cross Compilers
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Target sysroot / search paths configuration
# Set CMAKE_FIND_ROOT_PATH_MODE_PACKAGE to BOTH so CMake can locate Qt5
# in host sysroot or specified CMAKE_PREFIX_PATH if sysroot path is provided.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
