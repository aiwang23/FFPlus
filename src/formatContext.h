//
// Created by 10484 on 25-1-1.
//

#ifndef FORMATCONTEXT_H
#define FORMATCONTEXT_H

#include "stream.h"

extern "C" {
#include <libavformat/avformat.h>
}

#include "utils.h"

class packet;

/**
 * 文件
 *
 * 解封装顺序：
 * openInput -> read -> close
 *
 * 封装顺序：
 * allocOutput -> newStream -> openOutput -> write -> close
 */
class formatContext
{
	/**
	 * 以哪种方式打开文件
	 * NONE: 未知
	 * INPUT: 输入文件，解封装
	 * OUTPUT: 输出文件，封装
	 */
	enum fileType { NONE = -1, INPUT, OUTPUT };

public:
	explicit formatContext();

	formatContext(const formatContext &fmt);

	/**
	 * 快速打开输入文件
	 * @param url 文件路径
	 * @param fmt 指定封装格式
	 * @param dict 可选参数
	 */
	explicit formatContext(const char *url, const AVInputFormat *fmt = nullptr, dictionary dict = {});

	formatContext &operator =(const formatContext &fmt);

	~formatContext();

	/**
	 * 打开输入文件
	 *
	 * @param url 文件路径
	 * @param fmt 指定封装格式
	 * @param dict 可选参数
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int openInput(const char *url, const AVInputFormat *fmt = nullptr, dictionary dict = {});

	/**
	 * 初始化输出文件
	 *
	 * @param url 文件路径
	 * @param ofmt 指定封装格式
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int allocOutput(const char *url, const AVOutputFormat *ofmt = nullptr);

	/**
	 * 创建新的码流
	 * @param codec 编码器类型
	 * @return stream 码流对象
	 */
	stream newStream(const AVCodec *codec);

	/**
	 * 新建码流，并从其他码流复制参数
	 * @param st 码流对象
	 * @return stream 新码流
	 */
	stream copyStream(const stream &st);

	/**
	 * 新建码流，并从编码器复制参数
	 * @param codec_context 编码器
	 * @return stream 新码流
	 */
	stream copyStream(const enCodecContext &codec_context);


	/**
	 * 创建并打开输出文件
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int openOutput();

	/**
	 * 关闭文件
	 */
	void close();

	/**
	 * 根据码流索 idx 引获取码流对象 stream
	 * @param idx 码流索引
	 * @return stream 码流对象
	 */
	stream streamAt(int idx);

	/**
	 * 获取可用的视频流
	 * @return stream 视频流对象
	 */
	stream videoStream();

	/**
	 * 获取可用的音频流
	 * @return stream 音频流对象
	 */
	stream audioStream();

	/**
	 * 打印文件封装格式信息
	 */
	void dump();

	/**
	 * 从文件中读取 packet
	 * @param pkt packet 对象
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int read(const packet &pkt);

	/**
	 * 写入 packet 到文件
	 * @param pkt packet 对象
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int write(const packet &pkt);

	/**
	 * 根据流索引 idx 获取对应的码流
	 * @param idx 码流索引
	 * @return stream 码流对象
	 */
	stream operator[](int idx);

	/**
	 * 获取可用的视频流的索引
	 * @return 视频流的索引
	 */
	int videoStreamIndex();

	/**
	 * 获取可用的音频流的索引
	 * @return 音频流的索引
	 */
	int audioStreamIndex();

	/**
	 * @brief 获取文件路径
	 * 
	 * @return std::string 文件路径
	 */
	std::string url();

	/**
	 * @brief 获取封装格式名称
	 * 
	 * @return std::string 封装格式名称
	 */
	std::string format_name();

	/**
	 * @brief 获取文件大小, 单位为byte
	 * 
	 * @return int64_t 
	 */
	int64_t filesize() const;

	/**
	 * @brief 获取文件总时长, 单位为秒
	 * 
	 * @return double 
	 */
	double duration() const;

	/**
	 * @brief 获取文件码率, 单位为bit/s
	 * 
	 * @return long long
	 */
	long long bitrate() const;

	

private:
	AVFormatContext *fmt_context_ = nullptr;
	fileType open_file_t_ = fileType::NONE;
	mutable bool is_moved_ = false;
	bool is_wrote_head = false;
};


#endif //FORMATCONTEXT_H
