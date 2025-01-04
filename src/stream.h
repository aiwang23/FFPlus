//
// Created by 10484 on 25-1-1.
//

#ifndef STREAM_H
#define STREAM_H


extern "C" {
#include <libavformat/avformat.h>
}


class deCodecContext;
class enCodecContext;

/**
 * 码流
 */
class stream
{
public:
	stream();

	explicit stream(AVStream *st);

	~stream();

	/**
	 * 从编码器codec_context 复制参数到此码流
	 * @param codec_context 编码器
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int copyParametersFrom(const enCodecContext &codec_context);

	/**
	 * 从另一个码流st 复制参数到此码流
	 * @param st 另一个码流
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int copyParametersFrom(const stream &st);

	int copyParametersTo(const stream &st);

	/**
	 * 将此码流参数复制到deCodecContext
	 * @param codec_context
	 * @return
	 */
	int copyParametersTo(const deCodecContext& codec_context);

	int copyParametersTo(const enCodecContext &codec_context);

	/**
	 * 获取此码流对象的AVStream*
	 * @return
	 */
	AVStream *ffStream() const;

	/**
	 * 获取此码流对象的编码器类型AVCodecID
	 * @return
	 */
	AVCodecID ffCodecID();

	/**
	 * 获取时间基
	 * @return
	 */
	AVRational timebase();

	/**
	 * 获取码流在文件formatContext中的索引
	 * @return
	 */
	int index();

	const AVCodec *ffDeCodec();

	const AVCodec *ffEnCodec();

	/**
	 * 为视频流时，获取宽度
	 * @return
	 */
	int width() const;

	/**
	 * 为视频流时，获取高度
	 * @return
	 */
	int height() const;

	/**
	 * 视频流时，为像素格式 AVPixelFormat
	 * 音频流时，为采样格式 AVSampleFormat
	 * @return
	 */
	int format()const;

	int sampleRate() const;

	int channels() const;

	int samples() const;

	const AVCodecParameters * ffCodecPara();

private:
	AVStream *stream_ = nullptr;
};


#endif //STREAM_H
