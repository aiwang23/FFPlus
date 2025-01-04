//
// Created by 10484 on 25-1-4.
//

#include "deCodecContext.h"
#include "formatContext.h"
#include "packet.h"
#include "utils.h"
#include "frame.h"
#include "frameConverter.h"

#include <cstdio>


int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("usage: %s <audio input file> <pcm output file>\n", argv[0]);
		return -1;
	}

	printErrMsg(__FUNCTION__, __LINE__, 0);

	// 打开输入文件
	formatContext ifmtctx(argv[1]);

	// 打印输入文件信息
	ifmtctx.dump();

	// 创建并打开音频解码器
	deCodecContext ade_ctx(ifmtctx.audioStream());

	// 创建音频解码器
	frameConverter aconv(audioConvertConfig{
		ifmtctx.audioStream().channels(),
		static_cast<AVSampleFormat>(ifmtctx.audioStream().format()),
		ifmtctx.audioStream().sampleRate(),
		ifmtctx.audioStream().channels(),
		AV_SAMPLE_FMT_S16,
		ifmtctx.audioStream().sampleRate()
	});

	// 创建输出文件
	FILE *fp = fopen(argv[2], "wb");
	if (fp == nullptr)
	{
		printErrMsg(__FUNCTION__, __LINE__, -1);
	}

	packet pkt;
	frame iaframe;
	int nb_samples = ifmtctx.audioStream().samples();
	frame oaframe(nb_samples,
				  ifmtctx.audioStream().channels(),
				  AV_SAMPLE_FMT_S16
	);

	// 读取
	while (0 == ifmtctx.read(pkt))
	{
		// 音频帧
		if (pkt.streamIndex() == ifmtctx.audioStream().index())
		{
			ade_ctx.send(pkt);
			if (0 == ade_ctx.receive(iaframe))
			{
				aconv.convert(iaframe, oaframe);

				int dst_bufsize = oaframe.bufferSize();
				fwrite(oaframe.data(0), 1, dst_bufsize, fp);
			}
		}

		// 清空packet
		pkt.unref();
	}

	fclose(fp);

	printf("audio sample rate: %d\n", ifmtctx.audioStream().sampleRate());
	printf("audio channels: %d\n", ifmtctx.audioStream().channels());
	printf("audio format: %s\n", sampleFormat2Str(AV_SAMPLE_FMT_S16));
	printf("test cmd: %s\n",
		   "ffplay -ar 48000 -ac 2 -f s16le out_48000_2_s16le.pcm");

	return 0;
}
