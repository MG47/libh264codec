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
			ret = parse_sps(nal_buf);
			if (ret < 0)
				return -1;
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
	uint8_t expG_offset = 0;

	DEBUG_PRINT_INFO("----------SPS-----------");

	sps.profile_idc = nal_buf[1];
	if (sps.profile_idc != PROFILE_BASELINE) {
		DEBUG_PRINT_ERROR("Unsupported profile");
		return -1;
	}
	DEBUG_PRINT_INFO("profile_idc = %u", sps.profile_idc);

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
		DEBUG_PRINT_INFO("level_idc = %u", sps.level_idc);
		break;
	default:
		DEBUG_PRINT_ERROR("Unsupported level");
		return -1;
	}

	sps.seq_parameter_set_id = exp_goulomb_decode(&nal_buf[4], &expG_offset);
	DEBUG_PRINT_INFO("seq_parameter_set_id = %u", sps.seq_parameter_set_id);

	sps.log2_max_frame_num_minus4 = exp_goulomb_decode(&nal_buf[4], &expG_offset);
	if (sps.log2_max_frame_num_minus4 > 12) {
		DEBUG_PRINT_ERROR(
			"Invalid value for log2_max_frame_num_minus4: %u",
			sps.log2_max_frame_num_minus4);
			return -1;
	}
	DEBUG_PRINT_INFO("log2_max_frame_num_minus4 = %u",
		sps.log2_max_frame_num_minus4);

	sps.pic_order_cnt_type = exp_goulomb_decode(&nal_buf[4], &expG_offset);
	DEBUG_PRINT_INFO("pic_order_cnt_type = %u", sps.pic_order_cnt_type);

	sps.log2_max_pic_order_cnt_lsb_minus4 = exp_goulomb_decode(&nal_buf[4], &expG_offset);
	DEBUG_PRINT_INFO("log2_max_pic_order_cnt_lsb_minus4 = %u", sps.log2_max_pic_order_cnt_lsb_minus4);

	sps.num_ref_frames = exp_goulomb_decode(&nal_buf[4], &expG_offset);
	DEBUG_PRINT_INFO("num_ref_frames = %u", sps.num_ref_frames);

	//TODO verify
	sps.gaps_in_frame_num_value_allowed_flag = get_bit((uint32_t *)&nal_buf[4], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("gaps_in_frame_num_value_allowed_flag = %u", sps.gaps_in_frame_num_value_allowed_flag);

	sps.pic_width_in_mbs_minus_1 = exp_goulomb_decode(&nal_buf[4], &expG_offset);
	DEBUG_PRINT_INFO("pic_width_in_mbs_minus_1 = %u", sps.pic_width_in_mbs_minus_1);

	DEBUG_PRINT_INFO("---------------------");

	return 0;
}

/*
* Unsigned Exp-Goulomb decoder
* Assumption : Max v in u(v) is 32
*/
uint32_t H264_decoder::exp_goulomb_decode(void *buf, uint8_t *offset)
{
	uint32_t num = *(uint32_t *)buf;
	uint8_t code = 0;
	uint32_t no_of_zeros = 0, INFO = 0;

	num = to_little_endian(num);
	// count no of zeros
	while (!get_bit(&num, (*offset)++)) {
		no_of_zeros++;
	}

	// INFO = read no_of_zeros bits after 1
	// code val = 2^(no_of_zeros) + INFO - 1
	code = (1 << no_of_zeros);
	while (no_of_zeros--)
		INFO = get_bit(&num, (*offset)++);

	code += INFO - 1;
	return code;
}


