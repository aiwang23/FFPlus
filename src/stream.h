//
// Created by 10484 on 25-1-1.
//

#ifndef STREAM_H
#define STREAM_H

#include <string>

extern "C" {
#include <libavformat/avformat.h>
}


class deCodecContext;
class enCodecContext;

/**
 * ����
 */
class stream
{
public:
	stream();

	explicit stream(AVStream *st);

	stream& operator=(const stream &st);

	~stream();

	/**
	 * �ӱ�����codec_context ���Ʋ�����������
	 * @param codec_context ������
	 * @return �����룬��ʹ�� reCode2Msg ��ȡ������Ϣ
	 */
	int copyParametersFrom(const enCodecContext &codec_context);

	/**
	 * ����һ������st ���Ʋ�����������
	 * @param st ��һ������
	 * @return �����룬��ʹ�� reCode2Msg ��ȡ������Ϣ
	 */
	int copyParametersFrom(const stream &st);

	int copyParametersTo(const stream &st);

	/**
	 * ���������������Ƶ�deCodecContext
	 * @param codec_context
	 * @return
	 */
	int copyParametersTo(const deCodecContext &codec_context);

	int copyParametersTo(const enCodecContext &codec_context);

	/**
	 * ��ȡ�����������AVStream*
	 * @return
	 */
	AVStream *ffStream() const;

	/**
	 * ��ȡ����������ı���������AVCodecID
	 * @return
	 */
	AVCodecID ffCodecID();

	const AVCodec *ffDeCodec();

	const AVCodec *ffEnCodec();

	const AVCodecParameters *ffCodecPara();

	/**
	 * ��ȡʱ���
	 * @return
	 */
	AVRational timebase();

	/**
	 * ��ȡ�������ļ�formatContext�е�����
	 * @return
	 */
	int index();

	/**
	 * @brief ��ȡ����������
	 *
	 * @return std::string
	 */
	std::string codecName();

	/**
	 * @brief ��ȡ������������Ϣ
	 *
	 * @return std::string
	 */
	std::string profile();

	/**
	 * @brief ��ȡ��������
	 *
	 * @return int
	 */
	int bitrate() const;

	/**
	 * ��Ƶ��ʱ��Ϊ���ظ�ʽ AVPixelFormat
	 * ��Ƶ��ʱ��Ϊ������ʽ AVSampleFormat
	 * @return
	 */
	int format() const;

	/**
	 * @brief ��ȡ��ʽ����
	 *
	 * @return std::string
	 */
	std::string format_name();

	/**
	 * Ϊ��Ƶ��ʱ����ȡ���
	 * @return
	 */
	int width() const;

	/**
	 * Ϊ��Ƶ��ʱ����ȡ�߶�
	 * @return
	 */
	int height() const;

	/**
	 * @brief ��ȡ֡��
	 *
	 * @return double
	 */
	double fps();

	/**
	 * @brief ��ȡ������
	 *
	 * @return int
	 */
	int sampleRate() const;

	/**
	 * @brief ��ȡ������
	 *
	 * @return int
	 */
	int channels() const;

	/**
	 * @brief ��ȡ��������
	 *
	 * @return int
	 */
	int samples() const;

	/**
	 * �жϴ������Ƿ�Ϊ��
	 * @return
	 */
	bool empty();

	/**
	 * �жϴ������Ƿ�Ϊ��
	 * @return
	 */
	bool operator!();

private:
	AVStream *stream_ = nullptr;
};


#endif //STREAM_H
