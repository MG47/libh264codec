/*
* h264_decoder.h
*/

#ifndef __H264_DECODER_H__
#define __H264_DECODER_H__

#include "h264_common.h"

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
		Parser m_parser;
		struct session_info m_sinfo;

		// nal
		int read_nalu(uint8_t *nal_buf);
		uint32_t m_num_nal;
		struct nal_header m_cur_nh;
		uint32_t m_cur_nal_len;

		// sps
		struct sps m_sps;
		int parse_sps(uint8_t *nal_buf);
		uint32_t m_sps_count;

		//pps
		struct pps m_pps;
		int parse_pps(uint8_t *nal_buf);
		uint32_t m_pps_count;

		//slice
		struct slice_header m_sh;
		int parse_slice_idr(uint8_t *nal_buf);

		//macroblock
		struct mb_header m_mbh;

		uint32_t exp_golomb_decode(void *buf, uint8_t *offset);
};

#endif /*__H264_DECODER_H__*/



