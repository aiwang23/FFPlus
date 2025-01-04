//
// Created by 10484 on 25-1-5.
//

#include "formatContext.h"
#include "stream.h"
#include "packet.h"
#include "frame.h"
#include "enCodecContext.h"
#include "frameConverter.h"

#include <cstdio>


int main(int argc ,char **argv)
{
	if (argc != 3)
	{
		printf("usage: %s <pcm input file> <audio output file>\n", argv[0]);
		return -1;
	}

	// 打开音频输入文件
	FILE * ifp = fopen(argv[1], "rb");
	if (!ifp)
	{
		printErrMsg(__FUNCTION__, __LINE__, -1);
		return -1;
	}

	// 音频格式转换器
	frameConverter aconv(audioConvertConfig{
		2, AV_SAMPLE_FMT_S16, 48000,
		2, AV_SAMPLE_FMT_FLTP, 48000
	});

	//  音频编码器
	enCodecContext aenc(audioEnCodecConfig{
		48000, 2, AV_SAMPLE_FMT_FLTP,
		AV_CODEC_ID_AAC, 40000
	});

	// 初始化输出文件
	formatContext ofmt;
	ofmt.allocOutput(argv[2]);
	// 新增音频流，并拷贝编码参数
	stream oa_st = ofmt.copyStream(aenc);
	// 打开输出文件
	ofmt.openOutput();
	ofmt.dump();

	// 转换前的音频帧
	frame ifrm(
		1024, 2, AV_SAMPLE_FMT_S16
	);
	// 转换后的音频帧
	frame ofrm(
		oa_st.samples(), 2, AV_SAMPLE_FMT_FLTP
	);
	// 音频编码输出帧
	packet opkt;

	while (feof(ifp) == 0)
	{
		fread(ifrm.data(0), ifrm.bufferSize(), 1, ifp);

		aconv.convert(ifrm, ofrm);
		aenc.send(ofrm);

		if (aenc.receive(opkt) == 0)
			ofmt.write(opkt);

	}
	if (ifp)
		fclose(ifp);

	printf("test cmd: %s\n", "ffplay out.aac");

	return 0;
}
