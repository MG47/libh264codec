/*
* h264_decoder.h
*/

#include "h264_decoder.h"

H264_decoder::H264_decoder()
{
	DEBUG_PRINT_DEBUG("h264 decoder constructor");
	Parser parser;
	m_num_nal = 0;
}

H264_decoder::~H264_decoder()
{
	DEBUG_PRINT_DEBUG("h264 decoder destructor");
}

/*
* Main decode routine
*/
bool H264_decoder::decode(char *filename)
{
	int ret = 0;
	size_t bytes_written = 0;
	uint8_t *buf;

	DEBUG_PRINT_INFO("h264 decoder start decode");
	ret = parser.open_file(filename, buf);
	if (ret)
		return false;

	while (!ret) {
		DEBUG_PRINT_DEBUG("=================================");
		DEBUG_PRINT_DEBUG("Reading NAL Unit");
		ret = read_nalu();
	}

	bytes_written = parser.write_output_file(buf);

	return ret;
}

/*
* Read one NAL unit and get RBSP
*/
int H264_decoder::read_nalu()
{
	uint8_t *nal_buf;

	nal_buf = parser.get_nalu();
	if (!nal_buf)
		return -1;

	m_num_nal++;

	nh.forbidden_zero_bit = nal_buf[0] >> 7;
	nh.nal_ref_idc = nal_buf[0] >> 5;
	nh.nal_unit_type = nal_buf[0] & 0x1F;

	DEBUG_PRINT_DEBUG("forbidden = %u", nh.forbidden_zero_bit);
	DEBUG_PRINT_DEBUG("nal_ref_idc = %u", nh.nal_ref_idc);
	DEBUG_PRINT_DEBUG("nal_unit_type = %u", nh.nal_unit_type);

	if (nh.forbidden_zero_bit) {
		DEBUG_PRINT_INFO("Corrupted bitstream, forbidden bit is non-zero");
		return -1;
	}

	nh.nal_ref_idc = nal_buf[1] << 8 | nal_buf[0];

	return 0;
}


