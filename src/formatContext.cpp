//
// Created by 10484 on 25-1-1.
//

#include "formatContext.h"

#include "enCodecContext.h"
#include "packet.h"

formatContext::formatContext()
{
	// fmt_context_ = avformat_alloc_context();
}

formatContext::formatContext(const formatContext &fmt)
{
	if (!fmt.fmt_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return;
	}

	close();
	fmt_context_ = fmt.fmt_context_;
	open_file_t_ = fmt.open_file_t_;
	fmt.is_moved_ = true;
}

formatContext::formatContext(const char *url, const AVInputFormat *fmt, dictionary dict)
{
	int rs = openInput(url, fmt, dict);
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);
}

formatContext::~formatContext()
{
	close();
}

int formatContext::openInput(const char *url, const AVInputFormat *fmt, dictionary dict)
{
	int rs;
	open_file_t_ = fileType::INPUT;

	// 打开文件
	rs = avformat_open_input(&fmt_context_, url, fmt, dict.ffDictionaryPtr());
	if (rs < 0)
	{
		printErrMsg(__FUNCTION__, __LINE__, rs);
		return rs;
	}

	// 寻找可用流
	rs = avformat_find_stream_info(fmt_context_, nullptr);
	if (rs < 0)
	{
		printErrMsg(__FUNCTION__, __LINE__, rs);
		close();
		return rs;
	}

	return 0;
}

int formatContext::allocOutput(const char *url, const AVOutputFormat *ofmt)
{
	int rs;
	open_file_t_ = fileType::OUTPUT;

	rs = avformat_alloc_output_context2(&fmt_context_, ofmt, nullptr, url);
	if (rs < 0)
	{
		printErrMsg(__FUNCTION__, __LINE__, rs);
		close();
		return rs;
	}
	return rs;
}

stream formatContext::newStream(const AVCodec *codec)
{
	if (!fmt_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return {};
	}
	AVStream *st = avformat_new_stream(fmt_context_, codec);
	if (!st)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		close();
		return {};
	}
	return stream{st};
}

stream formatContext::copyStream(const stream &st)
{
	if (!fmt_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return {};
	}

	AVStream *newSt = avformat_new_stream(fmt_context_, const_cast<stream &>(st).ffEnCodec());
	if (!newSt)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		close();
		return {};
	}

	if (avcodec_parameters_copy(newSt->codecpar, const_cast<stream &>(st).ffCodecPara()) < 0)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		close();
		return {};
	}

	return stream{newSt};
}

stream formatContext::copyStream(const enCodecContext &codec_context)
{
	if (!fmt_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return {};
	}

	AVStream *newSt = avformat_new_stream(fmt_context_, codec_context.ffCodec());
	if (!newSt)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		close();
		return {};
	}

	int rs = avcodec_parameters_from_context(newSt->codecpar, codec_context.ffCodecContext());
	if (rs < 0)
	{
		printErrMsg(__FUNCTION__, __LINE__, rs);
		close();
		return {};
	}

	return stream{newSt};
}

int formatContext::openOutput()
{
	int rs;

	if (!fmt_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return AVERROR_UNKNOWN;
	}

	if (!(fmt_context_->oformat->flags & AVFMT_NOFILE))
	{
		rs = avio_open2(&fmt_context_->pb, fmt_context_->url, AVIO_FLAG_WRITE,
		                nullptr, nullptr);
		if (rs < 0)
		{
			printErrMsg(__FUNCTION__, __LINE__, rs);
			close();
			return rs;
		}
	}

	rs = avformat_write_header(fmt_context_, nullptr);
	if (rs < 0)
	{
		printErrMsg(__FUNCTION__, __LINE__, rs);
		close();
		return rs;
	}

	return rs;
}

void formatContext::close()
{
	if (is_moved_)
		return;

	if (fileType::INPUT == open_file_t_)
	{
		if (fmt_context_)
			avformat_close_input(&fmt_context_);
	}
	else if (fileType::OUTPUT == open_file_t_)
	{
		if (fmt_context_)
			av_write_trailer(fmt_context_);
		if (fmt_context_)
			avio_close(fmt_context_->pb);
		if (fmt_context_)
			avformat_free_context(fmt_context_);
	}
}

stream formatContext::streamAt(int idx)
{
	if (!fmt_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return {};
	}

	if (idx < 0 || idx >= fmt_context_->nb_streams)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return {};
	}

	return stream{fmt_context_->streams[idx]};
}

stream formatContext::videoStream()
{
	// return streamAt(video_st_idx_);
	return streamAt(videoStreamIndex());
}

stream formatContext::audioStream()
{
	// return streamAt(audio_st_idx_);
	return streamAt(audioStreamIndex());
}


void formatContext::dump()
{
	if (!fmt_context_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR_UNKNOWN);
		return;
	}
	av_dump_format(fmt_context_, 0, fmt_context_->url, (open_file_t_ == fileType::OUTPUT));
}

int formatContext::read(const packet &pkt)
{
	int rs;
	rs = av_read_frame(fmt_context_, pkt.ffPacket());
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

int formatContext::write(const packet &pkt)
{
	int rs;
	rs = av_interleaved_write_frame(fmt_context_, pkt.ffPacket());
	if (rs < 0)
		printErrMsg(__FUNCTION__, __LINE__, rs);

	return rs;
}

stream formatContext::operator[](int idx)
{
	return streamAt(idx);
}

int formatContext::videoStreamIndex()
{
	return av_find_best_stream(fmt_context_, AVMEDIA_TYPE_VIDEO,
	                           -1, -1, nullptr, 0);
}

int formatContext::audioStreamIndex()
{
	return av_find_best_stream(fmt_context_, AVMEDIA_TYPE_AUDIO,
	                           -1, -1, nullptr, 0);
}

std::string formatContext::url()
{
	return std::string(fmt_context_->url);
}

std::string formatContext::format_name()
{
	return std::string{
		open_file_t_ == fileType::INPUT ? fmt_context_->iformat->name : fmt_context_->oformat->name
	};
}

int64_t formatContext::filesize() const
{
	if (!fmt_context_)
		return -1;
	if (!fmt_context_->pb)
		return -1;
	return avio_size(fmt_context_->pb);
}

double formatContext::duration() const
{
	if (!fmt_context_)
		return -1;
	return fmt_context_->duration / AV_TIME_BASE;
}

int formatContext::bitrate() const
{
	if (!fmt_context_)
		return -1;
	return fmt_context_->bit_rate;
}
