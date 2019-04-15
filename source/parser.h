/*
* parser.h
*/

#ifndef __PARSER_H__
#define __PARSER_H__

#include "h264_common.h"

class Parser {
	public:
		Parser();
		~Parser();
		int32_t read_file(char *filename, uint8_t *buf);
		size_t write_output_file(uint8_t *buf);
	private:
		FILE *m_input_file;
		FILE *m_output_file;
		size_t m_input_file_size;
};

#endif /*__PARSER_H__*/



