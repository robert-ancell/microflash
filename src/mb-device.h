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
#include <gio/gio.h>

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE (MbDevice, mb_device, MB, DEVICE, GObject)

MbDevice *mb_device_new       (GMount   *mount);

GMount   *mb_device_get_mount (MbDevice *device);

G_END_DECLS
