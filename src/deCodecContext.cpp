//
// Created by 10484 on 25-1-1.
//

#include "deCodecContext.h"

#include "stream.h"

#include "frame.h"
#include "packet.h"

deCodecContext::deCodecContext()
= default;

deCodecContext::deCodecContext(const deCodecContext &ctx)
{
	if (!ctx.codec_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return;
	}

	close();
	codec_context_ = ctx.codec_context_;
	ctx.is_moved_ = true;
}

deCodecContext::deCodecContext(const stream &st)
{
	int rs;
	// 分配解码器
	rs = alloc(const_cast<stream &>(st).ffDeCodec());
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	// 复制参数
	rs = copyParametersFrom(st);
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	// 打开解码器
	rs = open();
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);
}

deCodecContext & deCodecContext::operator=(const deCodecContext &ctx)
{
	if (!ctx.codec_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return *this;
	}

	if (this != &ctx)
	{
		close();
		codec_context_ = ctx.codec_context_;
		ctx.is_moved_ = true;
	}

	return *this;
}


deCodecContext::~deCodecContext()
{
	close();
}

int deCodecContext::alloc(const AVCodec *codec)
{
	if (!codec)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	// 初始化解码器
	codec_context_ = avcodec_alloc_context3(codec);
	if (!codec_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	return 0;
}

int deCodecContext::alloc(AVCodecID codec_id)
{
	return alloc(avcodec_find_decoder(codec_id));
}


int deCodecContext::open(dictionary dict)
{
	if (!codec_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	int rs;
	// 打开解码器
	rs = avcodec_open2(codec_context_, codec_context_->codec, dict.ffDictionaryPtr());
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

int deCodecContext::send(const packet &inPacket)
{
	if (!codec_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	int rs;
	rs = avcodec_send_packet(codec_context_, inPacket.ffPacket());
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

int deCodecContext::receive(const frame &outFrame)
{
	if (!codec_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	int rs;
	rs = avcodec_receive_frame(codec_context_, outFrame.ffFrame());
	if (rs < 0)
		if (rs != AVERROR(EAGAIN) && rs != AVERROR_EOF)
			printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

void deCodecContext::close()
{
	if (is_moved_)
		return;
	if (codec_context_)
		avcodec_free_context(&codec_context_);
}

const AVCodec *deCodecContext::ffCodec() const
{
	return codec_context_ ? codec_context_->codec : nullptr;
}

AVCodecContext *deCodecContext::ffCodecContext() const
{
	return codec_context_;
}

int deCodecContext::copyParametersFrom(const stream &st)
{
	int rs;
	rs = avcodec_parameters_to_context(codec_context_, const_cast<stream &>(st).ffCodecPara());
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

AVSampleFormat deCodecContext::sampleFormat() const
{
	return codec_context_ ? codec_context_->sample_fmt : AV_SAMPLE_FMT_NONE;
}
