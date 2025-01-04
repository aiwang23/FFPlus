//
// Created by 10484 on 25-1-1.
//

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

extern "C" {
#include <libavutil/dict.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

/**
 * 设备信息
 */
struct deviceInfo
{
public:
	char vedioname[128] = {'\0'}; // 设备名称
	char vediodesc[256] = {'\0'}; //
};

/**
 * 错误码转错误信息
 * @param rsCode 错误码
 * @return 错误信息
 */
std::string reCode2Msg(int rsCode);

/**
 * 打印错误信息
 * @param funName	函数名称
 * @param line		行号
 * @param rsCode	错误码
 */
void printErrMsg(const char *funName, int line, int rsCode);

/**
 * 查找可用的音频设备
 * @return
 */
std::vector<deviceInfo> findAudioDevices();

/**
 * 查找可用的视频设备
 * @return
 */
std::vector<deviceInfo> findVideoDevices();

/**
 * 查找可用的解码器
 * @param codec_id 编码器ID
 * @return 编码器类型
 */
const AVCodec *findffDeCodec(AVCodecID codec_id);

/**
 * 查找可用的解码器
 * @param codec_name 解码器名称
 * @return
 */
const AVCodec *findffDeCodec(const char *codec_name);

/**
 * 查找可用的编码器
 * @param codec_id
 * @return
 */
const AVCodec *findffEnCodec(AVCodecID codec_id);

/**
 * 查找可用的编码器
 * @param codec_name
 * @return
 */
const AVCodec *findffEnCodec(const char *codec_name);

const AVInputFormat *findffInputFormat(const char *format_name);

const AVOutputFormat *findffOutputFormat(const char *format_name);


const char *sampleFormat2Str(AVSampleFormat sample_fmt);

const char *pixFormat2Str(AVPixelFormat pix_fmt);

/**
 * 字典类
 */
class dictionary
{
public:
	dictionary();

	~dictionary();

	/**
	 * 设置字典项
	 * @param key 键
	 * @param value 值
	 * @param flags
	 * @return 错误码，可使用 reCode2Msg 获取错误信息
	 */
	int setOpt(const char *key, const char *value, int flags = 0);

	const char *getOpt(const char *key);

	int count();

	/**
	 * 释放
	 */
	void free();

	/**
	 * 获取字典指针
	 * @return
	 */
	AVDictionary **ffDictionaryPtr();

private:
	AVDictionary *dictionary_ = nullptr;
};


#endif //UTILS_H
