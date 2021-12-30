#!/bin/sh

make
./animation test/model.obj 4 15 2 -15 15
ffmpeg -y -framerate 30 -i "animation_%d.ppm" animation.avi
