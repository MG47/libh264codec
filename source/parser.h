/*
* parser.h
*/

#ifndef __PARSER__
#define __PARSER__

#include <stdint.h>
#include <stdio.h>

class Parser {
	public:
		Parser();
		~Parser();
		int32_t read_file(char *filename, uint8_t *buf);
	private:
		FILE *m_file;
		size_t m_file_size;
};

#endif /*__PARSER__*/



