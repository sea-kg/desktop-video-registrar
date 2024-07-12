#!/bin/bash

rm -rf test.mp4


# "num-buffers" defines how many frames will be published by a given element like videotestsrc. After sending "num-buffers", EOS event is published.

# num-buffers=100 expected ~3 sec but sometimes get 8 seconds

gst-launch-1.0 -v v4l2src device=/dev/video0 num-buffers=100 ! videorate \
    ! 'video/x-raw,width=640,height=480,framerate=(fraction)25/1,format=YUY2' \
    ! videoconvert \
    ! x264enc tune=zerolatency speed-preset=ultrafast name=encoder \
    ! 'video/x-h264,width=640,height=480,stream-format=(string)byte-stream,alignment=(string)au,framerate=(fraction)25/1,level=(string)3.2, profile=main, interlace-mode=(string)progressive, colorimetry=(string)bt709, chroma-site=(string)mpeg2' \
    ! h264parse \
    ! mp4mux \
    ! filesink append=false location=test.mp4
