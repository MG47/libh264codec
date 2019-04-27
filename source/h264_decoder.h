/*
* h264_decoder.h
*/

#ifndef __H264_DECODER_H__
#define __H264_DECODER_H__

#include "h264_common.h"
#include "syntax_defs.h"

/* Session level info */
struct session_info {
	uint32_t width;
	uint32_t height;
	uint32_t bit_depth;
	uint32_t frame_rate;
	uint32_t profile;
	uint32_t level;
	uint32_t bitrate;
};


class H264_decoder {
	public:
		H264_decoder();
		~H264_decoder();
		int32_t decode(char *in_file, char *out_file);
	private:
		Parser parser;
		int read_nalu(uint8_t *nal_buf);
		int parse_sps();
		uint32_t sps_count;
		uint32_t m_num_nal;
		struct nal_header cur_nh;
		struct session_info sinfo;
};

#endif /*__H264_DECODER_H__*/



