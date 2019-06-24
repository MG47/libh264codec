#!/bin/bash

echo "rm *.yuv libh264dec.so test_decoder"
rm *.yuv libh264dec.so test_decoder
echo "cp ../source/libh264dec.so ."
cp ../source/libh264dec.so .
echo "clang++ test.cpp -o out libh264dec.so -Wl,-rpath,/$PWD"
clang++ test.cpp -o test_decoder libh264dec.so -Wl,-rpath,/$PWD
