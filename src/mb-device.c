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

#include "mb-device.h"

struct _MbDevice
{
    GObject parent_instance;

    GMount *mount;
};

G_DEFINE_TYPE (MbDevice, mb_device, G_TYPE_OBJECT)

void
mb_device_init (MbDevice *self)
{
}

static void
mb_device_dispose (GObject *object)
{
    MbDevice *self = MB_DEVICE (object);

    g_clear_object (&self->mount);

    G_OBJECT_CLASS (mb_device_parent_class)->dispose (object);
}

void
mb_device_class_init (MbDeviceClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = mb_device_dispose;
}

MbDevice *
mb_device_new (GMount *mount)
{
    MbDevice *device = g_object_new (mb_device_get_type (), NULL);
    device->mount = g_object_ref (mount);
    return device;
}

GMount *
mb_device_get_mount (MbDevice *device)
{
    g_return_val_if_fail (MB_IS_DEVICE (device), NULL);
    return device->mount;
}
