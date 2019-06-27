/*
* h264_utils.h - Utilities
*/

#ifndef __H264_UTILS_H__
#define __H264_UTILS_H__

#include "h264_common.h"

static inline uint32_t to_little_endian(uint32_t num)
{
	return  ((num >> 24) & 0xFF) |
		((num >> 8) & 0xFF00) |
		((num << 8) & 0xFF0000) |
		((num << 24) & 0xFF000000);
}

static inline uint32_t get_bit(void *base, uint8_t offset)
{
	uint32_t num = *(uint32_t *)base;
	num = to_little_endian(num);
	return ((num >> (31 - offset)) & 0x1);
}

static inline uint32_t get_n_bits(void *base, uint8_t offset, uint8_t count)
{
	uint32_t num = *(uint32_t *)base;
	num = to_little_endian(num);
	return ((num >> (31 - offset)) & (1 << (count - 1)));
}

#endif /*__H264_UTILS_H__*/



