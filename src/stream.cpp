//
// Created by 10484 on 25-1-1.
//

#include "stream.h"
#include "enCodecContext.h"
#include "deCodecContext.h"

stream::stream()
= default;

stream::stream(AVStream *st)
{
	if (st)
		stream_ = st;
}

stream::~stream()
= default;

int stream::copyParametersFrom(const enCodecContext &codec_context)
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	if (nullptr == codec_context.ffCodec())
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	// 复制参数
	int rs = avcodec_parameters_from_context(stream_->codecpar, codec_context.ffCodecContext());
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

int stream::copyParametersFrom(const stream &st)
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	// 复制参数
	int rs = avcodec_parameters_copy(stream_->codecpar, st.ffStream()->codecpar);
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

int stream::copyParametersTo(const stream &st)
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	// 复制参数
	int rs = avcodec_parameters_copy(st.ffStream()->codecpar, stream_->codecpar);
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

int stream::copyParametersTo(const deCodecContext &codec_context)
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	if (nullptr == codec_context.ffCodec())
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	// 复制参数
	int rs = avcodec_parameters_to_context(codec_context.ffCodecContext(), stream_->codecpar);
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

int stream::copyParametersTo(const enCodecContext &codec_context)
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}
	if (nullptr == codec_context.ffCodec())
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}
	// 复制参数
	int rs = avcodec_parameters_to_context(codec_context.ffCodecContext(), stream_->codecpar);
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

AVStream *stream::ffStream() const
{
	return stream_;
}

AVCodecID stream::ffCodecID()
{
	return stream_->codecpar->codec_id;
}

AVRational stream::timebase()
{
	return stream_->time_base;
}

int stream::index()
{
	return stream_->index;
}

const AVCodec *stream::ffDeCodec()
{
	return findffDeCodec(ffCodecID());
}

const AVCodec *stream::ffEnCodec()
{
	return findffEnCodec(ffCodecID());
}

int stream::width() const
{
	return stream_->codecpar->width;
}

int stream::height() const
{
	return stream_->codecpar->height;
}

int stream::format() const
{
	return stream_->codecpar->format;
}

int stream::sampleRate() const
{
	return stream_->codecpar->sample_rate;
}

int stream::channels() const
{
	return stream_->codecpar->ch_layout.nb_channels;
}

int stream::samples() const
{
	return stream_->codecpar->frame_size;
}

const AVCodecParameters *stream::ffCodecPara()
{
	return stream_->codecpar;
}
