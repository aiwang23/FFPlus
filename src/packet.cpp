//
// Created by 10484 on 25-1-1.
//

#include "packet.h"

packet::packet()
{
	packet_ = av_packet_alloc();
}

packet::~packet()
{
	this->free();
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
