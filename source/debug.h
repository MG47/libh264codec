/*
* debug.h - debug macros
*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "h264_common.h"

#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT_DEBUG(fmt, args...) do { 	\
		printf("DEBUG: %s:%s(): " fmt, 	\
		__FILE__, __func__, ##args); 	\
		printf("\n"); \
	} while(0)
#else
#define DEBUG_PRINT_DEBUG
#endif

#define DEBUG_PRINT_INFO(fmt, args...) do { 	\
		printf("INFO: %s:%s(): " fmt, 	\
		__FILE__, __func__, ##args); 	\
		printf("\n"); \
	} while(0)

#define DEBUG_PRINT_ERROR(fmt, args...) do { 	\
		printf("ERROR: %s:%s(): " fmt, 	\
		__FILE__, __func__, ##args); 	\
		printf("\n"); \
	} while(0)

#endif /*__DEBUG_H__*/



