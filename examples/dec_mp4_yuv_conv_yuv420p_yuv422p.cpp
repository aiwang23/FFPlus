//
// Created by 10484 on 25-1-3.
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
		printf("usage: %s <video input file> <yuv output file>\n", argv[0]);
		return -1;
	}

	printErrMsg(__FUNCTION__, __LINE__, 0);

	// �������ļ�
	formatContext ifmtctx(argv[1]);

	// ��ӡ�����ļ���Ϣ
	ifmtctx.dump();

	// ����������Ƶ������
	deCodecContext vde_ctx(ifmtctx.videoStream());

	frameConverter vconv(videoConvertConfig{
		ifmtctx.videoStream().width(),
		ifmtctx.videoStream().height(),
		(AVPixelFormat) ifmtctx.videoStream().format(),
		ifmtctx.videoStream().width(),
		ifmtctx.videoStream().height(),
		AV_PIX_FMT_YUV422P,
	});

	// ��������ļ�
	FILE *fp = fopen(argv[2], "wb");
	if (fp == nullptr)
	{
		printErrMsg(__FUNCTION__, __LINE__, -1);
	}

	packet pkt;
	frame ivframe;
	frame ovframe(ifmtctx.videoStream().width(),
	              ifmtctx.videoStream().height(),
	              AV_PIX_FMT_YUV422P
	);

	// ��ȡ
	while (0 == ifmtctx.read(pkt))
	{
		// ��Ƶ֡
		if (pkt.streamIndex() == ifmtctx.videoStream().index())
		{
			vde_ctx.send(pkt);
			if (0 == vde_ctx.receive(ivframe))
			{
				vconv.convert(ivframe, ovframe);

				fwrite(ovframe.data(0), 1, ovframe.width() * ovframe.height(), fp);
				fwrite(ovframe.data(1), 1,
				       ovframe.height() * ovframe.width() / 2, fp);
				fwrite(ovframe.data(2), 1,
				       ovframe.height() * ovframe.width() / 2, fp);
			}
		}

		// ���packet
		pkt.unref();
	}

	fclose(fp);

	printf("video width: %d\n", ifmtctx.videoStream().width());
	printf("video height: %d\n", ifmtctx.videoStream().height());
	printf("video format: %s\n", pixFormat2Str(AV_PIX_FMT_YUV422P));
	printf("test cmd: %s\n",
	       "ffplay -f rawvideo -video_size 640x480 -pixel_format yuv422p -i ./out_640x480_yuv422p.yuv");

	return 0;
}
