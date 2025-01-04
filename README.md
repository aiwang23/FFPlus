# 介绍
这是一个ffmpeg常用c api的 c++封装库。
当前使用ffmpeg版本为6.1。
目的是简化ffmpeg c api的操作。
windows和linux都可用。

> 注意：
> 在编译前，请先把 **FFPlus/CMakeLists.txt** 的 **ffmpeg安装路径** 换成自己电脑的安装路径

# 如何编译
```shell
cmake -S . -B build/
cmake --build build/
```

# 如何安装
```shell
cmake --install build/ --prefix path/to/
```


# 示例

- 复制文件 [demux_mux.cpp](examples%2Fdemux_mux.cpp)
- 解码mp4为yuv420p文件 [dec_mp4_yuv.cpp](examples%2Fdec_mp4_yuv.cpp)
- 解码MP4为YUV420p文件，并转换为yuv422p [dec_mp4_yuv_conv_yuv420p_yuv422p.cpp](examples%2Fdec_mp4_yuv_conv_yuv420p_yuv422p.cpp)
- 编码yuv420为mp4文件 [enc.cpp](examples%2Fenc.cpp)[enc_yuv_to_mp4.cpp](examples%2Fenc_yuv_to_mp4.cpp)
- 解码MP4为pcm [dec_mp4_pcm.cpp](examples%2Fdec_mp4_pcm.cpp) 
- 解码MP4为pcm，并把fltp转换为s16p [dec_mp4_pcm_conv_fltp_s16.cpp](examples%2Fdec_mp4_pcm_conv_fltp_s16.cpp)
