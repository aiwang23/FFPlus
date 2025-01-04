//
// Created by 10484 on 25-1-4.
//

#include "enCodecContext.h"
#include "formatContext.h"
#include "packet.h"
#include "utils.h"
#include "frame.h"

#include <cstdio>


int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("usage: %s <yuv input file> <mp4 output file>\n", argv[0]);
		return -1;
	}

	printErrMsg(__FUNCTION__, __LINE__, 0);

	// �������ļ�
	FILE *inFile = fopen(argv[1], "rb");
	if (!inFile)
	{
		printErrMsg(__FUNCTION__, __LINE__, -1);
		return -1;
	}

	// ��ʼ�������������ò������򿪱�����
	enCodecContext enCodecContext(videoEnCodecConfig{
		640,
		480,
		AV_PIX_FMT_YUV420P,
		10,
		0,
		AV_CODEC_ID_H264,
		400000,
		AVRational{1, 24},
		AVRational{1, 24},
		0,
		AV_CODEC_FLAG_GLOBAL_HEADER
	});

	// ��ʼ������ļ�
	formatContext ofmtctx;
	ofmtctx.allocOutput(argv[2]);
	// ������Ƶ�������Ʊ���������
	stream ov_st = ofmtctx.copyStream(enCodecContext);
	// ������ļ�
	ofmtctx.openOutput();

	frame inframe(640, 480, AV_PIX_FMT_YUV420P);
	packet outpacket;
	int i = 0;
	while (feof(inFile) == 0)
	{
		// ��ȡyuv420����
		fread(inframe.data(0), 1, 640 * 480, inFile);
		fread(inframe.data(1), 1, 640 * 480 / 4, inFile);
		fread(inframe.data(2), 1, 640 * 480 / 4, inFile);

		//* ����Ҫ����pts
		inframe.setPts(i++);
		// ����
		enCodecContext.send(inframe);
		while (enCodecContext.receive(outpacket) == 0)
		{
			// ����������
			outpacket.setStreamIndex(ov_st.index());
			// ��ʱ����ӱ�����ʱ���ת�������ʱ���
			outpacket.rescaleTimebase(enCodecContext.timebase(), ov_st.timebase());

			// д���ļ�
			ofmtctx.write(outpacket);
		}
	}

	printf("test cmd: %s\n", "ffplay out.mp4");

	return 0;
}
