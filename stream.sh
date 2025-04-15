#!/bin/bash

ffmpeg \
-f lavfi \
-i anullsrc \
-loop 1 \
-i tmp.jpeg \
-b:v 3k \
-b:a 1k \
-f flV rtmp://x.rtmp.youtube.com/live2/$1

