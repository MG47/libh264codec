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
	Unspecified 					= 0x0,
	slice_layer_without_partitioning_rbsp 		= 0x1,
	slice_data_partition_a_layer_rbsp 		= 0x2,
	slice_data_partition_b_layer_rbsp 		= 0x3,
	slice_data_partition_c_layer_rbsp 		= 0x4,
	/* TODO fix name */
	slice_layer_without_partitioning_rbsp_idr 	= 0x5,
	sei_rbsp 					= 0x6,
	seq_parameter_set_rbsp 				= 0x7,
	pic_parameter_set_rbsp 				= 0x8,
	access_unit_delimiter_rbsp 			= 0x9,
	end_of_seq_rbsp 				= 0xA,
	end_of_stream_rbsp 				= 0xB,
	filler_data_rbsp 				= 0xC,
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

/* ================ SEI message - Annex D ================ */

struct sei {

#if 0
buffering_period
pic_timing
pan_scan_rect
filler_payload
user_data_registered_itu_t_t35
user_data_unregistered
recovery_point
dec_ref_pic_marking_repetition
spare_pic
scene_info
sub_seq_info
sub_seq_layer_characteristics
sub_seq_characteristics
full_frame_freeze
full_frame_freeze_release
full_frame_snapshot
progressive_refinement_segment_start
progressive_refinement_segment_end
motion_constrained_slice_group_set
film_grain_characteristics
deblocking_filter_display_preference
stereo_video_info
post_filter_hint
tone_mapping_info
scalability_info
sub_pic_scalable_layer
non_required_layer_rep
priority_layer_info
layers_not_present
layer_dependency_change
scalable_nesting
base_layer_temporal_hrd
quality_layer_integrity_check
redundant_pic_property
tl0_dep_rep_index
tl_switching_point
parallel_decoding_info
view_scalability_info
multiview_scene_info
multiview_acquisition_info
non_required_view_component
view_dependency_change
operation_points_not_present
base_view_temporal_hrd
frame_packing_arrangement
multiview_view_position
display_orientation
mvcd_scalable_nesting
mvcd_view_scalability_info
depth_representation_info
three_dimensional_reference_displays_info
depth_timing
depth_sampling_info
constrained_depth_parameter_set_identifier
green_metadata
mastering_display_colour_volume
colour_remapping_info
alternative_transfer_characteristics
alternative_depth_info
reserved_sei_message
#endif

};

/* ================ VUI parameters - Annex E ================ */

struct hrd_parameters {
	uint8_t cpb_cnt_minus1;
	uint8_t bit_rate_scale;
	uint8_t cpb_size_scale;
	uint8_t bit_rate_value_minus1;
	uint8_t cpb_size_value_minus1;
	uint8_t cbr_flag;
	uint8_t initial_cpb_removal_delay_length_minus1;
	uint8_t cpb_removal_delay_length_minus1;
	uint8_t dpb_output_delay_length_minus1;
	uint8_t time_offset_length;
};

struct vui_parameters {
	uint8_t aspect_ratio_info_present_flag;
	uint8_t aspect_ratio_idc;
	uint16_t sar_width;
	uint16_t sar_height;
	uint8_t overscan_info_present_flag;
	uint8_t overscan_appropriate_flag;
	uint8_t video_signal_type_present_flag;
	uint8_t video_format;
	uint8_t video_full_range_flag;
	uint8_t colour_description_present_flag;
	uint8_t colour_primaries;
	uint8_t transfer_characteristics;
	uint8_t matrix_coefficients;
	uint8_t chroma_loc_info_present_flag;
	uint8_t chroma_sample_loc_type_top_field;
	uint8_t chroma_sample_loc_type_bottom_field;
	uint8_t timing_info_present_flag;
	uint32_t num_units_in_tick;
	uint32_t time_scale;
	uint8_t fixed_frame_rate_flag;
	uint8_t nal_hrd_parameters_present_flag;
	uint8_t vcl_hrd_parameters_present_flag;
	uint8_t low_delay_hrd_flag;
	uint8_t pic_struct_present_flag;
	uint8_t motion_vectors_over_pic_boundaries_flag;
	uint8_t max_bytes_per_pic_denom;
	uint8_t max_bits_per_mb_denom;
	uint8_t log2_max_mv_length_horizontal;
	uint8_t log2_max_mv_length_vertical;
	uint8_t max_num_reorder_frames;
	uint8_t max_dec_frame_buffering;
	struct hrd_parameters hrd_params;
};

/* ================ Macroblock definitions ================ */

enum MB_TYPE {
	I_PCM = 25,
};

struct mb_header {
	uint8_t mb_type;
};

/* ================ Profiles and Levels ================ */

/* Supported profiles */
enum h264_profiles {
	PROFILE_BASELINE  = 0x42,
};

/* Supported levels */
enum h264_levels  {
	LEVEL_1 	= 0xA,
	LEVEL_1_B 	= 0x9,
	LEVEL_1_1 	= 0xB,
	LEVEL_1_2 	= 0xC,
	LEVEL_1_3 	= 0xD,
	LEVEL_2 	= 0x14,
	LEVEL_2_1 	= 0x15,
	LEVEL_2_2 	= 0x16,
	LEVEL_3 	= 0x1E,
};




#endif /*__SYNTAX_DEFS_H__*/



