#include <thread>
#include "Decoder.h"

extern "C"
{
#include "libavformat/avformat.h"
};

int main(int argc, char* argv[])
{
	//打开输出文件
	FILE* YUVFile = fopen("out.yuv", "wb+");
	if (YUVFile == nullptr)
	{
		return -1;
	}

	//创建解码器对象
	ptrDecoder decodeH264 = Decoder::Create(DECODER_ASYNMODEL);
	if (nullptr == decodeH264)
	{
		if (YUVFile)
		{
			fclose(YUVFile);
		}
		return -1;
	}

	//设置解码回调函数
	decodeH264->SetDecoderFuncCB([=](ptrFrameData pData, int nWidth, int nHeight, YUVType yuvtype){
		fwrite(pData->data(), 1, pData->size(), YUVFile);
	});

	//打开解码器
	if (0 == decodeH264->open(DEC_CODEC_ID_H264))
	{
		decodeH264->close();
		if (YUVFile)
		{
			fclose(YUVFile);
		}
		return -1;
	}

	//打开H264文件
	av_register_all();
	AVFormatContext *ifmt_ctx = NULL;
	if ((avformat_open_input(&ifmt_ctx, "../Media/cuc_ieschool.h264", 0, 0)) < 0) 
	{
		printf("Could not open input file.");
		return -1;
	}

	if ((avformat_find_stream_info(ifmt_ctx, 0)) < 0) 
	{
		printf("Failed to retrieve input stream information");
		return -1;
	}

	//从H264文件中读取H264裸流并进行解码
	while (1)
	{
		AVPacket pkt;
		int ret = av_read_frame(ifmt_ctx, &pkt);
		if (ret < 0)
		{
			break;
		}

		ptrFrameData pData = std::make_shared<std::vector<unsigned char> >(pkt.data, pkt.data + pkt.size);
		decodeH264->DecoderFrame(pData);

		av_packet_unref(&pkt);
	}

	avformat_close_input(&ifmt_ctx);

	getchar();
	if (0 == decodeH264->close())
	{
		return -1;
	}
	decodeH264.reset();
	fclose(YUVFile);
	return 0;
}