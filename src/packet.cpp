//
// Created by 10484 on 25-1-1.
//

#include "packet.h"

#include "utils.h"

packet::packet()
{
	packet_ = av_packet_alloc();
}

packet::~packet()
{
	this->free();
}

packet::packet(const packet &pkt)
{
	if (!pkt.packet_)
	{
		printErrMsg(__FUNCTION__, __LINE__, AVERROR(EINVAL));
		return;
	}
	free();
	packet_ = pkt.packet_;
	pkt.is_moved_ = true;
}

AVPacket *packet::ffPacket() const
{
	return packet_;
}

void packet::unref()
{
	if (packet_)
		av_packet_unref(packet_);
}

void packet::free()
{
	if (is_moved_)
		return;

	if (packet_)
		av_packet_free(&packet_);
}

void packet::rescaleTimebase(AVRational src_tb, AVRational dst_tb)
{
	av_packet_rescale_ts(packet_, src_tb, dst_tb);
}

void packet::setStreamIndex(int idx)
{
	packet_->stream_index = idx;
}

int packet::streamIndex()
{
	return packet_->stream_index;
}
