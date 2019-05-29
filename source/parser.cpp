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
* Open input bitstream file
*/
uint32_t Parser::open_file(char *in_file, char *out_file)
{
	DEBUG_PRINT_INFO("Parsing input file: %s", in_file);
	m_input_file = fopen(in_file, "r");
	if (!m_input_file) {
		DEBUG_PRINT_ERROR("Error opening input file: %s", in_file);
		return 0;
	}

	fseek(m_input_file, 0, SEEK_END);
	m_input_file_size = ftell(m_input_file);
	fseek(m_input_file, 0, SEEK_SET);
	DEBUG_PRINT_INFO("Input file size : %lu", m_input_file_size);

	// TODO write output filename after with session info suffices
	if (!m_output_file) {
		m_output_file = fopen(((out_file) ? out_file : "out.yuv"), "w");
		if (!m_output_file) {
			DEBUG_PRINT_ERROR("Error creating output file: %s", strerror(errno));
			return 0;
		}
	}
	DEBUG_PRINT_INFO("Ouptut file Name : %s", (out_file) ? out_file : "out.yuv");

	return m_input_file_size;
}

/*
* Write decoded frames to output YUV file
* Returns number of bytes written to YUV file
*/
size_t Parser::write_output_file(uint8_t *buf, uint32_t len)
{
	size_t bytes_written = 0;
	if (!buf)
		return 0;

	bytes_written = fwrite(buf, 1, len, m_output_file);
	if (!bytes_written)
		DEBUG_PRINT_ERROR("Error writing to output file: %s", strerror(errno));

	return bytes_written;
}


/*
* Read one NAL unit from input file
*/
int32_t Parser::get_nalu(uint8_t *buf)
{
	size_t cur_pos = ftell(m_input_file);
	// 1000 for now
	uint32_t code = 0, i = 0, bytes_read = 0;
	uint8_t current_byte = 0;

	if (!buf) {
		DEBUG_PRINT_ERROR("Invalid buffer");
		return -1;
	}

	if (fread(buf, 4, 1, m_input_file) != 1) {
		if (!feof(m_input_file)) {
			DEBUG_PRINT_ERROR("Error reading input file: %s", strerror(errno));
			goto bailout;
		}
		return 0;
	}

	if ((buf[0] << 24 | buf[1] << 16  | buf[2] << 8 | buf[3]) != 0x01) {
		DEBUG_PRINT_DEBUG("Invalid start code");
		goto bailout;
	}

	while (1) {
		if (fread(&current_byte, 1, 1, m_input_file) != 1) {
			if (!feof(m_input_file)) {
				DEBUG_PRINT_ERROR("Error reading input file: %s", strerror(errno));
				goto bailout;
			}
			return 0;
		}
		*buf++ = current_byte;
		bytes_read++;

		// if this is the start code, exit the loop
		code = code << 8 | current_byte;
		if (code == 0x01) {
			bytes_read -= 4;
			fseek(m_input_file, -4, SEEK_CUR);
			return bytes_read;
		}

		// TODO refine this code
		// remove emulation prevention byte
		if (code == 0x0301 || code == 0x0302 || code == 0x0303) {
			DEBUG_PRINT_DEBUG("emulation prevention byte detected, removing");
			*(buf - 2) = 0;
		}

	}

	return bytes_read;
bailout:
	return -1;
}

