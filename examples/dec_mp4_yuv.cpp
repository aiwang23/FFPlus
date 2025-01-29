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
		printf("usage: %s <video input file> <yuv output file>\n", argv[0]);
		return -1;
	}

	printErrMsg(__FUNCTION__, __LINE__, 0);

	// 打开输入文件
	formatContext ifmtctx(argv[1]);

	// 打印输入文件信息
	ifmtctx.dump();

	// 创建视频解码器
	deCodecContext vde_ctx(ifmtctx.videoStream());

	FILE *fp = fopen(argv[2], "wb");
	if (fp == nullptr)
	{
		printErrMsg(__FUNCTION__, __LINE__, -1);
	}

	packet pkt;
	frame ivframe;

	// 读取
	while (0 == ifmtctx.read(pkt))
	{
		// 视频帧
		if (pkt.streamIndex() == ifmtctx.videoStream().index())
		{
			vde_ctx.send(pkt);
			if (0 == vde_ctx.receive(ivframe))
			{
				fwrite(ivframe.data(0), 1,
					   ivframe.height() * ivframe.width(), fp);
				fwrite(ivframe.data(1), 1,
					   ivframe.height() * ivframe.width() / 4, fp);
				fwrite(ivframe.data(2), 1,
					   ivframe.height() * ivframe.width() / 4, fp);
			}
		}

		// 清空packet
		pkt.unref();
	}

	fclose(fp);

	printf("video width: %d\n", ifmtctx.videoStream().width());
	printf("video height: %d\n", ifmtctx.videoStream().height());
	printf("video format: %s\n", pixFormat2Str(AV_PIX_FMT_YUV420P));
	printf("test cmd: %s\n",
		   "ffplay -f rawvideo -video_size 640x480 -pixel_format yuv420p -i 640x480_yuv420p.yuv");

	return 0;
}
