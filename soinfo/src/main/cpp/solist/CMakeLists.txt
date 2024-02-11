cmake_minimum_required(VERSION 3.18.1)
# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)



# configure a header file to pass some of the CMake settings
# to the source code

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h

# add the executable
include_directories(${CMAKE_SOURCE_DIR}/)

add_library( # Sets the name of the library.
        solist
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        solist.cpp
        elf_symbol_resolver.cpp)

# in android studio build.gradle


find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )

target_link_libraries( # Specifies the target library.
        solist
        ${log-lib})


