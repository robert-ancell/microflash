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

#include <glib/gi18n.h>

#include "mf-program-dialog.h"

struct _MfProgramDialog
{
    GtkFileChooserDialog parent_instance;

    GtkFileFilter *hex_filter;
    GtkFileFilter *all_filter;
};

G_DEFINE_TYPE (MfProgramDialog, mf_program_dialog, GTK_TYPE_FILE_CHOOSER_DIALOG)

void
mf_program_dialog_init (MfProgramDialog *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));

    /* Workarounds for:
     * https://gitlab.gnome.org/GNOME/gtk/merge_requests/376
     * https://gitlab.gnome.org/GNOME/gtk/merge_requests/380
     */
    gtk_file_filter_set_name (self->hex_filter, _("HEX Files"));
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (self), self->hex_filter);
    gtk_file_filter_set_name (self->all_filter, _("All Files"));
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (self), self->all_filter);
}

void
mf_program_dialog_class_init (MfProgramDialogClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class, "/com/github/robert-ancell/microflash/mf-program-dialog.ui");

    gtk_widget_class_bind_template_child (widget_class, MfProgramDialog, hex_filter);
    gtk_widget_class_bind_template_child (widget_class, MfProgramDialog, all_filter);
}

MfProgramDialog *
mf_program_dialog_new (void)
{
    return g_object_new (mf_program_dialog_get_type (), NULL);
}

MbFile *
mf_program_dialog_get_file (MfProgramDialog *self)
{
    g_return_val_if_fail (MF_IS_PROGRAM_DIALOG (self), NULL);

    g_autoptr(GFile) file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (self));
    if (file == NULL)
        return NULL;

    return mb_file_new (file);
}
