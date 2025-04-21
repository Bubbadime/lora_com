#!/bin/bash

ffmpeg \
-f lavfi \
-i anullsrc \
-loop 1 \
-i tmp.jpeg \
-r 25 \
-b:v 4k \
-b:a 1k \
-vcodec libx264 \
-f flv rtmp://a.rtmp.youtube.com/live2/$1

