cmake_minimum_required(VERSION 3.20)

project(FFPlus)

#* 生成可执行文件
add_library(FFPlus STATIC

        utils.h
        utils.cpp

        formatContext.cpp
        formatContext.h

        stream.cpp
        stream.h

        packet.cpp
        packet.h

        enCodecContext.cpp
        enCodecContext.h

        frame.cpp
        frame.h

        deCodecContext.cpp
        deCodecContext.h

        frameConverter.cpp
        frameConverter.h
)
