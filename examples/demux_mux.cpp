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

	// �������ļ�
	formatContext ifmtctx(argv[1]);
	// ��ӡ�����ļ���Ϣ
	ifmtctx.dump();

	// ��������ļ�
	formatContext ofmtctx;
	ofmtctx.allocOutput(argv[2]);
	// ������Ƶ��
	stream ovi_st = ofmtctx.copyStream(ifmtctx.videoStream());
	// ������Ƶ��
	stream oau_st = ofmtctx.copyStream(ifmtctx.audioStream());
	// ������ļ�
	ofmtctx.openOutput();
	// ��ӡ����ļ�
	ofmtctx.dump();

	packet pkt;

	// ��ȡ
	while (0 == ifmtctx.read(pkt))
	{
		// ��Ƶ֡
		if (pkt.streamIndex() == ifmtctx.videoStream().index())
		{
			// ʱ���ת��
			pkt.rescaleTimebase(ifmtctx.videoStream().timebase(), ovi_st.timebase());
			// д��
			ofmtctx.write(pkt);
		}
		// ��Ƶ֡
		else if (pkt.streamIndex() == ifmtctx.audioStream().index())
		{
			// ʱ���ת��
			pkt.rescaleTimebase(ifmtctx.audioStream().timebase(), oau_st.timebase());
			// д��
			ofmtctx.write(pkt);
		}

		// ���packet
		pkt.unref();
	}

	printf("test cmd: %s\n", "ffplay out.mp4");

	return 0;
}
