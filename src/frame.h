//
// Created by 10484 on 25-1-1.
//

#ifndef FRAME_H
#define FRAME_H

extern "C" {
#include <libavutil/frame.h>
}

/**
 * 未编码的帧
 */
class frame
{
public:
	frame();

	frame(const frame &frm);

	/**
	 * 初始化视频帧并分配缓冲区
	 * @param width 宽
	 * @param height 高
	 * @param pix_fmt 像素格式
	 */
	frame(int width, int height, AVPixelFormat pix_fmt);

	/**
	 * 初始化音频帧并分配缓冲区
	 * @param nb_samples 样本数
	 * @param channels 声道数
	 * @param sample_fmt 采样格式
	 */
	frame(int nb_samples, int channels, AVSampleFormat sample_fmt);

	frame &operator=(const frame &frm);

	~frame();

	/**
	 * 分配视频帧 frame 的缓冲区 AVFrame->data
	 * @param width 宽
	 * @param height 高
	 * @param pix_fmt 像素格式
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int newBuffer(int width, int height, AVPixelFormat pix_fmt);

	/**
	 * 分配音频帧 frame 的缓冲区 AVFrame->data
	 * @param nb_samples 样本数
	 * @param channels 声道数
	 * @param sample_fmt 采样格式
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int newBuffer(int nb_samples, int channels, AVSampleFormat sample_fmt);

	/**
	 *
	 * @return frame的 AVFrame*
	 */
	AVFrame *ffFrame() const;

	/**
	 * 释放frame
	 */
	void free();

	/**
	 * 重置frame
	 */
	void unref();

	uint8_t **data() const;

	uint8_t *data(int index)const ;

	int *linesize() const;

	int linesize(int index) const;

	int width() const;

	int height() const;

	int nb_samples() const;

	void setPts(int pts);

	int channels() const;

	int format()const;

	int bufferSize();

private:
	AVFrame *frame_ = nullptr;
	mutable bool is_moved_ = false;
};


#endif //FRAME_H
