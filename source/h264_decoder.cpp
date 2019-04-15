/*
* h264_decoder.h
*/

#include "h264_decoder.h"

H264_decoder::H264_decoder()
{
	DEBUG_PRINT_DEBUG("h264 decoder constructor\n");
	Parser parser;
}

H264_decoder::~H264_decoder()
{
	DEBUG_PRINT_DEBUG("h264 decoder destructor\n");
}

bool H264_decoder::decode(char *filename)
{
	int ret = 0;
	size_t bytes_written = 0;
	uint8_t *buf;

	DEBUG_PRINT_INFO("h264 decoder start decode\n");
	ret = parser.read_file(filename, buf);
	if (ret)
		return false;


	bytes_written = parser.write_output_file(buf);

	return ret;
}


