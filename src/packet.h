//
// Created by 10484 on 25-1-1.
//

#ifndef PACKET_H
#define PACKET_H

extern "C" {
#include <libavcodec/packet.h>
}

/**
 * 编码后的帧
 */
class packet
{
public:
	packet();

	~packet();

	packet(const packet &pkt);

	packet& operator=(const packet &pkt);

	/**
	 *
	 * @return packet 的 AVPacket*
	 */
	AVPacket *ffPacket() const;

	/**
	 * 重置packet
	 */
	void unref();

	/**
	 * 释放packet
	 */
	void free();

	/**
	 * 把 packet的pts和dts和duration的时间基单位 从 src_tb 转换为 dst_tb
	 * @param src_tb 源时间基
	 * @param dst_tb 目标时间基
	 */
	void rescaleTimebase(AVRational src_tb, AVRational dst_tb);

	/**
	 * 设置packet的流索引
	 * @param idx 流索引
	 */
	void setStreamIndex(int idx);

	/**
	 * 获取packet的流索引
	 * @return packet的流索引
	 */
	int streamIndex();

private:
	AVPacket *packet_ = nullptr;
	mutable bool is_moved_ = false;
};


#endif //PACKET_H
