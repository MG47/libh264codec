/*
* debug.h - debug macros
*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "h264_common.h"

#define LOG_DEBUG "DEBUG"
#define LOG_INFO "INFO"
#define LOG_ERROR "ERROR"

#ifdef DEBUG
#define DEBUG_PRINT_DEBUG(...) \
	printf("%s::%s:%s: %s", LOG_DEBUG, __FILE__, __func__, __VA_ARGS__)
#else
#define DEBUG_PRINT_DEBUG
#endif

#define DEBUG_PRINT_INFO(...) \
	printf("%s::%s:%s: %s", LOG_INFO, __FILE__, __func__, __VA_ARGS__)

#define DEBUG_PRINT_ERROR(...) \
	printf("%s::%s:%s: %s", LOG_ERROR, __FILE__, __func__, __VA_ARGS__)

#endif /*__DEBUG_H__*/



