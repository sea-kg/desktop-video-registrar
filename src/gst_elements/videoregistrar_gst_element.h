#pragma once

#include <gst/gst.h>
#include <string>

class IVideoRegistrarGstElement {
    public:
        virtual GstElement *createElement() = 0;
};