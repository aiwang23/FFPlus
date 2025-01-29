//
// Created by 10484 on 25-1-3.
//

#ifndef FRAMECONVERTER_H
#define FRAMECONVERTER_H

extern "C" {
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

// auto a = sws_getContext();
// auto a1 = sws_scale();
// auto b = swr_alloc_set_opts2();
// auto b1 = swr_convert();

class frame;

struct videoConvertConfig
{
	int src_w{};
	int src_h{};
	AVPixelFormat src_pix_fmt = AV_PIX_FMT_NONE;
	int dst_w{};
	int dst_h{};
	AVPixelFormat dst_pix_fmt = AV_PIX_FMT_NONE;
	int flags{};
	SwsFilter *src_filter = nullptr;
	SwsFilter *dst_filter = nullptr;
	const double param{};
};

struct audioConvertConfig
{
	int src_channels;
	AVSampleFormat src_sample_fmt;
	int src_sample_rate;
	int dst_channels;
	AVSampleFormat dst_sample_fmt;
	int dst_sample_rate;
	int log_level_offset;
	void *log_ctx;
};

class frameConverter
{
private:
	enum convertType { NONE = -1, VIDEO, AUDIO };

public:
	frameConverter();

	frameConverter(const frameConverter & cnt);

	/**
	 * ��ʼ����Ƶת������������
	 * @param config ��Ƶ����
	 */
	frameConverter(videoConvertConfig config);

	/**
	 * ��ʼ����Ƶת������������
	 * @param config ��Ƶ����
	 */
	frameConverter(audioConvertConfig config);

	~frameConverter();

	int alloc(videoConvertConfig config);

	int alloc(audioConvertConfig config);

	int convert(const frame &inFrame, const frame &outFrame);

	void free();

private:
	SwsContext *sws_context_ = nullptr;
	SwrContext *swr_context_ = nullptr;
	convertType type_ = NONE;
	mutable bool is_moved_ = false;
};


#endif //FRAMECONVERTER_H
