#!/bin/bash

rm -rf test.mp4


# "num-buffers" defines how many frames will be published by a given element like videotestsrc. After sending "num-buffers", EOS event is published.

# num-buffers=100 expected ~3 sec but sometimes get 8 seconds (from web cam coming 12 fps)

gst-launch-1.0 -v v4l2src device=/dev/video0 num-buffers=1000 ! videorate rate=1 max-duplication-time=300000000 \
    ! 'video/x-raw,width=640,height=480,format=YUY2' \
    ! videoconvert \
    ! queue ! timeoverlay \
    ! x264enc tune=zerolatency speed-preset=ultrafast name=encoder \
    ! 'video/x-h264,width=640,height=480,stream-format=(string)byte-stream,alignment=(string)au,level=(string)3.2, profile=main, interlace-mode=(string)progressive, colorimetry=(string)bt709, chroma-site=(string)mpeg2' \
    ! h264parse \
    ! splitmuxsink "location=out/test_%02d.mp4" max-size-time=60000000000 muxer-factory=matroskamux muxer-properties="properties,streamable=true"

#BUG ? max-size-time=60000000000  does not work

## Other you can use repalce the splitmuxsink
# ! mp4mux \
# ! filesink append=false location=test.mp4
