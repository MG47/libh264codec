/*
* h264_decoder.h
*/

#include "h264_decoder.h"

H264_decoder::H264_decoder()
{
	DEBUG_PRINT_DEBUG("h264 decoder constructor");
	Parser m_parser;
	m_num_nal = 0;
	m_sps_count = 0;
	memset(&m_cur_nh, 0, sizeof(struct nal_header));
	memset(&m_sinfo, 0, sizeof(struct session_info));
	memset(&m_sps, 0, sizeof(struct sps));
	memset(&m_pps, 0, sizeof(struct pps));
	memset(&m_mbh, 0, sizeof(struct mb_header));
	m_frame = nullptr;
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
	ret = m_parser.open_file(in_file, out_file);
	if (!ret)
		return 0;

		DEBUG_PRINT_INFO("buffer size %u", ret);


	uint8_t *nal_buf = new uint8_t[ret];
	while (1) {
		ret = read_nalu(nal_buf);
		if (ret <= 0)
			break;

		switch (m_cur_nh.nal_unit_type) {
		case seq_parameter_set_rbsp:
			m_sps_count++;
			ret = parse_sps(nal_buf);
			if (ret < 0)
				return -1;
			break;
		case pic_parameter_set_rbsp:
			m_pps_count++;
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
			DEBUG_PRINT_ERROR("Unsupported NAL unit type %u", m_cur_nh.nal_unit_type);
			return -1;
		}
	}

	dealloc_yuv420p_frame(m_frame);
	delete[] nal_buf;
	return ret;
}

