/*
* h264_utils.h - Utilities
*/

#ifndef __H264_UTILS_H__
#define __H264_UTILS_H__

#include "h264_common.h"

static inline uint32_t get_bit(uint32_t *base, uint8_t offset)
{
	return (((*base) >> (31 - offset)) & 0x1);
}

static inline uint32_t to_little_endian(uint32_t num)
{
	return  ((num >> 24) & 0xFF) |
		((num >> 8) & 0xFF00) |
		((num << 8) & 0xFF0000) |
		((num << 24) & 0xFF000000);
}


#endif /*__H264_UTILS_H__*/



