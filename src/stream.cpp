//
// Created by 10484 on 25-1-1.
//

#include "stream.h"
#include "enCodecContext.h"
#include "deCodecContext.h"

extern "C" {
#include <libavutil/imgutils.h>
}

stream::stream()
= default;

stream::stream(AVStream *st)
{
	if (st)
		stream_ = st;
}

stream &stream::operator=(const stream &st)
{
	if (this != &st)
		stream_ = st.stream_;
	return *this;
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
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AV_CODEC_ID_NONE;
	}
	else if (nullptr == stream_->codecpar)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AV_CODEC_ID_NONE;
	}
	return stream_->codecpar->codec_id;
}

AVRational stream::timebase()
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return {};
	}
	return stream_->time_base;
}

int stream::index()
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
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
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	else if (nullptr == stream_->codecpar)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	return stream_->codecpar->width;
}

int stream::height() const
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	else if (nullptr == stream_->codecpar)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	return stream_->codecpar->height;
}

double stream::fps()
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	return av_q2d(stream_->avg_frame_rate);
}

std::string stream::format_name()
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return {};
	}
	else if (nullptr == stream_->codecpar)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return {};
	}

	std::string name;
	if (stream_->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		name = av_get_pix_fmt_name((AVPixelFormat) stream_->codecpar->format);
	else if (stream_->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		name = av_get_sample_fmt_name((AVSampleFormat) stream_->codecpar->format);
	return name;
}

std::string stream::codecName()
{
	return std::string{avcodec_get_name(ffCodecID())};
}

std::string stream::profile()
{
	if (nullptr == stream_)
		return {};
	if (nullptr == stream_->codecpar)
		return {};
	return std::string{av_get_profile_name(ffDeCodec(), stream_->codecpar->profile)};
}

int stream::bitrate() const
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	else if (nullptr == stream_->codecpar)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	return stream_->codecpar->bit_rate;
}

int stream::format() const
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	else if (nullptr == stream_->codecpar)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	return stream_->codecpar->format;
}

int stream::sampleRate() const
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	else if (nullptr == stream_->codecpar)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	return stream_->codecpar->sample_rate;
}

int stream::channels() const
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	else if (nullptr == stream_->codecpar)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	else if (stream_->codecpar->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	return stream_->codecpar->ch_layout.nb_channels;
}

int stream::samples() const
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	else if (nullptr == stream_->codecpar)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return -1;
	}
	return stream_->codecpar->frame_size;
}

bool stream::empty()
{
	return nullptr == stream_;
}

bool stream::operator!()
{
	return empty();
}

const AVCodecParameters *stream::ffCodecPara()
{
	if (nullptr == stream_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return nullptr;
	}
	return stream_->codecpar;
}
