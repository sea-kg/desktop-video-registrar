#include <iostream>
#include <vector>
#include <stdlib.h>

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
    setenv("GST_DEBUG", "*:WARN", true);

    std::cout << "Welcome to Video Registrar" << std::endl;

    // Basicly ised code from here:
    // https://stackoverflow.com/questions/59381362/v4l2src-simple-pipeline-to-c-application

    // GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;

    gst_init(&argc, &argv);

    GstElement *source = gst_element_factory_make("v4l2src", "source");
    // g_return_if_fail (source != NULL);
    if (source == NULL) {
        std::cerr << "Could not create 'v4l2src' element" << std::endl;
        return -1;
    }
    g_object_set(
        source,
        "device", "/dev/video0", // Optional
        "num-buffers", 1000,  // -1 - infinity, 1000 - frist 1000 frames
        NULL
    );

    GstElement *videorate = gst_element_factory_make("videorate", "rate"); // without this elemrnt will be not work
    if (videorate == NULL) {
        std::cerr << "Could not create 'videorate' element" << std::endl;
        return -1;
    }
    g_object_set(
        G_OBJECT(videorate),
        "max-rate", 30,
        // "rate", 1, // Segmentation fault, wtf?, but default value is 1
        NULL
    );

    GstElement *capsfilter = gst_element_factory_make("capsfilter","filter");
    if (capsfilter == NULL) {
        std::cerr << "Could not create 'capsfilter' element" << std::endl;
        return -1;
    }
    GstCaps *caps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "YUY2", "width", G_TYPE_INT, 640, "height", G_TYPE_INT, 480,  NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);

    GstElement *videoconvert = gst_element_factory_make("videoconvert", "convert");
     if (videoconvert == NULL) {
        std::cerr << "Could not create 'videoconvert' element" << std::endl;
        return -1;
    }

    GstElement *queue = gst_element_factory_make("queue", "queue");
    if (queue == NULL) {
        std::cerr << "Could not create 'queue' element" << std::endl;
        return -1;
    }

    GstElement *timeoverlay = gst_element_factory_make("timeoverlay", "timeoverlay");
    if (timeoverlay == NULL) {
        std::cerr << "Could not create 'timeoverlay' element" << std::endl;
        return -1;
    }
    GDateTime *dt = g_date_time_new_now_local();
    g_object_set(
        G_OBJECT(timeoverlay),
        "show-times-as-dates", true,
        "datetime-format", "%Y-%m-%d_%H:%M:%S",
        "datetime-epoch", dt,
        NULL
    );

    GstElement *x264enc = gst_element_factory_make("x264enc", "x264enc");
    if (x264enc == NULL) {
        std::cerr << "Could not create 'x264enc' element" << std::endl;
        return -1;
    }
    g_object_set (
        G_OBJECT(x264enc),
        "tune", 0x00000004,  // zerolatency, https://gstreamer.freedesktop.org/documentation/x264/index.html?gi-language=c#GstX264EncTune
        "name", "encoder",
        "speed-preset", 1,  // ultrafast, https://gstreamer.freedesktop.org/documentation/x264/index.html?gi-language=c#GstX264EncPreset
        NULL
    );
    GstElement *capsfilter_enc = gst_element_factory_make("capsfilter","capsfilter_enc");
    if (capsfilter_enc == NULL) {
        std::cerr << "Could not create 'capsfilter_enc' element" << std::endl;
        return -1;
    }
    GstCaps *caps_enc = gst_caps_new_simple(
        "video/x-h264",
        "width", G_TYPE_INT, 640,
        "height", G_TYPE_INT, 480,
        "stream-format", G_TYPE_STRING, "byte-stream",
        "alignment", G_TYPE_STRING, "au",
        "level", G_TYPE_STRING, "3.2",
        "profile", G_TYPE_STRING, "main",
        "interlace-mode", G_TYPE_STRING, "progressive",
        "colorimetry", G_TYPE_STRING, "bt709",
        "chroma-site", G_TYPE_STRING, "mpeg2",
        NULL
    );
    g_object_set(G_OBJECT(capsfilter_enc), "caps", caps_enc, NULL);

    GstElement *h264parse = gst_element_factory_make("h264parse","h264parse");
    if (h264parse == NULL) {
        std::cerr << "Could not create 'h264parse' element" << std::endl;
        return -1;
    }

    GstElement *splitmuxsink = gst_element_factory_make("splitmuxsink", "splitmuxsink");
    g_object_set (
        G_OBJECT(splitmuxsink),
        "location", "out/test_%02d.mp4",
        "muxer-factory", "matroskamux",
        // "muxer-properties", "properties,streamable=true", // GLib-ERROR **: 18:57:11.600: ../../../glib/gmem.c:167: failed to allocate 60130373416 bytes
        "max-size-time", 60000000000L,
        NULL
    );

    pipeline = gst_pipeline_new("pipe");

    gst_bin_add(GST_BIN(pipeline), source);
    gst_bin_add(GST_BIN(pipeline), videorate);
    gst_bin_add(GST_BIN(pipeline), capsfilter);
    gst_bin_add(GST_BIN(pipeline), videoconvert);
    gst_bin_add(GST_BIN(pipeline), queue);
    gst_bin_add(GST_BIN(pipeline), timeoverlay);
    gst_bin_add(GST_BIN(pipeline), x264enc);
    gst_bin_add(GST_BIN(pipeline), capsfilter_enc);
    gst_bin_add(GST_BIN(pipeline), h264parse);
    gst_bin_add(GST_BIN(pipeline), splitmuxsink);

    gst_element_link(source, videorate);
    gst_element_link(videorate, capsfilter);
    gst_element_link(capsfilter, videoconvert);
    gst_element_link(videoconvert, queue);
    gst_element_link(queue, timeoverlay);
    gst_element_link(timeoverlay, x264enc);
    gst_element_link(x264enc, capsfilter_enc);
    gst_element_link(capsfilter_enc, h264parse);
    gst_element_link(h264parse, splitmuxsink);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    // Parse message
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE (msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error (msg, &err, &debug_info);
                g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
                g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
                g_clear_error (&err);
                g_free (debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print ("End-Of-Stream reached.\n");
                break;
            default:
                // We should not reach here because we only asked for ERRORs and EOS
                g_printerr ("Unexpected message received.\n");
                break;
        }
        gst_message_unref (msg);
    }

    // Free resources
    gst_object_unref(bus);

    gst_element_set_state(pipeline,GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}