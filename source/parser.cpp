/*
* parser.h
*/

#include "parser.h"

Parser::Parser()
{
	printf("Parser init\n");
	m_file = NULL;
}

Parser::~Parser()
{
	printf("Parser exit\n");
}

int Parser::read_file(char *filename, uint8_t *buf)
{
	printf("Parsing input file\n");
	m_file = fopen(filename, "r");
	if (!m_file) {
		printf("Error opening input file: %s\n", filename);
		return -1;
	}

	fseek(m_file, 0, SEEK_END);
	m_file_size = ftell(m_file);
	fseek(m_file, 0, SEEK_SET);

	buf = new uint8_t[m_file_size];
	printf("file size : %lu\n", m_file_size);

	return 0;
}


