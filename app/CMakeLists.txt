cmake_minimum_required(VERSION 3.6.0)

file(GLOB SOURCE ${CMAKE_SOURCE_DIR}/src/main/cpp/*.cpp)
add_library(
        kjplayer-lib

        SHARED

        ${SOURCE})


# 设置头文件
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include)

# 设置lib路径
set(lib_path ${CMAKE_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${lib_path}")


target_link_libraries(
        kjplayer-lib
        avfilter
        avformat
        avcodec
        avutil
        swresample
        swscale
        log
        z
        OpenSLES
        android
)