/*
* h264_decoder.h
*/

#ifndef __H264_DECODER_H__
#define __H264_DECODER_H__

#include "h264_common.h"

/* NAL unit definitions */
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

class H264_decoder {
	public:
		H264_decoder();
		~H264_decoder();
		bool decode(char *in_file, char *out_file);
	private:
		Parser parser;
		int read_nalu();
		uint32_t m_num_nal;
		struct nal_header nh;
};

#endif /*__H264_DECODER_H__*/



