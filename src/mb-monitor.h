/*
 * Copyright © 2018 Robert Ancell
 * Author: Robert Ancell <robert.ancell@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 */

#pragma once

#include <glib-object.h>

#include "mb-device.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE (MbMonitor, mb_monitor, MB, MONITOR, GObject)

MbMonitor *mb_monitor_new         (void);

GPtrArray *mb_monitor_get_devices (MbMonitor *monitor);

G_END_DECLS
