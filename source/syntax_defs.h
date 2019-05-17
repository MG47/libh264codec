/*
* syntax_defs.h - H264 syntax definitions
*/

#ifndef __SYNTAX_DEFS_H__
#define __SYNTAX_DEFS_H__

#include "h264_common.h"

/* ================ NAL unit definitions ================ */
struct nal_header {
	uint8_t forbidden_zero_bit;
	uint8_t nal_ref_idc;
	uint8_t nal_unit_type;
};

enum nal_unit_type {
	Unspecified = 0x0,
	slice_layer_without_partitioning_rbsp = 0x1,
	slice_data_partition_a_layer_rbsp = 0x2,
	slice_data_partition_b_layer_rbsp = 0x3,
	slice_data_partition_c_layer_rbsp = 0x4,
	/* TODO fix name */
	slice_layer_without_partitioning_rbsp_idr = 0x5,
	sei_rbsp = 0x6,
	seq_parameter_set_rbsp = 0x7,
	pic_parameter_set_rbsp = 0x8,
	access_unit_delimiter_rbsp = 0x9,
	end_of_seq_rbsp = 0xA,
	end_of_stream_rbsp = 0xB,
	filler_data_rbsp = 0xC,
};

/* ================ SPS definitions ================ */
struct sps {
	uint8_t profile_idc;
	uint8_t constraint_set_flags;
	uint8_t level_idc;
	uint8_t seq_parameter_set_id;
	uint8_t log2_max_frame_num_minus4;
	uint8_t log2_max_pic_order_cnt_lsb_minus4;
	uint8_t pic_order_cnt_type;
	uint32_t num_ref_frames;
	uint8_t gaps_in_frame_num_value_allowed_flag;
	uint32_t pic_width_in_mbs_minus_1;
	uint32_t pic_height_in_map_units_minus_1;
	uint8_t frame_mbs_only_flag;
	uint8_t direct_8x8_inference_flag;
	uint8_t frame_cropping_flag;
	uint8_t vui_prameters_present_flag;
	uint8_t rbsp_stop_one_bit;
};

/* ================ PPS definitions ================ */
struct pps {
	uint8_t pic_parameter_set_id;
	uint8_t seq_parameter_set_id;
	uint8_t entropy_coding_mode_flag;
	uint8_t bottom_field_pic_order_in_frame_present_flag;
	uint8_t num_slice_groups_minus1;
	uint8_t slice_group_map_type;
	uint8_t run_length_minus1;
	uint8_t top_left;
	uint8_t bottom_right;
	uint8_t slice_group_change_direction_flag;
	uint8_t slice_group_change_rate_minus1;
	uint8_t slice_group_id;
	uint8_t num_ref_idx_l0_default_active_minus1;
	uint8_t num_ref_idx_l1_default_active_minus1;
	uint8_t weighted_pred_flag;
	uint8_t weighted_bipred_idc;
	uint8_t pic_init_qp_minus26;
	uint8_t pic_init_qs_minus26;
	uint8_t chroma_qp_index_offset;
	uint8_t deblocking_filter_control_present_flag;
	uint8_t constrained_intra_pred_flag;
	uint8_t redundant_pic_cnt_present_flag;
	uint8_t transform_8x8_mode_flag;
	uint8_t pic_scaling_matrix_present_flag;
	uint8_t pic_scaling_list_present_flag;
	uint8_t second_chroma_qp_index_offset;
};

/* ================ Slice definitions ================ */
struct slice_header {
	uint8_t first_mb_in_slice;
	uint8_t slice_type;
	uint8_t pic_parameter_set_id;
	uint8_t colour_plane_id;
	uint8_t frame_num;
	uint8_t field_pic_flag;
	uint8_t bottom_field_flag;
	uint8_t idr_pic_id;
	uint8_t pic_order_cnt_lsb;
	uint8_t delta_pic_order_cnt_bottom;
	uint8_t delta_pic_order_cnt[2];
	uint8_t redundant_pic_cnt;
	uint8_t direct_spatial_mv_pred_flag;
	uint8_t num_ref_idx_active_override_flag;
	uint8_t num_ref_idx_l0_active_minus1;
	uint8_t num_ref_idx_l1_active_minus1;
	// TODO see other fields
	uint8_t cabac_init_idc;
	uint8_t slice_qp_delta;
	uint8_t sp_for_switch_flag;
	uint8_t slice_qs_delta;
	uint8_t disable_deblocking_filter_idc;
	uint8_t slice_alpha_c0_offset_div2;
	uint8_t slice_beta_offset_div2;
	uint8_t slice_group_change_cycle;
};

/* ================ Macroblock definitions ================ */

enum MB_TYPE {
	I_PCM = 25,
};

struct mb_header {
	uint8_t mb_type;
};


/* Supported profiles */
#define PROFILE_BASELINE 0x42

/* Supported levels */
#define LEVEL_1 	0xA
#define LEVEL_1_B 	0x9
#define LEVEL_1_1 	0xB
#define LEVEL_1_2 	0xC
#define LEVEL_1_3 	0xD
#define LEVEL_2 	0x14
#define LEVEL_2_1 	0x15
#define LEVEL_2_2 	0x16
#define LEVEL_3 	0x1E



#endif /*__SYNTAX_DEFS_H__*/



