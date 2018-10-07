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

static GFile *
file_from_uri_or_path (const gchar *uri_or_path)
{
    g_autofree gchar *scheme = g_uri_parse_scheme (uri_or_path);
    if (scheme != NULL)
        return g_file_new_for_uri (uri_or_path);
    else
        return g_file_new_for_path (uri_or_path);
}

int
main (int argc, char **argv)
{
    gtk_init (&argc, &argv);

    g_autoptr(MbMonitor) monitor = mb_monitor_new ();

    MfWindow *window = mf_window_new (monitor);

    g_autoptr(GFile) f = NULL;
    if (argc > 1) {
        f = file_from_uri_or_path (argv[1]);
    } else {
        GtkWidget *dialog = gtk_file_chooser_dialog_new ("Select Program", NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
                                                         "Select", GTK_RESPONSE_OK, NULL);
        GtkFileFilter *hex_filter = gtk_file_filter_new ();
        gtk_file_filter_set_name (hex_filter, "HEX Files");
        gtk_file_filter_add_mime_type (hex_filter, "application/intel-hex");
        gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), hex_filter);
        GtkFileFilter *all_filter = gtk_file_filter_new ();
        gtk_file_filter_set_name (all_filter, "All Files");
        gtk_file_filter_add_pattern (all_filter, "*");
        gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), all_filter);

        if (gtk_dialog_run (GTK_DIALOG (dialog)) != GTK_RESPONSE_OK)
            return EXIT_SUCCESS;

        f = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));
        gtk_widget_destroy (dialog);
    }

    g_autoptr(MbFile) file = mb_file_new (f);
    mf_window_set_file (window, file);

    gtk_window_present (GTK_WINDOW (window));

    gtk_main ();

    return EXIT_SUCCESS;
}
