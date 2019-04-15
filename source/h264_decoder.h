/*
* h264_decoder.h
*/

#ifndef __H264_DECODER_H__
#define __H264_DECODER_H__

#include "h264_common.h"
#include "parser.h"

class H264_decoder {
	public:
		H264_decoder();
		~H264_decoder();
		bool decode(char *filename);
	private:
		Parser parser;

};

#endif /*__H264_DECODER_H__*/



