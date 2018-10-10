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

#include "mb-file.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE (MfProgramDialog, mf_program_dialog, MF, PROGRAM_DIALOG, GtkFileChooserDialog)

MfProgramDialog *mf_program_dialog_new      (void);

MbFile          *mf_program_dialog_get_file (MfProgramDialog *dialog);

G_END_DECLS
