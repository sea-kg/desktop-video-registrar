#pragma once

#include "videoregistrar_gst_element.h"

class VideoRegistrarGstElementTimeoverlay : public IVideoRegistrarGstElement {
    public:
        virtual GstElement *createElement() override;
    private:
        std::string TAG{"VideoRegistrarGstElementTimeoverlay"};
        GstElement *m_pElement;
        GDateTime *m_pDateTime;
};