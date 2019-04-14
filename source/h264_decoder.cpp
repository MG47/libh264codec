/*
* h264_decoder.h
*/

#include "h264_decoder.h"

H264_decoder::H264_decoder()
{
	printf("h264 decoder constructor\n");
	Parser parser;
}

H264_decoder::~H264_decoder()
{
	printf("h264 decoder destructor\n");
}

bool H264_decoder::decode(char *filename)
{
	int ret = 0;
	uint8_t *buf;
	printf("h264 decoder start decode\n");
	ret = parser.read_file(filename, buf);
	if (ret)
		return false;

	return ret;
}


