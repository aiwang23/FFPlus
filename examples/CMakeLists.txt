cmake_minimum_required(VERSION 3.20)

project(examples)

include_directories(${CMAKE_SOURCE_DIR}/src/)

set(examples_arr
        dec_mp4_yuv
        dec_mp4_pcm
        dec_mp4_yuv_conv_yuv420p_yuv422p
        dec_mp4_pcm_conv_fltp_s16
        enc_yuv_to_mp4
        enc_pcm_aac_conv_s16_fltp
        demux_mux
)

foreach (item IN LISTS examples_arr)
    add_executable(${item} ${item}.cpp)
    message(STATUS "add_executable(${item}  ${item}.cpp)")
endforeach ()

set(TARGET_LIBS
        # FFmpeg libraries
        avutil avcodec avformat avdevice avfilter swscale swresample
)

if (WIN32) # windows的库
    set(TARGET_LIBS
            ${TARGET_LIBS}
            # Windows libraries
            Strmiids.lib
    )
elseif (UNIX AND NOT APPLE) # linux的库

else () # 未知平台
    message(FATAL_ERROR "Unsupported operating system")
endif ()

foreach (item IN LISTS examples_arr)
    target_link_libraries(${item} FFPlus ${TARGET_LIBS})
    message(STATUS "target_link_libraries(${item}  FFPlus  ${TARGET_LIBS})")
endforeach ()

foreach (item IN LISTS examples_arr)
    install(TARGETS ${item} DESTINATION bin/)
endforeach ()