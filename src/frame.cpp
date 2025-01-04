//
// Created by 10484 on 25-1-1.
//

#include "frame.h"

#include "utils.h"

frame::frame()
{
	frame_ = av_frame_alloc();
}

frame::frame(int width, int height, AVPixelFormat pix_fmt) : frame()
{
	newBuffer(width, height, pix_fmt);
}

frame::frame(int nb_samples, int channels, AVSampleFormat sample_fmt) : frame()
{
	newBuffer(nb_samples, channels, sample_fmt);
}

frame::~frame()
{
	free();
}

int frame::newBuffer(int width, int height, AVPixelFormat pix_fmt)
{
	int rs;

	if (!frame_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR(ENOMEM));
		return AVERROR(ENOMEM);
	}

	if (width <= 0 || height <= 0 || pix_fmt == AV_PIX_FMT_NONE)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR(EINVAL));
		return AVERROR(EINVAL);
	}

	frame_->width = width;
	frame_->height = height;
	frame_->format = pix_fmt;

	rs = av_frame_get_buffer(frame_, 0);
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

int frame::newBuffer(int nb_samples, int channels, AVSampleFormat sample_fmt)
{
	int rs;

	if (!frame_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR(ENOMEM));
		return AVERROR(ENOMEM);
	}

	if (nb_samples <= 0 || channels <= 0 || sample_fmt == AV_SAMPLE_FMT_NONE)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR(EINVAL));
		return AVERROR(EINVAL);
	}

	frame_->nb_samples = nb_samples;
	av_channel_layout_default(&frame_->ch_layout, channels);
	frame_->format = sample_fmt;

	rs = av_frame_get_buffer(frame_, 0);
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

AVFrame *frame::ffFrame() const
{
	return frame_;
}

void frame::free()
{
	if (frame_)
		av_frame_free(&frame_);
}

void frame::unref()
{
	av_frame_unref(frame_);
}

uint8_t **frame::data() const
{
	return frame_->data;
}

uint8_t *frame::data(int index) const
{
	return frame_->data[index];
}

int *frame::linesize() const
{
	return frame_->linesize;
}

int frame::linesize(int index) const
{
	return frame_->linesize[index];
}

int frame::width() const
{
	return frame_->width;
}

int frame::height() const
{
	return frame_->height;
}

int frame::nb_samples() const
{
	return frame_->nb_samples;
}

void frame::setPts(int pts)
{
	frame_->pts = pts;
}

int frame::channels() const
{
	return frame_->ch_layout.nb_channels;
}

int frame::format() const
{
	return frame_->format;
}

int frame::bufferSize()
{
	int dst_bufsize = av_samples_get_buffer_size(frame_->linesize,
	                                             frame_->ch_layout.nb_channels,
	                                             frame_->nb_samples,
	                                             static_cast<AVSampleFormat>(frame_->format),
	                                             1);

	return dst_bufsize;
}
