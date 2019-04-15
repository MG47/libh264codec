/*
* parser.h
*/

#include "parser.h"

Parser::Parser()
{
	DEBUG_PRINT_DEBUG("Parser init");
	m_input_file = NULL;
	m_output_file = NULL;
}

Parser::~Parser()
{
	DEBUG_PRINT_DEBUG("Parser exit");
}

/*
* Read input YUV file
*/
int Parser::read_file(char *filename, uint8_t *buf)
{
	DEBUG_PRINT_INFO("Parsing input file: %s", filename);
	m_input_file = fopen(filename, "r");
	if (!m_input_file) {
		DEBUG_PRINT_ERROR("Error opening input file: %s", filename);
		return -1;
	}

	fseek(m_input_file, 0, SEEK_END);
	m_input_file_size = ftell(m_input_file);
	fseek(m_input_file, 0, SEEK_SET);

	buf = new uint8_t[m_input_file_size];
	DEBUG_PRINT_INFO("file size : %lu", m_input_file_size);

	return 0;
}

/*
* Write decoded frames to output YUV file
* Returns number of bytes written to YUV file
*/
size_t Parser::write_output_file(uint8_t *buf)
{
	size_t bytes_written = 0;
	if (!buf)
		return 0;

	if (!m_output_file) {
		m_output_file = fopen("out.yuv", "w");
		if (!m_output_file) {
			DEBUG_PRINT_ERROR("Error creating output file: %s\n", strerror(errno));
			return 0;
		}
	}
	return 0;
}

