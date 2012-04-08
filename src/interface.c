/* 
 * Wavelet sharpen GIMP plugin
 * 
 * interface.c
 * Copyright 2008 by Marco Rossini
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2+
 * as published by the Free Software Foundation.
 */

#include "plugin.h"
#include "interface.h"

gboolean
user_interface (GimpDrawable * drawable)
{
  gboolean run;

  gimp_ui_init (_("Wavelet sharpen"), FALSE);

  /* prepare the preview */
  preview = gimp_drawable_preview_new (drawable, &settings.show_preview);
  g_signal_connect_swapped (preview, "invalidated", G_CALLBACK (sharpen),
			    drawable);
  gtk_widget_show (preview);

  /* prepare the amount/radius vbox */
  amount_vbox = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (amount_vbox);

  amount_hbox[0] = gtk_hbox_new (FALSE, 10);
  amount_label[0] = gtk_label_new (_("Amount"));
  gtk_misc_set_alignment (GTK_MISC (amount_label[0]), 0.0, 0.0);
  amount_adj[0] =
    gtk_adjustment_new (settings.sharpen_amount, 0, 10, 0.1, 0.1, 0);
  amount_spin[0] =
    gtk_spin_button_new (GTK_ADJUSTMENT (amount_adj[0]), 0.1, 1);
  amount_scale[0] = gtk_hscale_new (GTK_ADJUSTMENT (amount_adj[0]));
  gtk_scale_set_draw_value (GTK_SCALE (amount_scale[0]), FALSE);
  gtk_box_pack_start (GTK_BOX (amount_vbox), amount_hbox[0], FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (amount_hbox[0]), amount_label[0], FALSE, FALSE,
		      0);
  gtk_box_pack_start (GTK_BOX (amount_hbox[0]), amount_scale[0], TRUE, TRUE,
		      0);
  gtk_box_pack_start (GTK_BOX (amount_hbox[0]), amount_spin[0], FALSE, FALSE,
		      0);
  g_signal_connect_swapped (amount_adj[0], "value_changed",
			    G_CALLBACK (gimp_preview_invalidate), preview);
  g_signal_connect (amount_adj[0], "value_changed", G_CALLBACK (set_amount),
		    NULL);
  gtk_widget_show (amount_scale[0]);
  gtk_widget_show (amount_spin[0]);
  gtk_widget_show (amount_label[0]);
  gtk_widget_set_tooltip_text (amount_label[0], TT_AMOUNT);
  gtk_widget_set_tooltip_text (amount_scale[0], TT_AMOUNT);
  gtk_widget_set_tooltip_text (amount_spin[0], TT_AMOUNT);
  gtk_widget_show (amount_hbox[0]);

  amount_hbox[1] = gtk_hbox_new (FALSE, 10);
  amount_label[1] = gtk_label_new (_("Radius"));
  gtk_misc_set_alignment (GTK_MISC (amount_label[1]), 0.0, 0.0);
  amount_adj[1] =
    gtk_adjustment_new (settings.sharpen_radius, 0, 2, 0.01, 0.01, 0);
  amount_spin[1] =
    gtk_spin_button_new (GTK_ADJUSTMENT (amount_adj[1]), 0.01, 2);
  amount_scale[1] = gtk_hscale_new (GTK_ADJUSTMENT (amount_adj[1]));
  gtk_scale_set_draw_value (GTK_SCALE (amount_scale[1]), FALSE);
  gtk_box_pack_start (GTK_BOX (amount_vbox), amount_hbox[1], FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (amount_hbox[1]), amount_label[1], FALSE, FALSE,
		      0);
  gtk_box_pack_start (GTK_BOX (amount_hbox[1]), amount_scale[1], TRUE, TRUE,
		      0);
  gtk_box_pack_start (GTK_BOX (amount_hbox[1]), amount_spin[1], FALSE, FALSE,
		      0);
  g_signal_connect_swapped (amount_adj[1], "value_changed",
			    G_CALLBACK (gimp_preview_invalidate), preview);
  g_signal_connect (amount_adj[1], "value_changed", G_CALLBACK (set_radius),
		    NULL);
  gtk_widget_show (amount_scale[1]);
  gtk_widget_show (amount_spin[1]);
  gtk_widget_show (amount_label[1]);
  gtk_widget_set_tooltip_text (amount_label[1], TT_RADIUS);
  gtk_widget_set_tooltip_text (amount_scale[1], TT_RADIUS);
  gtk_widget_set_tooltip_text (amount_spin[1], TT_RADIUS);
  gtk_widget_show (amount_hbox[1]);

  /* prepare luminance select */
  if (channels > 2)
    {
      lum_toggle = gtk_check_button_new_with_label (_("Sharpen luminance only"));
      if (settings.luminance_only == TRUE)
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (lum_toggle), 1);
      else
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (lum_toggle), 0);
      g_signal_connect (lum_toggle, "toggled",
			G_CALLBACK (set_ycbcr_mode), NULL);
      g_signal_connect_swapped (lum_toggle, "toggled",
				G_CALLBACK (gimp_preview_invalidate),
				preview);
      gtk_widget_set_tooltip_text (lum_toggle, TT_LUMINANCE);
      gtk_widget_show (lum_toggle);
    }
  else
    {
      lum_toggle = NULL;
    }

  /* prepare the reset buttons */
  button_hbox = gtk_hbox_new (FALSE, 10);
  reset_button = gtk_button_new_from_stock (GIMP_STOCK_RESET);
  g_signal_connect (reset_button, "clicked", G_CALLBACK (reset_channel),
		    NULL);
  if (lum_toggle)
    gtk_box_pack_start (GTK_BOX (button_hbox), lum_toggle, TRUE, TRUE, 0);
  gtk_widget_set_tooltip_text (reset_button, TT_RESET);
  gtk_box_pack_start (GTK_BOX (button_hbox), reset_button, FALSE, FALSE, 0);
  gtk_widget_show (reset_button);

  gtk_widget_show (button_hbox);

  /* prepeare the dialog */
  dialog_vbox = gtk_vbox_new (FALSE, 10);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_vbox), 10);
  dialog = gimp_dialog_new (_("Wavelet sharpen"), "wavelet sharpen", NULL, 0,
			    gimp_standard_help_func,
			    "plug-in-wavelet-sharpen", GTK_STOCK_CANCEL,
			    GTK_RESPONSE_CANCEL, GTK_STOCK_OK,
			    GTK_RESPONSE_OK, NULL);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), dialog_vbox);
  gtk_box_pack_start (GTK_BOX (dialog_vbox), preview, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (dialog_vbox), amount_vbox, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (dialog_vbox), button_hbox, FALSE, FALSE, 0);

  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
  if (settings.winxsize > 0 && settings.winysize > 0)
    gtk_window_resize (GTK_WINDOW (dialog), settings.winxsize,
		       settings.winysize);

  gtk_widget_show (dialog_vbox);
  gtk_widget_show (dialog);

  run = (gimp_dialog_run (GIMP_DIALOG (dialog)) == GTK_RESPONSE_OK);

  gtk_window_get_size (GTK_WINDOW (dialog), &(settings.winxsize),
		       &(settings.winysize));

  /* FIXME: destroy all widgets - memory leak! */
  gtk_widget_destroy (dialog);

  return run;
}
