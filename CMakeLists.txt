cmake_minimum_required(VERSION 3.16)

# some option
option(GEN_SHARED_LIB "generate shared lib: default ON" ON)
option(GEN_STATIC_LIB "generate static lib: default OFF" OFF)
option(USE_EXAMPLE "compile example program: default ON" ON)
option(USE_CLANG "use clang to compile program: default OFF" OFF)

# compiler choose
if(USE_CLANG)
    set(CMAKE_ASM_COMPILER clang)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
    # speed gdb time
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-gdb-index")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-gdb-index")
else()
    set(CMAKE_ASM_COMPILER gcc)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
endif()




project(Apollo_CPP_Client LANGUAGES CXX C)

# set cpp 11 standard not allow extension
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set cpp file extension name
set(CPP_HEADER_EXT "*.h" "*.hpp" "*.hrp" "*.hxx")
set(CPP_SOURCE_EXT "*.cpp" "*.cxx" "*.cc")
set(C_SOURCE_EXT "*.c")


#  compiler flag
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64 -D__STDC_FORMAT_MACROS")
# speed compile
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")

# if(USE_CLANG)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-mismatched-tags -Wno-unused-function -fcolor-diagnostics")
# else()
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-but-set-variable -Wno-sign-compare -Wno-comment -Wno-unused-function")
# endif()
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")



# default use release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "choose the type of build, default use Release"
            FORCE)
endif()

# set gen dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/release/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/release/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/release/arc")


# subdirectory
add_subdirectory(src)
add_subdirectory(apollo_client_proxy)
if(USE_EXAMPLE)
    add_subdirectory(example)
endif()