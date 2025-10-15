# 设置cmake最低版本跟项目名称
cmake_minimum_required(VERSION 3.0)
project(mmprc)

cmake_policy(SET CMP0079 NEW)

# 设置项目可执行文件得输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 设置项目编译头文件搜索路径

include_directories(${PROJECT_SOURCE_DIR}/example)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/include)

# 设置项目库文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/lib)


add_custom_target(clean-all
    # COMMAND ${CMAKE_COMMAND} -E echo "🧹 Cleaning all build files..."
    COMMAND ${CMAKE_COMMAND} -E remove -f
        ${CMAKE_BINARY_DIR}/CMakeCache.txt
        ${CMAKE_BINARY_DIR}/cmake_install.cmake
        ${CMAKE_BINARY_DIR}/Makefile
        ${PROJECT_SOURCE_DIR}/bin/provider
    COMMAND ${CMAKE_COMMAND} -E remove_directory
        ${CMAKE_BINARY_DIR}/CMakeFiles
        ${CMAKE_BINARY_DIR}/example
        ${CMAKE_BINARY_DIR}/src
        

    # COMMAND ${CMAKE_COMMAND} -E echo "✅ Done!"
)

add_subdirectory(example)
add_subdirectory(src)
