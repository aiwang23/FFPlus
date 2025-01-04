cmake_minimum_required(VERSION 3.20)

project(FFPlus)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_STANDARD 11)

set(CMAKE_BUILD_TYPE DEBUG)
#set(CMAKE_BUILD_TYPE release)

#* 寻找头文件和库
#! 这里请换成自己电脑的ffmpeg的路径
if (WIN32)
    # FFmpeg6
    include_directories("C:/Software/ffmpeg-master-latest-win64-gpl-shared/include")
    link_directories("C:/Software/ffmpeg-master-latest-win64-gpl-shared/lib")
elseif (UNIX AND NOT APPLE)
    include_directories("/usr/local/include")
    link_directories("/usr/local/lib")
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

add_subdirectory(src)
add_subdirectory(examples)

install(
        DIRECTORY ${CMAKE_SOURCE_DIR}/src/ DESTINATION include/
        FILES_MATCHING PATTERN "*.h"
)

install(
        TARGETS FFPlus DESTINATION lib/
)

