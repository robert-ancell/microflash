#pragma once

#include <glib-object.h>

#include "mb-device.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE (MbMonitor, mb_monitor, MB, MONITOR, GObject)

MbMonitor *mb_monitor_new         (void);

GPtrArray *mb_monitor_get_devices (MbMonitor *monitor);

G_END_DECLS
