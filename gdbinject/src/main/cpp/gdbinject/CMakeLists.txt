# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# 设置目标导入路径，这里是一行


#if (${RXDEBUG} STREQUAL OFF)
#    message("this is RXDEBUG OFF")
#else()
#    message("this is RXDEBUG ON")
#endif()




#include_directories(${CMAKE_SOURCE_DIR}/include)

add_library( # Sets the name of the library.
        gdbinject
        # Sets the library as a shared library.
        STATIC
        # Provides a relative path to your source file(s).
        gdbinject.c
        signals.c
        packets.c
        utils.c
        gdb/signals.def
        )



