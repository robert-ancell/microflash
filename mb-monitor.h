#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE (MbMonitor, mb_monitor, MB, MONITOR, GObject)

MbMonitor *mb_monitor_new (void);

G_END_DECLS
