/*
* main.cpp - test libh264codec
*/

#include <iostream>
#include "../source/h264_decoder.h"

void test_decode()
{
	int ret;
	std::cout << "Specify h264 file:" << std::endl;
#if 0
	//get input
#else
	#define FILENAME "iamlegend.h264"
#endif

	H264_decoder dec;

	ret = dec.decode(FILENAME);
	if (ret)
		std::cout << "Failed to decode " << FILENAME << std::endl;
}

int main()
{
	std::cout << "libh264 Test" << std::endl;


	test_decode();


	return 0;
}



