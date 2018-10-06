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

#include <gtk/gtk.h>

#include "mb-file.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE (MfWindow, mf_window, MF, WINDOW, GtkWindow)

MfWindow *mf_window_new      (void);

void      mf_window_set_file (MfWindow *window,
                              MbFile   *file);

G_END_DECLS
