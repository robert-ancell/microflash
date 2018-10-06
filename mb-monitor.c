#include <gio/gio.h>
#include <gudev/gudev.h>

#include "mb-monitor.h"

struct _MbMonitor
{
    GObject parent_instance;

    GVolumeMonitor *volume_monitor;
    GUdevClient *udev_client;
    GList *devices;
};

G_DEFINE_TYPE (MbMonitor, mb_monitor, G_TYPE_OBJECT)

static gboolean
is_microbit (const gchar *path)
{
    g_autoptr(GUdevClient) client = g_udev_client_new (NULL);
    g_autoptr(GUdevDevice) device = g_udev_client_query_by_device_file (client, path);
    const gchar *bus = g_udev_device_get_property (device, "ID_BUS");
    const gchar *vendor_id = g_udev_device_get_property (device, "ID_VENDOR_ID");
    const gchar *model_id = g_udev_device_get_property (device, "ID_MODEL_ID");
    return g_strcmp0 (bus, "usb") == 0 && g_strcmp0 (vendor_id, "0d28") == 0 && g_strcmp0 (model_id, "0204") == 0;
}

static void
mount_is_microbit (GMount *mount)
{
    g_autoptr(GDrive) drive = g_mount_get_drive (mount);
    g_autoptr(GFile) root = g_mount_get_root (mount);
    g_autofree gchar *root_path = g_file_get_path (root);
    g_autofree gchar *unix_device = g_drive_get_identifier (drive, G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
    g_printerr ("%s (%s %s)\n", g_mount_get_name (mount), root_path, is_microbit (unix_device) ? "true" : "false");
}

static void
mount_added_cb (MbMonitor *self, GMount *mount)
{
    mount_is_microbit (mount);
}

static void
mount_removed_cb (MbMonitor *self, GMount *mount)
{
}

void
mb_monitor_init (MbMonitor *self)
{
    self->volume_monitor = g_volume_monitor_get ();
    g_signal_connect_object (self->volume_monitor, "mount-added", G_CALLBACK (mount_added_cb), self, G_CONNECT_SWAPPED);
    g_signal_connect_object (self->volume_monitor, "mount-removed", G_CALLBACK (mount_removed_cb), self, G_CONNECT_SWAPPED);

    self->udev_client = g_udev_client_new (NULL);

    GList *mounts = g_volume_monitor_get_mounts (self->volume_monitor);
    for (GList *link = mounts; link != NULL; link = link->next) {
        GMount *mount = link->data;
        mount_added_cb (self, mount);
    }
    g_list_free_full (mounts, g_object_unref);
}

static void
mb_monitor_dispose (GObject *object)
{
    MbMonitor *self = MB_MONITOR (object);

    g_clear_object (&self->volume_monitor);
    g_clear_object (&self->udev_client);   

    G_OBJECT_CLASS (mb_monitor_parent_class)->dispose (object);
}

void
mb_monitor_class_init (MbMonitorClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = mb_monitor_dispose;
}

MbMonitor *
mb_monitor_new (void)
{
    return g_object_new (mb_monitor_get_type (), NULL);
}
