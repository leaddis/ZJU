cmake_minimum_required(VERSION 3.5)
project(splines VERSION 1.2.0)

# 禁止在顶层目录编译
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# 设置 C99 编译标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# 设置 C++17 编译标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置外部 CMake 模块路径，以便存放自己编写的 cmake 模块
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# 设置Release版本的编译参数
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# 设置 CMAKE_BUILD_TYPE
# Debug 会打开调试选项，Release 会启用各种优化
# 默认为 Debug
if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    message(STATUS "CMAKE_BUILD_TYPE is unset, defaulting to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        add_compile_options(-g)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # note: not working for current version because C++17 support is poor
    add_compile_options(-Wall -Wextra -Wpedantic -inline-forceinline)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        add_compile_options(-g)
    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        # C99 是允许使用的，但 Clang 依然可能会给出警告，因此屏蔽
        add_compile_options(-Wno-c99-extensions)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        add_compile_options(-g)
    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        # C99 是允许使用的，但 Clang 依然可能会给出警告，因此屏蔽
        add_compile_options(-Wno-c99-extensions)
    endif()
else()
    message(WARNING "Unsupported CXX compiler ${CMAKE_CXX_COMPILER_ID}")
endif()

# 将所有 src/*.cpp 文件列出来
file(GLOB_RECURSE CXX_SOURCE ${PROJECT_SOURCE_DIR}/src/*.cpp)

# 这里 SOURCE 表示所有用于编译成内置函数库的 cpp 文件
set(SOURCE ${CXX_SOURCE}
        src/csaps/csaps.cpp
        src/csaps/csaps.h)

# 设置 CMake 选项
option(BUILD_SHARED_LIBS "Enable shared library. Default: ON" ON)
option(BUILD_TEST "Enable the build of test/test*. Default: OFF" OFF)
set(PACKAGE_NAME_SUFFIX "" CACHE STRING "Change the suffix of the package name. Default is empty")

# 配置文件
configure_file(config.json.in config.json @ONLY)

# 检查 core/worker 外部依赖
# 需要预先将内核库放到 deps/core 目录中
set(BALTAM_SDK_DIR "" CACHE STRING "Baltamatica SDK search path")

if ("${BALTAM_SDK_DIR}" STREQUAL "")
    set(BALTAM_SDK_DIR "deps/core")
endif()

# Boost
find_package(Boost REQUIRED)

#Core
find_package(baltam_core 3.2.0 REQUIRED COMPONENTS dev HINTS ${BALTAM_SDK_DIR})

# 导出模块
add_library(splines_main SHARED ${SOURCE})
set_target_properties(splines_main PROPERTIES OUTPUT_NAME "main")
set_target_properties(splines_main PROPERTIES PREFIX "")
target_link_libraries(splines_main PRIVATE baltam_core::baltam_core)

# 额外依赖
if (APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/openblas")
    # try to find frameworks after standard libraries or headers
    set(CMAKE_FIND_FRAMEWORK "LAST")
endif()

# LAPACKE
find_package(LAPACKE REQUIRED)
target_include_directories(splines_main PRIVATE ${LAPACKE_INCLUDE_DIRS})
target_link_libraries(splines_main PRIVATE ${LAPACKE_LIBRARIES})


# 去除多余符号
set_target_properties(splines_main PROPERTIES LINK_FLAGS_RELEASE -s)
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--exclude-all-symbols")
else()
    set_target_properties(splines_main PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)
endif()

include_directories("${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/src")

# 编译选项
add_definitions(-DBALTAM_USING_SHARED_PTR)
add_definitions(-DBALTAM_REGISTER)
if (BUILD_SHARED_LIBS)
    add_definitions(-DBALTAM_BUILD_DYN_LIB -DBALTAM_DLL_EXPORTS)
endif()

# 添加 test 目录下的测试程序
if (BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

# install 相关目标
install(TARGETS splines_main
    RUNTIME DESTINATION CurveFitting COMPONENT main
    LIBRARY DESTINATION CurveFitting COMPONENT main)
install(FILES ${PROJECT_BINARY_DIR}/config.json DESTINATION CurveFitting COMPONENT main)

# 打包
set(CPACK_COMPONENTS_ALL main)
set(CPACK_GENERATOR "ZIP")
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_STRIP_FILES ON)

if (NOT "${PACKAGE_NAME_SUFFIX}" STREQUAL "")
    message(STATUS "Append '${PACKAGE_NAME_SUFFIX}' to the package file name")
    set(CPACK_ARCHIVE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${PACKAGE_NAME_SUFFIX}")
else()
    message(STATUS "No package name suffix given")
endif()
include(CPack)

# 输出编译信息
include(${CMAKE_SOURCE_DIR}/cmake/baltam_utils.cmake)
message("-- Configuration summary for splines:")
message("   -- prefix: ${CMAKE_INSTALL_PREFIX}")
message("   -- build: ${CMAKE_BUILD_TYPE}")
message("   -- SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message("   -- TESTS: ${BUILD_TEST}")
cprsummary("CC" "${CMAKE_C_COMPILER}"
        "${CMAKE_C_FLAGS_DEBUG}"
        "${CMAKE_C_FLAGS_MINSIZEREL}"
        "${CMAKE_C_FLAGS_RELEASE}"
        "${CMAKE_C_FLAGS_RELWITHDEBINFO}"
        "${CMAKE_C_FLAGS}")
cprsummary("CXX" "${CMAKE_CXX_COMPILER}"
        "${CMAKE_CXX_FLAGS_DEBUG}"
        "${CMAKE_CXX_FLAGS_MINSIZEREL}"
        "${CMAKE_CXX_FLAGS_RELEASE}"
        "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
        "${CMAKE_CXX_FLAGS}")