/*
* Read one NAL unit and get RBSP
*/
int H264_decoder::read_nalu(uint8_t *nal_buf)
{
	int32_t ret;

	ret = m_parser.get_nalu(nal_buf);
	if (ret <= 0)
		return ret;

	m_num_nal++;

	m_cur_nal_len = ret;
	m_cur_nh.forbidden_zero_bit = nal_buf[0] >> 7;
	m_cur_nh.nal_ref_idc = nal_buf[0] >> 5;
	m_cur_nh.nal_unit_type = nal_buf[0] & 0x1F;

	DEBUG_PRINT_DEBUG("=================================");
	DEBUG_PRINT_DEBUG("Reading NAL Unit");
	DEBUG_PRINT_DEBUG("Length of NAL unit : %u", ret);


	DEBUG_PRINT_DEBUG("forbidden = %u", m_cur_nh.forbidden_zero_bit);
	DEBUG_PRINT_DEBUG("nal_ref_idc = %u", m_cur_nh.nal_ref_idc);
	DEBUG_PRINT_DEBUG("nal_unit_type = %u", m_cur_nh.nal_unit_type);

	if (m_cur_nh.forbidden_zero_bit) {
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

	DEBUG_PRINT_INFO("----------m_sps-----------");

	m_sps.profile_idc = nal_buf[1];
	if (m_sps.profile_idc != PROFILE_BASELINE) {
		DEBUG_PRINT_ERROR("Unsupported profile");
		return -1;
	}
	DEBUG_PRINT_INFO("profile_idc = %u", m_sps.profile_idc);

	m_sps.level_idc = nal_buf[3];

	switch (m_sps.level_idc) {
	case LEVEL_1:
	case LEVEL_1_B:
	case LEVEL_1_1:
	case LEVEL_1_2:
	case LEVEL_1_3:
	case LEVEL_2:
	case LEVEL_2_1:
	case LEVEL_2_2:
	case LEVEL_3:
		DEBUG_PRINT_INFO("level_idc = %u", m_sps.level_idc);
		break;
	default:
		DEBUG_PRINT_ERROR("Unsupported level");
		return -1;
	}

	m_sps.seq_parameter_set_id = exp_golomb_decode(&nal_buf[4], &expG_offset);
	if (m_sps.seq_parameter_set_id > 31) {
		DEBUG_PRINT_ERROR(
			"Invalid value for seq_parameter_set_id: %u",
			m_sps.seq_parameter_set_id);
			return -1;
	}
	DEBUG_PRINT_INFO("seq_parameter_set_id = %u", m_sps.seq_parameter_set_id);

	m_sps.log2_max_frame_num_minus4 = exp_golomb_decode(&nal_buf[4], &expG_offset);
	if (m_sps.log2_max_frame_num_minus4 > 12) {
		DEBUG_PRINT_ERROR(
			"Invalid value for log2_max_frame_num_minus4: %u",
			m_sps.log2_max_frame_num_minus4);
			return -1;
	}
	DEBUG_PRINT_INFO("log2_max_frame_num_minus4 = %u",
		m_sps.log2_max_frame_num_minus4);

	m_sps.pic_order_cnt_type = exp_golomb_decode(&nal_buf[4], &expG_offset);
	if (m_sps.pic_order_cnt_type > 2) {
		DEBUG_PRINT_ERROR(
			"Invalid value for pic_order_cnt_type: %u",
			m_sps.pic_order_cnt_type);
			return -1;
	}
	DEBUG_PRINT_INFO("pic_order_cnt_type = %u", m_sps.pic_order_cnt_type);

	m_sps.log2_max_pic_order_cnt_lsb_minus4 =
		exp_golomb_decode(&nal_buf[4], &expG_offset);
	if (m_sps.log2_max_pic_order_cnt_lsb_minus4 > 12) {
		DEBUG_PRINT_ERROR(
			"Invalid value for log2_max_pic_order_cnt_lsb_minus4: %u",
			m_sps.log2_max_pic_order_cnt_lsb_minus4);
			return -1;
	}
	DEBUG_PRINT_INFO("log2_max_pic_order_cnt_lsb_minus4 = %u",
		m_sps.log2_max_pic_order_cnt_lsb_minus4);

	// TODO check spec again
	m_sps.num_ref_frames = exp_golomb_decode(&nal_buf[4], &expG_offset);
	DEBUG_PRINT_INFO("num_ref_frames = %u", m_sps.num_ref_frames);

	m_sps.gaps_in_frame_num_value_allowed_flag = get_bit(&nal_buf[4], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("gaps_in_frame_num_value_allowed_flag = %u", m_sps.gaps_in_frame_num_value_allowed_flag);

	m_sps.pic_width_in_mbs_minus_1 = exp_golomb_decode(&nal_buf[4], &expG_offset);
	DEBUG_PRINT_INFO("pic_width_in_mbs_minus_1 = %u", m_sps.pic_width_in_mbs_minus_1);

	m_sps.pic_height_in_map_units_minus_1 = exp_golomb_decode(&nal_buf[4], &expG_offset);
	DEBUG_PRINT_INFO("pic_height_in_map_units_minus_1 = %u", m_sps.pic_height_in_map_units_minus_1);

	m_sps.frame_mbs_only_flag = get_bit(&nal_buf[4], expG_offset);
	if (!m_sps.frame_mbs_only_flag) {
		DEBUG_PRINT_ERROR(
			"Frame-Field decoding is not supported");
			return -1;
	}
	expG_offset++;
	DEBUG_PRINT_INFO("frame_mbs_only_flag = %u", m_sps.frame_mbs_only_flag);

	m_sps.direct_8x8_inference_flag = get_bit(&nal_buf[4], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("direct_8x8_inference_flag = %u", m_sps.direct_8x8_inference_flag);

	m_sps.frame_cropping_flag = get_bit(&nal_buf[4], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("frame_cropping_flag = %u", m_sps.frame_cropping_flag);

	m_sps.vui_prameters_present_flag = get_bit(&nal_buf[4], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("vui_prameters_present_flag = %u", m_sps.vui_prameters_present_flag);

	m_sps.rbsp_stop_one_bit = get_bit(&nal_buf[4], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("rbsp_stop_one_bit = %u", m_sps.rbsp_stop_one_bit);
	DEBUG_PRINT_INFO("---------------------");

	//TODO
	DEBUG_PRINT_INFO("----------Session Info-----------");
	m_sinfo.width = (m_sps.pic_width_in_mbs_minus_1 + 1) * 16;
	m_sinfo.height = (m_sps.pic_height_in_map_units_minus_1 + 1) * 16;
	m_sinfo.bit_depth = 0;
	m_sinfo.frame_rate = 0;
	m_sinfo.profile = m_sps.profile_idc;
	m_sinfo.level = m_sps.level_idc;
	m_sinfo.bitrate = 0;
	DEBUG_PRINT_INFO("width = %u", m_sinfo.width);
	DEBUG_PRINT_INFO("height = %u", m_sinfo.height);
	DEBUG_PRINT_INFO("profile = %u", m_sinfo.profile);
	DEBUG_PRINT_INFO("level = %u", m_sinfo.level);

	return 0;
}

/*
* Parse picture parameter set
*/
int H264_decoder::parse_pps(uint8_t *nal_buf)
{
	uint8_t expG_offset = 0;
	DEBUG_PRINT_INFO("----------m_pps-----------");

	m_pps.pic_parameter_set_id = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("pic_parameter_set_id = %u", m_pps.pic_parameter_set_id);

	m_pps.seq_parameter_set_id = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("seq_parameter_set_id = %u", m_pps.seq_parameter_set_id);

	m_pps.entropy_coding_mode_flag = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("entropy_coding_mode_flag = %u", m_pps.entropy_coding_mode_flag);

	m_pps.bottom_field_pic_order_in_frame_present_flag = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("bottom_field_pic_order_in_frame_present_flag = %u",
		m_pps.bottom_field_pic_order_in_frame_present_flag);

	m_pps.num_slice_groups_minus1 = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("num_slice_groups_minus1 = %u", m_pps.num_slice_groups_minus1);

	if (m_pps.num_slice_groups_minus1 > 0) {
		m_pps.slice_group_map_type = exp_golomb_decode(&nal_buf[1], &expG_offset);
		DEBUG_PRINT_INFO("slice_group_map_type = %u", m_pps.slice_group_map_type);

		//TODO slice_group_map_type value conditions

	}

	m_pps.num_ref_idx_l0_default_active_minus1 = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("num_ref_idx_l0_default_active_minus1 = %u",
		m_pps.num_ref_idx_l0_default_active_minus1);

	m_pps.num_ref_idx_l1_default_active_minus1 = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("num_ref_idx_l1_default_active_minus1 = %u",
		m_pps.num_ref_idx_l1_default_active_minus1);

	m_pps.weighted_pred_flag = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("weighted_pred_flag = %u", m_pps.weighted_pred_flag);

	m_pps.weighted_bipred_idc = get_bit(&nal_buf[1], expG_offset) << 1 |
		get_bit(&nal_buf[1], expG_offset + 1);
	expG_offset += 2;
	DEBUG_PRINT_INFO("weighted_bipred_idc = %u",m_pps.weighted_bipred_idc);

	// TODO signed golomb
	m_pps.pic_init_qp_minus26 = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("pic_init_qp_minus26 = %u",m_pps.pic_init_qp_minus26);

	// TODO signed golomb
	m_pps.pic_init_qs_minus26 = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("pic_init_qs_minus26 = %u",m_pps.pic_init_qs_minus26);

	// TODO signed golomb
	m_pps.chroma_qp_index_offset = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("chroma_qp_index_offset = %u",m_pps.chroma_qp_index_offset);

	m_pps.deblocking_filter_control_present_flag = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("deblocking_filter_control_present_flag = %u",
		m_pps.deblocking_filter_control_present_flag);

	m_pps.constrained_intra_pred_flag = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("constrained_intra_pred_flag = %u", m_pps.constrained_intra_pred_flag);

	m_pps.redundant_pic_cnt_present_flag = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("redundant_pic_cnt_present_flag = %u", m_pps.redundant_pic_cnt_present_flag);

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

	m_sh.first_mb_in_slice = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("first_mb_in_slice = %u", m_sh.first_mb_in_slice);

	m_sh.slice_type = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("slice_type = %u", m_sh.slice_type);

	m_sh.pic_parameter_set_id = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("pic_parameter_set_id = %u", m_sh.pic_parameter_set_id);

	// TODO condition separate_colour_plane_flag
	m_sh.frame_num = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("frame_num = %u", m_sh.frame_num);

	//TODO
	if (!m_sps.frame_mbs_only_flag) {
		// field_pic_flag
		// if (field_pic_flag)
		// bottom_field_flag
	}

	if (IdrPicFlag) {
		m_sh.idr_pic_id = exp_golomb_decode(&nal_buf[1], &expG_offset);
		DEBUG_PRINT_INFO("idr_pic_id = %u", m_sh.idr_pic_id);
	}

	m_sh.pic_order_cnt_lsb = get_bit(&nal_buf[1], expG_offset);
	expG_offset++;
	DEBUG_PRINT_INFO("pic_order_cnt_lsb = %u", m_sh.pic_order_cnt_lsb);

	//TODO other fields

	//TODO signed golomb
	m_sh.slice_qp_delta = exp_golomb_decode(&nal_buf[1], &expG_offset);
	DEBUG_PRINT_INFO("slice_qp_delta = %u", m_sh.slice_qp_delta);

	/* -----------------------------------------------------------*/
	/* TODO write a seprate function to write data to output YUV */

	if (!m_frame) {
		m_frame = alloc_yuv420p_frame(m_sinfo.width, m_sinfo.height);
		if (!m_frame) {
			DEBUG_PRINT_ERROR("Could not allocate a frame");
			return -1;
		}

	}

	int i, j, x, y;
	uint32_t count = 1 + (expG_offset / 8);
	uint32_t bytes_written = 0;

	for (i = 0; i < m_sinfo.height / 16 ; i++) {
		for (j = 0; j < m_sinfo.width / 16; j++) {

			//TODO remove i==0 j==0 conditions
			//macroblock header parsing
			if (!((i == 0) && (j == 0))) {
				uint8_t local_offset = 0;
				m_mbh.mb_type = exp_golomb_decode(&nal_buf[count], &local_offset);
				if (m_mbh.mb_type != I_PCM) {
					DEBUG_PRINT_ERROR("only I_PCM mb type is supported");
							return -1;
				}
				DEBUG_PRINT_INFO("mb_type = %u", m_mbh.mb_type);
				count += 1 + local_offset / 8;
			} else {
				count += 2;
			}


			//TODO remove hardcoding for mb size
			for (x = i * 16; x < (i + 1) * 16; x++)
				for (y = j * 16; y < (j + 1) * 16; y++)
					m_frame->frm_data[x * m_sinfo.width + y] = nal_buf[count++];
			for (x = i * 8; x < (i + 1) * 8; x++)
				for (y = j * 8; y < (j + 1) * 8; y++)
					m_frame->frm_data[m_frame->cb_offset + x * m_sinfo.width / 2 + y]
						= nal_buf[count++];
			for (x = i * 8; x < (i + 1) * 8; x++)
				for (y = j * 8; y < (j + 1) * 8; y++)
					m_frame->frm_data[m_frame->cr_offset + x * m_sinfo.width / 2 + y]
						= nal_buf[count++];
		}
	}

	bytes_written = m_parser.write_output_file((uint8_t *)m_frame->frm_data, m_frame->frame_size);
	DEBUG_PRINT_DEBUG("bytes_written  = %u", bytes_written);
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

/*
* Allocate YUV420P frame
* Returns pointer to the frame structure
*/
struct yuv420p_frame *H264_decoder::alloc_yuv420p_frame(uint32_t width, uint32_t height)
{
	struct yuv420p_frame *frame;
	frame = new yuv420p_frame;
	frame->frame_size = width * height + 2 * (width * height / 4);
	frame->cb_offset = width * height;
	frame->cr_offset = frame->cb_offset + width * height / 4;
	frame->frm_data = new uint8_t[3 * width * height / 2]();
	return frame;
}

/*
* Deallocate a frame
*/
void H264_decoder::dealloc_yuv420p_frame(struct yuv420p_frame *frame)
{
	if (!frame)
		return;
	delete[] frame->frm_data;
	delete frame;
}

