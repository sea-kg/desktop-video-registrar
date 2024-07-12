#include <iostream>
#include <gst/gst.h>

GstElement *pipeline = nullptr;



void sigintHandler(int unused) {
	g_print("Sending EoS");
	gst_element_send_event(pipeline, gst_event_new_eos());
}

int main() {

    signal(SIGINT, sigintHandler);

    std::cout << "Hello!" << std::endl;
    return 0;
}