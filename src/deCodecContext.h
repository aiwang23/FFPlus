//
// Created by 10484 on 25-1-1.
//

#ifndef DECODECCONTEXT_H
#define DECODECCONTEXT_H

#include "utils.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class stream;
class frame;
class packet;

/**
 * 解码器
 *
 * 调用顺序：
 * alloc -> open -> send/receive ->close
 *
 *
 */
class deCodecContext
{
public:
	deCodecContext();

	deCodecContext(const deCodecContext &ctx);

	/**
	 * alloc copyParametersFrom open 三合一
	 * 初始化解码器，复制码流 st 的参数到当前解码器，并打开解码器
	 * @param st 码流
	 */
	deCodecContext(const stream &st);

	deCodecContext& operator=(const deCodecContext &ctx);

	~deCodecContext();

	/**
	 * 初始化解码器
	 * @param codec	解码器参数, 可使用 findffDeCodec 获取
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int alloc(const AVCodec *codec);

	/**
	 * 初始化解码器
	 * @param codec_id 解码器类型, 具体请看 libavcodec/codec_id.h
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int alloc(AVCodecID codec_id);

	/**
	 * 打开解码器
	 * @param dict 可选参数
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int open(dictionary dict = {});

	int send(const packet &inPacket);

	int receive(const frame &outFrame);

	/**
	 * 关闭解码器
	 */
	void close();

	/**
	 * 获取解码器的 AVCodec*
	 * @return 解码器的 AVCodec*
	 */
	const AVCodec *ffCodec() const;

	AVCodecContext *ffCodecContext() const;

	/**
	 * 从码流 st 复制参数到当前解码器
	 * @param st 码流
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int copyParametersFrom(const stream &st);

	AVSampleFormat sampleFormat() const;

private:
	AVCodecContext *codec_context_ = nullptr;
	mutable  bool is_moved_ = false;
};


#endif //DECODECCONTEXT_H
