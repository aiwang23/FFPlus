#include "formatContext.h"
#include "packet.h"
#include "utils.h"
#include <cstdio>


int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("usage: %s <video input file> <video output file>\n", argv[0]);
		return -1;
	}

	printErrMsg(__FUNCTION__, __LINE__, 0);

	// 打开输入文件
	formatContext ifmtctx(argv[1]);
	// 打印输入文件信息
	ifmtctx.dump();

	// 创建输出文件
	formatContext ofmtctx;
	ofmtctx.allocOutput(argv[2]);
	// 新增视频流
	stream ovi_st = ofmtctx.copyStream(ifmtctx.videoStream());
	// 新增音频流
	stream oau_st = ofmtctx.copyStream(ifmtctx.audioStream());
	// 打开输出文件
	ofmtctx.openOutput();
	// 打印输出文件
	ofmtctx.dump();

	packet pkt;

	// 读取
	while (0 == ifmtctx.read(pkt))
	{
		// 视频帧
		if (pkt.streamIndex() == ifmtctx.videoStream().index())
		{
			// 时间基转换
			pkt.rescaleTimebase(ifmtctx.videoStream().timebase(), ovi_st.timebase());
			// 写入
			ofmtctx.write(pkt);
		}
		// 音频帧
		else if (pkt.streamIndex() == ifmtctx.audioStream().index())
		{
			// 时间基转换
			pkt.rescaleTimebase(ifmtctx.audioStream().timebase(), oau_st.timebase());
			// 写入
			ofmtctx.write(pkt);
		}

		// 清空packet
		pkt.unref();
	}

	printf("test cmd: %s\n", "ffplay out.mp4");

	return 0;
}
