//
// Created by 10484 on 25-1-1.
//

#ifndef ENCODECCONTEXT_H
#define ENCODECCONTEXT_H


class packet;
class frame;
class stream;

extern "C" {
#include <libavcodec/avcodec.h>
}

#include "utils.h"

struct videoEnCodecConfig
{
	/* 视频参数 */
	int width{};                             // 宽度
	int height{};                            // 高度
	AVPixelFormat pix_fmt = AV_PIX_FMT_NONE; // 像素格式
	int gop_size = 10;                       // I帧间隔
	int max_b_frame = 0;                     // 最大B帧数量

	/* 通用参数 */
	AVCodecID codec_id = AV_CODEC_ID_NONE;   // 编码器ID
	int64_t bit_rate = 400000;               // 比特率
	AVRational time_base{};                  // 时间基
	AVRational frame_rate{};                 // 帧率
	unsigned int codec_tag = 0;              // 编码器标签
	int flags = AV_CODEC_FLAG_GLOBAL_HEADER; // 编码器标志
};

struct audioEnCodecConfig
{
	/* 音频参数 */
	int sample_rate{};                                 // 采样率
	int channels{};                                    // 声道数
	AVSampleFormat sample_format = AV_SAMPLE_FMT_NONE; // 采样格式

	/* 通用参数 */
	AVCodecID codec_id = AV_CODEC_ID_NONE;   // 编码器ID
	int64_t bit_rate = 400000;               // 比特率
	AVRational time_base{};                  // 时间基
	AVRational frame_rate{};                 // 帧率
	unsigned int codec_tag = 0;              // 编码器标签
	int flags = AV_CODEC_FLAG_GLOBAL_HEADER; // 编码器标志
};

/**
 * 编码器
 * 调用顺序：
 * alloc -> open ->  send/receive ->close
 *
 * TODO
 */
class enCodecContext
{
public:
	enCodecContext();

	enCodecContext(const enCodecContext & ctx);

	/**
	 * 初始化编码器，配置参数后调用 open 初始化
	 * 相当于 alloc 和 open 二合一
	 * @param config
	 * @param dict
	 */
	enCodecContext(videoEnCodecConfig config, dictionary dict = {});

	enCodecContext(audioEnCodecConfig config, dictionary dict = {});

	enCodecContext & operator=(const enCodecContext & ctx);

	~enCodecContext();

	/**
	 * 初始化编码器
	 * @param codec 编码器，可使用 findffEnCodec 获取
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int alloc(const AVCodec *codec);

	/**
	 * 初始化编码器
	 * @param codec_id 编码器类型, 具体请看 libavcodec/codec_id.h
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int alloc(AVCodecID codec_id);

	/**
	 * 打开视频编码器
	 * ! 必须先调用 alloc, 并填充参数
	 * @param config 视频编码器配置
	 * @param dict 可选参数
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int open(videoEnCodecConfig config = {}, dictionary dict = {});

	/**
	 * 打开音频编码器
	 * ! 必须先调用 alloc, 并填充参数
	 * @param config 音频编码器配置
	 * @param dict 可选参数
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int open(audioEnCodecConfig config = {}, dictionary dict = {});

	/**
	 * 发送数据到编码器
	 * @param inFrame 要输入的 frame
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int send(const frame &inFrame);

	/**
	 * 从编码器接收数据
	 * @param outPacket 要输出的 packet
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int receive(const packet &outPacket);

	/**
	 * 关闭编码器
	 */
	void close();

	/**
	 * 获取编码器的 AVCodec*
	 * @return 编码器的 AVCodec*
	 */
	const AVCodec *ffCodec() const;

	/**
	 * 获取编码器的 AVCodecContext*
	 * @return 编码器的 AVCodecContext*
	 */
	AVCodecContext *ffCodecContext() const;

	int copyParametersFrom(const stream &st);

	AVRational timebase();

	static AVPixelFormat getPixFmt(const char *name);

	bool isOpened();

private:
	AVCodecContext *codec_context_ = nullptr;
	mutable bool is_moved_ = false;
};


#endif //ENCODECCONTEXT_H
