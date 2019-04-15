/*
* main.cpp - test libh264codec
*/

#include <iostream>
#include "../source/h264_decoder.h"

void test_decode(char *filename)
{
	int ret;
	H264_decoder dec;

	ret = dec.decode(filename);
	if (ret)
		std::cout << "Failed to decode " << filename << std::endl;
}

int main(int argc, char **argv)
{
	std::cout << "libh264 Test" << std::endl;

	test_decode(argv[1]);

	return 0;
}



