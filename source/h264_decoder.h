/*
* h264_decoder.h
*/

#ifndef __h264_decoder__
#define __h264_decoder__

#include "parser.h"

class H264_decoder {
	public:
		H264_decoder();
		~H264_decoder();
		bool decode(char *filename);
	private:
		Parser parser;

};

#endif /*__h264_decoder__*/



