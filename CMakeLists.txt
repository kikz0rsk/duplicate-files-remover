set(CMAKE_TOOLCHAIN_FILE "d:/Development/vcpkg/scripts/buildsystems/vcpkg.cmake")

cmake_minimum_required(VERSION 3.27)
project(duplicate_remover)

include_directories("include")

set(CMAKE_CXX_STANDARD 20)

find_package(xxHash CONFIG REQUIRED)

add_executable(duplicate_remover src/main.cpp
        src/Core.cpp
        src/FileEntry.cpp)

target_link_libraries(duplicate_remover PRIVATE xxHash::xxhash)