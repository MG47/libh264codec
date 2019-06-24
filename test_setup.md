Testing Instructions : v0.1
===========================

# Build the library and the test program

	* Sync libh264codec repository
	git clone https://github.com/MG47/libh264codec.git
	cd libh264codec/

	* Directory structure

	libh264codec/
	|--source 	(codec libraries)
	|--test		(test programs)

	* Build the decoder library (libh264decode.so)
	cd source/
	make clean && make

	* Build the test program 'test_decoder'
	cd test/
	build.sh

# Media setup

	Test video used: 'Big Buck Bunny' movie: https://peach.blender.org/download/

	* Original movie: bbb, 4KUHD, 60FPS
	* Trim the movie to 1min
	ffmpeg -i bbb_4kuhd60.mp4 -ss 00:00:00 -t 00:01:00 -async 1 -c copy bbb_trim_4kuhd60.mp4

	* Downscale to SQCIF resolution (128x96)
	ffmpeg -i bbb_trim_4kuhd60.mp4 -vf scale=128:96 bbb_trim_sqcif60.mp4

	* Get Raw YUV420P from mp4
	ffmpeg -i bbb_trim_sqcif60.mp4 -s sqcif -pix_fmt yuv420p bbb_trim_sqcif60.yuv

	* Encode Raw YUV in I_PCM format

# Test decoder (libh264decode)

	* Decode using libh264decode
	* <test_decoder> <input_h264_filename> <output_yuv_filename>
	cd test/
	./test_decoder bbb_trim_sqcif60_ipcm.h264 bbb_trim_sqcif60_output.yuv

	* Play decoded YUV using VLC
	vlc --rawvid-fps 60 --rawvid-width 128 --rawvid-height 96 --rawvid-chroma I420 bbb_trim_sqcif60_output.yuv

