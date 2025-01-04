//
// Created by 10484 on 25-1-1.
//

#include "enCodecContext.h"
#include "stream.h"
#include "frame.h"
#include "packet.h"

enCodecContext::enCodecContext()
= default;

enCodecContext::enCodecContext(videoEnCodecConfig config, dictionary dict)
{
	alloc(config.codec_id);
	open(config, dict);
}

enCodecContext::enCodecContext(audioEnCodecConfig config, dictionary dict)
{
	alloc(config.codec_id);
	open(config, dict);
}

enCodecContext::~enCodecContext()
{
	close();
}

int enCodecContext::alloc(const AVCodec *codec)
{
	if (!codec)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	// 分配解码器
	codec_context_ = avcodec_alloc_context3(codec);
	if (!codec_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	return 0;
}

int enCodecContext::alloc(AVCodecID codec_id)
{
	return alloc(findffEnCodec(codec_id));
}

int enCodecContext::open(videoEnCodecConfig config, dictionary dict)
{
	if (!codec_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	//* 通用参数
	// 编解码类型
	codec_context_->codec_id =
			config.codec_id != AV_CODEC_ID_NONE ? config.codec_id : codec_context_->codec_id;
	// 比特率
	codec_context_->bit_rate =
			config.bit_rate >= 0 ? config.bit_rate : codec_context_->bit_rate;
	// 时间基
	codec_context_->time_base =
			config.time_base.num != 0 && config.time_base.den != 0 ? config.time_base : codec_context_->time_base;
	// 帧率
	codec_context_->framerate =
			config.frame_rate.num != 0 && config.frame_rate.den != 0 ? config.frame_rate : codec_context_->framerate;
	// 编解码标签
	codec_context_->codec_tag =
			config.codec_tag != 0 ? config.codec_tag : codec_context_->codec_tag;
	// 编解码标志
	codec_context_->flags |=
			config.flags != 0 ? config.flags : codec_context_->flags;

	//* 视频参数
	// 宽
	codec_context_->width =
			config.width >= 0 ? config.width : codec_context_->width;
	// 高
	codec_context_->height =
			config.height >= 0 ? config.height : codec_context_->height;
	// 关键帧间隔
	codec_context_->gop_size =
			config.gop_size >= 0 ? config.gop_size : codec_context_->gop_size;
	// 最大B帧数
	codec_context_->max_b_frames =
			config.max_b_frame >= 0 ? config.max_b_frame : codec_context_->max_b_frames;
	// 像素格式
	codec_context_->pix_fmt =
			config.pix_fmt != AV_PIX_FMT_NONE ? config.pix_fmt : codec_context_->pix_fmt;

	int rs;
	// 打开解码器
	rs = avcodec_open2(codec_context_, codec_context_->codec, dict.ffDictionaryPtr());
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

int enCodecContext::open(audioEnCodecConfig config, dictionary dict)
{
	if (!codec_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	//* 通用参数
	// 编解码类型
	codec_context_->codec_id =
			config.codec_id != AV_CODEC_ID_NONE ? config.codec_id : codec_context_->codec_id;
	// 比特率
	codec_context_->bit_rate =
			config.bit_rate >= 0 ? config.bit_rate : codec_context_->bit_rate;
	// 时间基
	codec_context_->time_base =
			config.time_base.num != 0 && config.time_base.den != 0 ? config.time_base : codec_context_->time_base;
	// 帧率
	codec_context_->framerate =
			config.frame_rate.num != 0 && config.frame_rate.den != 0 ? config.frame_rate : codec_context_->framerate;
	// 编解码标签
	codec_context_->codec_tag =
			config.codec_tag != 0 ? config.codec_tag : codec_context_->codec_tag;
	// 编解码标志
	codec_context_->flags |=
			config.flags != 0 ? config.flags : codec_context_->flags;

	//* 音频参数
	// 采样率
	codec_context_->sample_rate =
			config.sample_rate >= 0 ? config.sample_rate : codec_context_->sample_rate;
	// 声道数
	if (config.channels >= 1)
		av_channel_layout_default(&codec_context_->ch_layout, config.channels);
	// 采样格式
	codec_context_->sample_fmt =
			config.sample_format != AV_SAMPLE_FMT_NONE ? config.sample_format : codec_context_->sample_fmt;

	int rs;
	// 打开解码器
	rs = avcodec_open2(codec_context_, codec_context_->codec, dict.ffDictionaryPtr());
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

int enCodecContext::send(const frame &inFrame)
{
	if (!codec_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	int rs;
	// 发送 frame
	rs = avcodec_send_frame(codec_context_, inFrame.ffFrame());
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

int enCodecContext::receive(const packet &outPacket)
{
	if (!codec_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	int rs;
	// 接收 packet
	rs = avcodec_receive_packet(codec_context_, outPacket.ffPacket());
	if (rs < 0)
		if (rs != AVERROR(EAGAIN) && rs != AVERROR_EOF)
			printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

void enCodecContext::close()
{
	if (codec_context_)
		avcodec_free_context(&codec_context_);
}

const AVCodec *enCodecContext::ffCodec() const
{
	return codec_context_ ? codec_context_->codec : nullptr;
}

AVCodecContext *enCodecContext::ffCodecContext() const
{
	return codec_context_;
}

int enCodecContext::copyParametersFrom(const stream &st)
{
	int rs;
	rs = avcodec_parameters_to_context(codec_context_, const_cast<stream &>(st).ffCodecPara());
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

AVRational enCodecContext::timebase()
{
	return codec_context_ ? codec_context_->time_base : AVRational{0, 1};
}
