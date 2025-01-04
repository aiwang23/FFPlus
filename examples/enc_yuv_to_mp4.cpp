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

	// 打开输入文件
	FILE *inFile = fopen(argv[1], "rb");
	if (!inFile)
	{
		printErrMsg(__FUNCTION__, __LINE__, -1);
		return -1;
	}

	// 初始化编码器，配置参数，打开编码器
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

	// 初始化输出文件
	formatContext ofmtctx;
	ofmtctx.allocOutput(argv[2]);
	// 新增视频流，复制编码器参数
	stream ov_st = ofmtctx.copyStream(enCodecContext);
	// 打开输出文件
	ofmtctx.openOutput();

	frame inframe(640, 480, AV_PIX_FMT_YUV420P);
	packet outpacket;
	int i = 0;
	while (feof(inFile) == 0)
	{
		// 读取yuv420数据
		fread(inframe.data(0), 1, 640 * 480, inFile);
		fread(inframe.data(1), 1, 640 * 480 / 4, inFile);
		fread(inframe.data(2), 1, 640 * 480 / 4, inFile);

		//* 编码要设置pts
		inframe.setPts(i++);
		// 编码
		enCodecContext.send(inframe);
		while (enCodecContext.receive(outpacket) == 0)
		{
			// 设置流索引
			outpacket.setStreamIndex(ov_st.index());
			// 把时间基从编码器时间基转换成输出时间基
			outpacket.rescaleTimebase(enCodecContext.timebase(), ov_st.timebase());

			// 写入文件
			ofmtctx.write(outpacket);
		}
	}

	printf("test cmd: %s\n", "ffplay out.mp4");

	return 0;
}
