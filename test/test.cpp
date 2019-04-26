/*
* main.cpp - test libh264codec
*/

#include <iostream>
#include "../source/h264_decoder.h"

void test_decode(char *input_filename, char *output_filename)
{
	int ret;
	H264_decoder dec;

	ret = dec.decode(input_filename, output_filename);
	if (ret)
		std::cout << "Failed to decode " << input_filename << std::endl;
}

int main(int argc, char **argv)
{
	std::cout << "libh264 Test" << std::endl;

	if (argc > 3 || argc < 2) {
		std::cout << "Invalid arguments" << std::endl;
		return 0;
	}

	test_decode(argv[1], argv[2]);

	return 0;
}



