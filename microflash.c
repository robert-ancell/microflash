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

#include <gtk/gtk.h>

#include "mb-monitor.h"
#include "mb-file.h"
#include "mf-window.h"

static void
device_added_cb (MbMonitor *monitor, MbDevice *device)
{
    g_printerr ("+microbit\n");
}

static void
device_removed_cb (MbMonitor *monitor, MbDevice *device)
{
    g_printerr ("-microbit\n");
}

int
main (int argc, char **argv)
{
    gtk_init (&argc, &argv);

    MfWindow *window = mf_window_new ();

    if (argc > 1) {
        g_autoptr(GFile) f = g_file_new_for_path (argv[1]);
        g_autoptr(MbFile) file = mb_file_new (f);
        mf_window_set_file (window, file);
    }

    g_autoptr(MbMonitor) monitor = mb_monitor_new ();
    g_signal_connect (monitor, "device-added", G_CALLBACK (device_added_cb), NULL);
    g_signal_connect (monitor, "device-removed", G_CALLBACK (device_removed_cb), NULL);
    GPtrArray *devices = mb_monitor_get_devices (monitor);
    for (int i = 0; i < devices->len; i++) {
        MbDevice *device = g_ptr_array_index (devices, i);
        device_added_cb (monitor, device);
    }

    gtk_window_present (GTK_WINDOW (window));

    gtk_main ();

    return EXIT_SUCCESS;
}
