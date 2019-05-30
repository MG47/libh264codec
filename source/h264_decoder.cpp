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
			sps_count++;
			ret = parse_sps(nal_buf);
			if (ret < 0)
				return -1;
			break;
		case pic_parameter_set_rbsp:
			pps_count++;
			ret = parse_pps(nal_buf);
			if (ret < 0)
				return -1;
			break;
		case slice_layer_without_partitioning_rbsp_idr:
			DEBUG_PRINT_DEBUG("Parsing Coded IDR slice");
			ret = parse_slice_idr(nal_buf);
			if (ret < 0)
				return -1;
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

	sps.seq_parameter_set_id = exp_golomb_decode(&nal_buf[4], &expG_offset);
	if (sps.seq_parameter_set_id > 31) {
		DEBUG_PRINT_ERROR(
			"Invalid value for seq_parameter_set_id: %u",
			sps.seq_parameter_set_id);
			return -1;
	}
	DEBUG_PRINT_INFO("seq_parameter_set_id = %u", sps.seq_parameter_set_id);

	sps.log2_max_frame_num_minus4 = exp_golomb_decode(&nal_buf[4], &expG_offset);
	if (sps.log2_max_frame_num_minus4 > 12) {
		DEBUG_PRINT_ERROR(
			"Invalid value for log2_max_frame_num_minus4: %u",
			sps.log2_max_frame_num_minus4);
			return -1;
	}
	DEBUG_PRINT_INFO("log2_max_frame_num_minus4 = %u",
		sps.log2_max_frame_num_minus4);

	sps.pic_order_cnt_type = exp_golomb_decode(&nal_buf[4], &expG_offset);
	if (sps.pic_order_cnt_type > 2) {
		DEBUG_PRINT_ERROR(
			"Invalid value for pic_order_cnt_type: %u",
			sps.pic_order_cnt_type);
			return -1;
	}
	DEBUG_PRINT_INFO("pic_order_cnt_type = %u", sps.pic_order_cnt_type);

	sps.log2_max_pic_order_cnt_lsb_minus4 =
		exp_golomb_decode(&nal_buf[4], &expG_offset);
	if (sps.log2_max_pic_order_cnt_lsb_minus4 > 12) {
		DEBUG_PRINT_ERROR(
			"Invalid value for log2_max_pic_order_cnt_lsb_minus4: %u",
			sps.log2_max_pic_order_cnt_lsb_minus4);
			return -1;
	}
	DEBUG_PRINT_INFO("log2_max_pic_order_cnt_lsb_minus4 = %u",
		sps.log2_max_pic_order_cnt_lsb_minus4);

	// TODO check spec again
	sps.num_ref_frames = exp_golomb_decode(&nal_buf[4], &expG_offset);
	DEBUG_PRINT_INFO("num_ref_frames = %u", sps.num_ref_frames);

	sps.gaps_in_frame_num_value_allowed_flag = get_bit(&nal_buf[4], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("gaps_in_frame_num_value_allowed_flag = %u", sps.gaps_in_frame_num_value_allowed_flag);

	sps.pic_width_in_mbs_minus_1 = exp_golomb_decode(&nal_buf[4], &expG_offset);
	DEBUG_PRINT_INFO("pic_width_in_mbs_minus_1 = %u", sps.pic_width_in_mbs_minus_1);

	sps.pic_height_in_map_units_minus_1 = exp_golomb_decode(&nal_buf[4], &expG_offset);
	DEBUG_PRINT_INFO("pic_height_in_map_units_minus_1 = %u", sps.pic_height_in_map_units_minus_1);

	sps.frame_mbs_only_flag = get_bit(&nal_buf[4], expG_offset);
	if (!sps.frame_mbs_only_flag) {
		DEBUG_PRINT_ERROR(
			"Frame-Field decoding is not supported");
			return -1;
	}
	expG_offset++;
	DEBUG_PRINT_INFO("frame_mbs_only_flag = %u", sps.frame_mbs_only_flag);

	sps.direct_8x8_inference_flag = get_bit(&nal_buf[4], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("direct_8x8_inference_flag = %u", sps.direct_8x8_inference_flag);

	sps.frame_cropping_flag = get_bit(&nal_buf[4], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("frame_cropping_flag = %u", sps.frame_cropping_flag);

	sps.vui_prameters_present_flag = get_bit(&nal_buf[4], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("vui_prameters_present_flag = %u", sps.vui_prameters_present_flag);

	sps.rbsp_stop_one_bit = get_bit(&nal_buf[4], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("rbsp_stop_one_bit = %u", sps.rbsp_stop_one_bit);
	DEBUG_PRINT_INFO("---------------------");

	//TODO
	DEBUG_PRINT_INFO("----------Session Info-----------");
	sinfo.width = (sps.pic_width_in_mbs_minus_1 + 1) * 16;
	sinfo.height = (sps.pic_height_in_map_units_minus_1 + 1) * 16;
	sinfo.bit_depth = 0;
	sinfo.frame_rate = 0;
	sinfo.profile = sps.profile_idc;
	sinfo.level = sps.level_idc;
	sinfo.bitrate = 0;
	DEBUG_PRINT_INFO("width = %u", sinfo.width);
	DEBUG_PRINT_INFO("height = %u", sinfo.height);
	DEBUG_PRINT_INFO("profile = %u", sinfo.profile);
	DEBUG_PRINT_INFO("level = %u", sinfo.level);

	return 0;
}

/*
* Parse picture parameter set
*/
int H264_decoder::parse_pps(uint8_t *nal_buf)
{
	uint8_t expG_offset = 0;
	DEBUG_PRINT_INFO("----------PPS-----------");

	pps.pic_parameter_set_id = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("pic_parameter_set_id = %u", pps.pic_parameter_set_id);

	pps.seq_parameter_set_id = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("seq_parameter_set_id = %u", pps.seq_parameter_set_id);

	pps.entropy_coding_mode_flag = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("entropy_coding_mode_flag = %u", pps.entropy_coding_mode_flag);

	pps.bottom_field_pic_order_in_frame_present_flag = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("bottom_field_pic_order_in_frame_present_flag = %u",
		pps.bottom_field_pic_order_in_frame_present_flag);

	pps.num_slice_groups_minus1 = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("num_slice_groups_minus1 = %u", pps.num_slice_groups_minus1);

	if (pps.num_slice_groups_minus1 > 0) {
		pps.slice_group_map_type = exp_golomb_decode(&nal_buf[1], &expG_offset);
		DEBUG_PRINT_INFO("slice_group_map_type = %u", pps.slice_group_map_type);

		//TODO slice_group_map_type value conditions

	}

	pps.num_ref_idx_l0_default_active_minus1 = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("num_ref_idx_l0_default_active_minus1 = %u",
		pps.num_ref_idx_l0_default_active_minus1);

	pps.num_ref_idx_l1_default_active_minus1 = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("num_ref_idx_l1_default_active_minus1 = %u",
		pps.num_ref_idx_l1_default_active_minus1);

	pps.weighted_pred_flag = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("weighted_pred_flag = %u", pps.weighted_pred_flag);

	pps.weighted_bipred_idc = get_bit(&nal_buf[1], expG_offset) << 1 |
		get_bit(&nal_buf[1], expG_offset + 1);
	expG_offset += 2;
	DEBUG_PRINT_INFO("weighted_bipred_idc = %u",pps.weighted_bipred_idc);

	// TODO signed golomb
	pps.pic_init_qp_minus26 = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("pic_init_qp_minus26 = %u",pps.pic_init_qp_minus26);

	// TODO signed golomb
	pps.pic_init_qs_minus26 = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("pic_init_qs_minus26 = %u",pps.pic_init_qs_minus26);

	// TODO signed golomb
	pps.chroma_qp_index_offset = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("chroma_qp_index_offset = %u",pps.chroma_qp_index_offset);

	pps.deblocking_filter_control_present_flag = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("deblocking_filter_control_present_flag = %u",
		pps.deblocking_filter_control_present_flag);

	pps.constrained_intra_pred_flag = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("constrained_intra_pred_flag = %u", pps.constrained_intra_pred_flag);

	pps.redundant_pic_cnt_present_flag = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("redundant_pic_cnt_present_flag = %u", pps.redundant_pic_cnt_present_flag);

#if 0
	uint8_t transform_8x8_mode_flag;
	uint8_t pic_scaling_matrix_present_flag;
	uint8_t pic_scaling_list_present_flag;
	uint8_t second_chroma_qp_index_offset;
#endif


	DEBUG_PRINT_INFO("---------------------");
	return 0;
}

/*
* Parse IDR slice
*/
int H264_decoder::parse_slice_idr(uint8_t *nal_buf)
{
	uint8_t expG_offset = 0;
	DEBUG_PRINT_INFO("----------IDR Slice-----------");


	//TODO make struct for flags
	uint8_t IdrPicFlag = 1;

	sh.first_mb_in_slice = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("first_mb_in_slice = %u", sh.first_mb_in_slice);

	sh.slice_type = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("slice_type = %u", sh.slice_type);

	sh.pic_parameter_set_id = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("pic_parameter_set_id = %u", sh.pic_parameter_set_id);

	// TODO condition separate_colour_plane_flag
	sh.frame_num = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("frame_num = %u", sh.frame_num);

	//TODO
	if (!sps.frame_mbs_only_flag) {
		// field_pic_flag
		// if (field_pic_flag)
		// bottom_field_flag
	}

	if (IdrPicFlag) {
		sh.idr_pic_id = exp_golomb_decode(&nal_buf[1], &expG_offset);
		DEBUG_PRINT_INFO("idr_pic_id = %u", sh.idr_pic_id);
	}

	sh.pic_order_cnt_lsb = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("pic_order_cnt_lsb = %u", sh.pic_order_cnt_lsb);

	//TODO other fields

	//TODO signed golomb
	sh.slice_qp_delta = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("slice_qp_delta = %u", sh.slice_qp_delta);

	/* -----------------------------------------------------------*/
	/* TEMP code to write YUV directly */

	/* SQCIF */
	#define LUMA_WIDTH 128
	#define LUMA_HEIGHT 96
	#define CHROMA_WIDTH LUMA_WIDTH / 2
	#define CHROMA_HEIGHT LUMA_HEIGHT / 2

	struct frame
	{
		uint8_t Y[LUMA_HEIGHT][LUMA_WIDTH];
		uint8_t Cb[CHROMA_HEIGHT][CHROMA_WIDTH];
		uint8_t Cr[CHROMA_HEIGHT][CHROMA_WIDTH];
	};


	struct frame fr;
	int i, j, x, y;
	memset(&fr, 0, sizeof(struct frame));

	uint32_t count = 1 + (expG_offset / 8);

	for (i = 0; i < LUMA_HEIGHT/16 ; i++) {
		for (j = 0; j < LUMA_WIDTH/16; j++) {

			//TODO remove i==0 j==0 conditions
			//macroblock header parsing
			if (!((i == 0) && (j == 0))) {
				uint8_t local_offset = 0;
				mbh.mb_type = exp_golomb_decode(&nal_buf[count], &local_offset);
				if (mbh.mb_type != I_PCM) {
					DEBUG_PRINT_ERROR("only I_PCM mb type is supported");
							return -1;
				}
				DEBUG_PRINT_INFO("mb_type = %u", mbh.mb_type);
				count += 1 + local_offset / 8;
			} else {
				count += 2;
			}

			for (x = i * 16; x < (i + 1) * 16; x++)
				for (y = j * 16; y < (j + 1) *16; y++)
					fr.Y[x][y] = nal_buf[count++];
			for (x = i * 8; x < (i + 1) * 8; x++)
				for (y = j * 8; y < (j + 1) * 8; y++)
					fr.Cb[x][y] = nal_buf[count++];
			for (x = i * 8; x < (i + 1) * 8; x++)
				for (y = j * 8; y < (j + 1) * 8; y++)
					fr.Cr[x][y] = nal_buf[count++];
		}
	}
	parser.write_output_file((uint8_t *)&fr, sizeof(struct frame));
	DEBUG_PRINT_DEBUG("bytes_written  = %u", count);
	/* -----------------------------------------------------------*/


	DEBUG_PRINT_INFO("---------------------");

	return 0;
}

/*
* Unsigned Exp-Golomb decoder
* Returns the first code num in provided buffer
* Increments offset by number of bits parsed
* Assumption : Max v in u(v) is 32
*/
uint32_t H264_decoder::exp_golomb_decode(void *buf, uint8_t *offset)
{
	uint32_t num = *(uint32_t *)buf;
	uint32_t no_of_zeros = 0, INFO = 0;

	// count no of zeros
	while (!get_bit(&num, (*offset)++)) {
		no_of_zeros++;
	}

	// INFO = read no_of_zeros bits after 1
	// code val = 2^(no_of_zeros) + INFO - 1
	INFO = (1 << no_of_zeros);
	while (no_of_zeros--)
		INFO |= get_bit(&num, (*offset)++) << no_of_zeros;

	INFO--;
	return INFO;
}


