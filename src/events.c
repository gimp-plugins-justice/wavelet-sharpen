/* 
 * Wavelet sharpen GIMP plugin
 * 
 * events.c
 * Copyright 2008 by Marco Rossini
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2+
 * as published by the Free Software Foundation.
 */

#include "plugin.h"
#include "interface.h"

void
reset_channel (GtkWidget * w, gpointer data)
{
  settings.sharpen_amount = 0;
  settings.sharpen_radius = 0.5;
  gtk_adjustment_set_value (GTK_ADJUSTMENT (amount_adj[0]), 0);
  gtk_adjustment_set_value (GTK_ADJUSTMENT (amount_adj[1]), 0.5);
}

void
set_ycbcr_mode (GtkWidget * w, gpointer data)
{
  settings.luminance_only =
    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w));
}

void
set_amount (GtkWidget * w, gpointer data)
{
  double val;
  val = gtk_adjustment_get_value (GTK_ADJUSTMENT (w));
  settings.sharpen_amount = val;
}

void
set_radius (GtkWidget * w, gpointer data)
{
  double val;
  val = gtk_adjustment_get_value (GTK_ADJUSTMENT (w));
  settings.sharpen_radius = val;
}
