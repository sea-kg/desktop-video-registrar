#include <iostream>
#include <vector>

#include <gst/gst.h>

GstElement *pipeline = nullptr;
GstBus *bus = nullptr;
GstMessage *msg = nullptr;

void sigintHandler(int unused) {
	g_print("Sending EoS");
	gst_element_send_event(pipeline, gst_event_new_eos());
    exit(0);
}

int main(int argc, char *argv[]) {

    signal(SIGINT, sigintHandler);
    std::cout << "Webcam Video Registrar" << std::endl;

    gst_init(&argc, &argv);
    GMainLoop *loop = g_main_loop_new(nullptr, FALSE);

    std::string sPipelineCommand = ""
        // "playbin uri=https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm"
        " v4l2src device=/dev/video0 num-buffers=1000"
        " ! videorate rate=1 "
        " ! 'video/x-raw,width=640,height=480,format=YUY2'"
        " ! videoconvert"
        " ! queue ! timeoverlay"
        " ! x264enc tune=zerolatency speed-preset=ultrafast name=encoder"
        " ! 'video/x-h264,width=640,height=480,stream-format=(string)byte-stream,alignment=(string)au,level=(string)3.2, profile=main, interlace-mode=(string)progressive, colorimetry=(string)bt709, chroma-site=(string)mpeg2'"
        " ! h264parse"
        " ! splitmuxsink 'location=out/test_%02d.mp4' max-size-time=60000000000 muxer-factory=matroskamux 'muxer-properties=properties,streamable=true'"
    ;
    std::cout << "Build the pipeline gst-launch-1.0 " << sPipelineCommand << std::endl;

    pipeline = gst_parse_launch(sPipelineCommand.c_str(), nullptr);

    std::cout << "Start playing" << std::endl;
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    // bus = gst_element_get_bus (pipeline);
    // msg = gst_bus_timed_pop_filtered (
    //     bus,
    //     GST_CLOCK_TIME_NONE,
    //     (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS)
    // );

    // std::cout << "See next tutorial for proper error message handling/parsing " << std::endl;
    // if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
    //     g_printerr ("An error occurred! Re-run with the GST_DEBUG=*:WARN "
    //         "environment variable set for more details.\n");
    // }

    std::cout << "Start main loop" << std::endl;
    g_main_loop_run(loop);

    std::cout << "Free resources" << std::endl;
    gst_message_unref (msg);
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    g_main_loop_unref(loop);

    return 0;
}