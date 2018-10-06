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
    GtkWindow parent_instance;

    GtkWidget *header_bar;
    GtkWidget *flash_button;

    MbFile *file;
};

G_DEFINE_TYPE (MfWindow, mf_window, GTK_TYPE_WINDOW)

void
mf_window_init (MfWindow *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));
}

static void
mf_window_dispose (GObject *object)
{
    MfWindow *self = MF_WINDOW (object);

    G_OBJECT_CLASS (mf_window_parent_class)->dispose (object);
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
}

MfWindow *
mf_window_new (void)
{
    return g_object_new (mf_window_get_type (), NULL);
}

void
mf_window_set_file (MfWindow *window, MbFile *file)
{
    g_return_if_fail (MF_IS_WINDOW (window));
    g_set_object (&window->file, file);

    const gchar *title = NULL;
    if (window->file != NULL)
        title = mb_file_get_name (window->file);
    gtk_header_bar_set_title (window->header_bar, title);
}
