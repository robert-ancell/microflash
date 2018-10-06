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

#include "mf-window.h"

struct _MfWindow
{
    GtkWindow     parent_instance;

    GtkHeaderBar *header_bar;
    GtkButton    *flash_button;
    GtkStack     *device_stack;
    GtkLabel     *not_detected_label;
    GtkImage     *device_image;

    GCancellable *cancellable;
    MbMonitor    *monitor;
    MbFile       *file;
    MbDevice     *device;
};

G_DEFINE_TYPE (MfWindow, mf_window, GTK_TYPE_WINDOW)

void
mf_window_init (MfWindow *self)
{
    self->cancellable = g_cancellable_new ();

    gtk_widget_init_template (GTK_WIDGET (self));
}

static void
mf_window_dispose (GObject *object)
{
    MfWindow *self = MF_WINDOW (object);

    g_cancellable_cancel (self->cancellable);

    g_clear_object (&self->cancellable);
    g_clear_object (&self->monitor);
    g_clear_object (&self->file);
    g_clear_object (&self->device);

    G_OBJECT_CLASS (mf_window_parent_class)->dispose (object);
}

static void
flash_copied_cb (GObject *object, GAsyncResult *result, gpointer user_data)
{
    //MfWindow *self = user_data;

    g_autoptr(GError) error = NULL;
    if (!g_file_copy_finish (G_FILE (object), result, &error)) {
        if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
           g_warning ("Failed to flash file: %s", error->message);
        return;
    }
}

static void
flash_cb (MfWindow *self)
{
    g_autofree gchar *basename = g_file_get_basename (mb_file_get_file (self->file));
    g_autoptr(GFile) target = g_file_get_child (g_mount_get_root (mb_device_get_mount (self->device)), basename);

    g_file_copy_async (mb_file_get_file (self->file),
                       target,
                       G_FILE_COPY_NONE,
                       G_PRIORITY_DEFAULT,
                       self->cancellable,
                       NULL, NULL,
                       flash_copied_cb, self);
}

void
mf_window_class_init (MfWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    object_class->dispose = mf_window_dispose;

    gtk_widget_class_set_template_from_resource (widget_class, "/com/github/robert-ancell/microflash/mf-window.ui");

    gtk_widget_class_bind_template_child (widget_class, MfWindow, header_bar);
    gtk_widget_class_bind_template_child (widget_class, MfWindow, flash_button);
    gtk_widget_class_bind_template_child (widget_class, MfWindow, device_stack);
    gtk_widget_class_bind_template_child (widget_class, MfWindow, not_detected_label);
    gtk_widget_class_bind_template_child (widget_class, MfWindow, device_image);
    gtk_widget_class_bind_template_callback (widget_class, flash_cb);
}

static void
devices_changed_cb (MfWindow *self)
{
    g_clear_object (&self->device);
    GPtrArray *devices = mb_monitor_get_devices (self->monitor);
    if (devices->len > 0)
        self->device = g_object_ref (g_ptr_array_index (devices, 0));
    gtk_widget_set_sensitive (GTK_WIDGET (self->flash_button), self->file != NULL && self->device != NULL);
    gtk_stack_set_visible_child (self->device_stack, self->device != NULL ? GTK_WIDGET (self->device_image) : GTK_WIDGET (self->not_detected_label));
}

MfWindow *
mf_window_new (MbMonitor *monitor)
{
    MfWindow *self = g_object_new (mf_window_get_type (), NULL);
    self->monitor = g_object_ref (monitor);
    g_signal_connect_object (monitor, "device-added", G_CALLBACK (devices_changed_cb), self, G_CONNECT_SWAPPED);
    g_signal_connect_object (monitor, "device-removed", G_CALLBACK (devices_changed_cb), self, G_CONNECT_SWAPPED);
    devices_changed_cb (self);
    return self;
}

void
mf_window_set_file (MfWindow *self, MbFile *file)
{
    g_return_if_fail (MF_IS_WINDOW (self));
    g_set_object (&self->file, file);

    const gchar *title = NULL;
    if (self->file != NULL)
        title = mb_file_get_name (self->file);
    gtk_header_bar_set_title (self->header_bar, title);

    devices_changed_cb (self);
}
