/* 
 * Wavelet sharpen GIMP plugin
 * 
 * wavelet-sharpen.c
 * Copyright 2008 by Marco Rossini
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2+
 * as published by the Free Software Foundation.
 * 
 * Instructions:
 * compile with gimptool, eg. 'gimptool-2.0 --install wavelet-sharpen.c'
 */

#include "plugin.h"

/* declare all widgets global to make things a lot easier */

/* colour mode frame */
GtkWidget *lum_toggle;

/* amount/radius frame */
GtkWidget *amount_label[2], *amount_spin[2];
GtkWidget *amount_hbox[2], *amount_vbox, *amount_scale[2];
GtkObject *amount_adj[2];

/* reset buttons */
GtkWidget *reset_button, *button_hbox;

/* dialog */
GtkWidget *dialog, *dialog_vbox;
GtkWidget *preview;
