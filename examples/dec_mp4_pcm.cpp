//
// Created by 10484 on 25-1-3.
//

#include "deCodecContext.h"
#include "formatContext.h"
#include "packet.h"
#include "utils.h"
#include "frame.h"

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

	// 创建音频解码器
	deCodecContext ade_ctx(ifmtctx.audioStream());

	// 创建音频输出文件
	FILE *fp = fopen(argv[2], "wb");
	if (fp == nullptr)
	{
		printErrMsg(__FUNCTION__, __LINE__, -1);
	}

	packet pkt;
	frame iaframe;

	// 一个采样由多少个byte
	int numBytes = av_get_bytes_per_sample(ade_ctx.sampleFormat());

	// 读取
	while (0 == ifmtctx.read(pkt))
	{
		// 音频帧
		if (pkt.streamIndex() == ifmtctx.audioStream().index())
		{
			ade_ctx.send(pkt);
			if (0 == ade_ctx.receive(iaframe))
			{
				for (int i = 0; i < iaframe.nb_samples(); ++i)
				{
					for (int ch = 0; ch < ifmtctx.audioStream().channels(); ++ch)
					{
						fwrite(iaframe.data(ch) + numBytes * i, 1, numBytes, fp);
					}
				}
			}
		}

		// 清空packet
		pkt.unref();
	}

	fclose(fp);

	printf("audio sample rate: %d\n", ifmtctx.audioStream().sampleRate());
	printf("audio channels: %d\n", ifmtctx.audioStream().channels());
	printf("audio format: %s\n", sampleFormat2Str(AV_SAMPLE_FMT_FLTP));
	printf("test cmd: ffplay -ar 48000 -ac 2 -f f32le %d_%d_%s.pcm\n",
	       ifmtctx.audioStream().sampleRate(),
	       ifmtctx.audioStream().channels(),
	       sampleFormat2Str(AV_SAMPLE_FMT_FLTP)
	);

	return 0;
}
