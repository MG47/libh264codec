/*
* h264_decoder.h
*/

#include "h264_decoder.h"

H264_decoder::H264_decoder()
{
	DEBUG_PRINT_DEBUG("h264 decoder constructor");
	Parser parser;
	m_num_nal = 0;
	sps_count = 0;
	memset(&cur_nh, 0, sizeof(struct nal_header));
	memset(&sinfo, 0, sizeof(struct session_info));
}

H264_decoder::~H264_decoder()
{
	DEBUG_PRINT_DEBUG("h264 decoder destructor");
}

/*
* Main decode routine
*/
int32_t H264_decoder::decode(char *in_file, char *out_file)
{
	int ret = 0;
	size_t bytes_written = 0;
	uint8_t *buf;

	DEBUG_PRINT_INFO("h264 decoder start decode");
	ret = parser.open_file(in_file, out_file);
	if (!ret)
		return 0;

		DEBUG_PRINT_INFO("buffer size %u", ret);


	uint8_t *nal_buf = new uint8_t[ret];
	while (1) {
		ret = read_nalu(nal_buf);
		if (ret <= 0)
			break;

		switch (cur_nh.nal_unit_type) {
		case seq_parameter_set_rbsp:
			parse_sps(nal_buf);
			break;
		case pic_parameter_set_rbsp:
			DEBUG_PRINT_DEBUG("Parsing PPS");
			break;
		case slice_layer_without_partitioning_rbsp_idr:
			DEBUG_PRINT_DEBUG("Parsing Coded IDR slice");
			// Temp code: write slice data to output yuv
			ret = parser.write_output_file(&nal_buf[5], (cur_nal_len - 5));
			DEBUG_PRINT_DEBUG("Wrote %u bytes", ret);
			break;
		default:
			DEBUG_PRINT_ERROR("Unsupported NAL unit type %u", cur_nh.nal_unit_type);
			return -1;
		}
	}

	delete[] nal_buf;
	return ret;
}

/*
* Read one NAL unit and get RBSP
*/
int H264_decoder::read_nalu(uint8_t *nal_buf)
{
	int32_t ret;

	ret = parser.get_nalu(nal_buf);
	if (ret <= 0)
		return ret;

	m_num_nal++;

	cur_nal_len = ret;
	cur_nh.forbidden_zero_bit = nal_buf[0] >> 7;
	cur_nh.nal_ref_idc = nal_buf[0] >> 5;
	cur_nh.nal_unit_type = nal_buf[0] & 0x1F;

	DEBUG_PRINT_DEBUG("=================================");
	DEBUG_PRINT_DEBUG("Reading NAL Unit");
	DEBUG_PRINT_DEBUG("Length of NAL unit : %u", ret);


	DEBUG_PRINT_DEBUG("forbidden = %u", cur_nh.forbidden_zero_bit);
	DEBUG_PRINT_DEBUG("nal_ref_idc = %u", cur_nh.nal_ref_idc);
	DEBUG_PRINT_DEBUG("nal_unit_type = %u", cur_nh.nal_unit_type);

	if (cur_nh.forbidden_zero_bit) {
		DEBUG_PRINT_INFO("Corrupted bitstream, forbidden bit is non-zero");
		return -1;
	}

	/* TODO emulation prevention byte */

	return ret;
}

/*
* Parse sequence parameter set
*/
int H264_decoder::parse_sps(uint8_t *nal_buf)
{
	DEBUG_PRINT_DEBUG("----------Parsing SPS-----------");

#if 0

/* ================ SPS definitions ================ */
struct sps {
	uint8_t profile_idc;
	uint8_t constraint_set_flags;
	uint8_t level_idc;
	uint8_t seq_parameter_set_id;
	uint8_t chroma_format_idc;
};

#define PROFILE_BASELINE 0x42
#endif
	sps.profile_idc = nal_buf[1];
	if (sps.profile_idc != PROFILE_BASELINE) {
		DEBUG_PRINT_ERROR("Unsupported profile");
		return -1;
	}
	DEBUG_PRINT_DEBUG("profile = %u", sps.profile_idc);

	sps.level_idc = nal_buf[3];

	switch (sps.level_idc) {
	case LEVEL_1:
	case LEVEL_1_B:
	case LEVEL_1_1:
	case LEVEL_1_2:
	case LEVEL_1_3:
	case LEVEL_2:
	case LEVEL_2_1:
	case LEVEL_2_2:
	case LEVEL_3:
		DEBUG_PRINT_DEBUG("level = %u", sps.level_idc);
		break;
	default:
		DEBUG_PRINT_ERROR("Unsupported level");
		return -1;
	}

	return 0;
}
