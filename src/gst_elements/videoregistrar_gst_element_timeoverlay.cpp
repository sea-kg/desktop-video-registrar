#include "videoregistrar_gst_element_timeoverlay.h"

#include "wsjcpp_core.h"

GstElement * VideoRegistrarGstElementTimeoverlay::createElement() {
    m_pElement = gst_element_factory_make("timeoverlay", "timeoverlay");
    if (m_pElement == NULL) {
        WsjcppLog::throw_err(TAG, "Could not create 'timeoverlay' element");
        return nullptr;
    }
    m_pDateTime = g_date_time_new_now_local();
    g_object_set(
        G_OBJECT(m_pElement),
        "show-times-as-dates", true,
        "datetime-format", "%Y-%m-%d_%H:%M:%S",
        "datetime-epoch", m_pDateTime,
        NULL
    );
    return m_pElement;
}
