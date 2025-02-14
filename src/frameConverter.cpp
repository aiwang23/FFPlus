//
// Created by 10484 on 25-1-3.
//

#include "frameConverter.h"

#include "utils.h"
#include "frame.h"

frameConverter::frameConverter()
= default;

frameConverter::frameConverter(const frameConverter &cnt)
{
	if (!cnt.swr_context_ && !cnt.sws_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR(EINVAL));
		return;
	}
	free();
	if (cnt.type_ == convertType::VIDEO)
		sws_context_ = cnt.sws_context_;
	else if (cnt.type_ == convertType::AUDIO)
		swr_context_ = cnt.swr_context_;
	type_ = cnt.type_;
	cnt.is_moved_ = true;
}

frameConverter::frameConverter(videoConvertConfig config)
{
	alloc(config);
}

frameConverter::frameConverter(audioConvertConfig config)
{
	alloc(config);
}

frameConverter & frameConverter::operator=(const frameConverter &cnt)
{
	if (!cnt.swr_context_ && !cnt.sws_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR(EINVAL));
		return *this;
	}

	if (this != &cnt)
	{
		free();
		if (cnt.type_ == convertType::VIDEO)
			sws_context_ = cnt.sws_context_;
		else if (cnt.type_ == convertType::AUDIO)
			swr_context_ = cnt.swr_context_;
		type_ = cnt.type_;
		cnt.is_moved_ = true;
	}

	return *this;
}

frameConverter::~frameConverter()
{
	free();
}

int frameConverter::alloc(videoConvertConfig config)
{
	type_ = convertType::VIDEO;

	if (config.src_w <= 0 || config.src_h <= 0 || config.src_pix_fmt == AV_PIX_FMT_NONE)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR(EINVAL));
		return AVERROR(EINVAL);
	}

	if (config.dst_w <= 0 || config.dst_h <= 0 || config.dst_pix_fmt == AV_PIX_FMT_NONE)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR(EINVAL));
		return AVERROR(EINVAL);
	}

	sws_context_ = sws_getContext(config.src_w, config.src_h, config.src_pix_fmt,
	                              config.dst_w, config.dst_h, config.dst_pix_fmt,
	                              config.flags, config.src_filter, config.dst_filter,
	                              &config.param);
	if (!sws_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR(EINVAL));
		return AVERROR(EINVAL);
	}

	return 0;
}

int frameConverter::alloc(audioConvertConfig config)
{
	type_ = convertType::AUDIO;

	if (config.src_channels <= 0 || config.src_sample_rate <= 0 || config.src_sample_fmt == AV_SAMPLE_FMT_NONE)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR(EINVAL));
		return AVERROR(EINVAL);
	}

	if (config.dst_channels <= 0 || config.dst_sample_rate <= 0 || config.dst_sample_fmt == AV_SAMPLE_FMT_NONE)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR(EINVAL));
		return AVERROR(EINVAL);
	}

	AVChannelLayout src_channel_layout;
	av_channel_layout_default(&src_channel_layout, config.src_channels);
	AVChannelLayout dst_channel_layout;
	av_channel_layout_default(&dst_channel_layout, config.dst_channels);

	int rs;
	rs = swr_alloc_set_opts2(&swr_context_,
	                         &dst_channel_layout, config.dst_sample_fmt, config.dst_sample_rate,
	                         &src_channel_layout, config.src_sample_fmt, config.src_sample_rate,
	                         config.log_level_offset, config.log_ctx
	);
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	rs = swr_init(swr_context_);
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

int frameConverter::convert(const frame &inFrame, const frame &outFrame)
{
	int rs;

	if (type_ == convertType::VIDEO)
	{
		rs = sws_scale(sws_context_,
		               inFrame.data(),
		               inFrame.linesize(),
		               0, inFrame.height(),
		               outFrame.data(),
		               outFrame.linesize()
		);
		if (rs < 0)
			printErrMsg(__FUNCTION__, __LINE__, rs);
	}
	else if (type_ == convertType::AUDIO)
	{
		rs = swr_convert(swr_context_,
		                 outFrame.data(),
		                 outFrame.nb_samples(),
		                 const_cast<const uint8_t **>(inFrame.data()),
		                 inFrame.nb_samples()
		);
		if (rs < 0)
			printErrMsg(__FUNCTION__, __LINE__, rs);
	}

	return rs;
}

void frameConverter::free()
{
	if (is_moved_)
		return;

	if (sws_context_)
		sws_freeContext(sws_context_);

	if (swr_context_)
		swr_free(&swr_context_);
}

AVPixelFormat frameConverter::getPixFmt(const char* name)
{
	return av_get_pix_fmt(name);
}

AVSampleFormat frameConverter::getSampleFmt(const char* name)
{
    return av_get_sample_fmt(name);
}

bool frameConverter::isAlloced()
{
	return sws_context_ || swr_context_;
}
