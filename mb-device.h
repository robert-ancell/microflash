#pragma once

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE (MbDevice, mb_device, MB, DEVICE, GObject)

MbDevice *mb_device_new       (GMount   *mount);

GMount   *mb_device_get_mount (MbDevice *device);

G_END_DECLS
