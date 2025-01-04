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

	// �������ļ�
	formatContext ifmtctx(argv[1]);

	// ��ӡ�����ļ���Ϣ
	ifmtctx.dump();

	// ����������Ƶ������
	deCodecContext ade_ctx(ifmtctx.audioStream());

	// ������Ƶ������
	frameConverter aconv(audioConvertConfig{
		ifmtctx.audioStream().channels(),
		static_cast<AVSampleFormat>(ifmtctx.audioStream().format()),
		ifmtctx.audioStream().sampleRate(),
		ifmtctx.audioStream().channels(),
		AV_SAMPLE_FMT_S16,
		ifmtctx.audioStream().sampleRate()
	});

	// ��������ļ�
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

	// ��ȡ
	while (0 == ifmtctx.read(pkt))
	{
		// ��Ƶ֡
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

		// ���packet
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
