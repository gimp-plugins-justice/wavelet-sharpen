/* 
 * Wavelet sharpen GIMP plugin
 * 
 * plugin.c
 * Copyright 2008 by Marco Rossini
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2+
 * as published by the Free Software Foundation.
 */

#include "plugin.h"

GimpPlugInInfo PLUG_IN_INFO = { NULL, NULL, query, run };

MAIN ()
     void query (void)
{
  static GimpParamDef args[] = {
    {GIMP_PDB_INT32, "run-mode", "Run mode"},
    {GIMP_PDB_IMAGE, "image", "Input image"},
    {GIMP_PDB_DRAWABLE, "drawable", "Input drawable"},
    {GIMP_PDB_FLOAT, "amount", "Amount of sharpening"},
    {GIMP_PDB_FLOAT, "radius", "Radius of sharpening"},
    {GIMP_PDB_INT8, "luminance", "Sharpen luminance channel only"}
  };

  gimp_install_procedure ("plug-in-wavelet-sharpen",
			  _("Sharpens the image using wavelets."),
			  _("Sharpens the image using wavelets."),
			  /* Translators: this is [firstname surname] */
			  _("Marco Rossini"),
			  _("Copyright 2008 Marco Rossini"),
			  "2008",
			  _("_Wavelet sharpen ..."),
			  "RGB*, GRAY*",
			  GIMP_PLUGIN, G_N_ELEMENTS (args), 0, args, NULL);

  gimp_plugin_domain_register("gimp20-wavelet-sharpen-plug-in", LOCALEDIR);

  gimp_plugin_menu_register ("plug-in-wavelet-sharpen",
			     "<Image>/Filters/Enhance");
}

wavelet_settings settings = {
  0,				/* sharpen_amount */
  0.5,				/* sharpen_radius */
  TRUE,				/* luminance_only */
  TRUE,				/* show_preview */
  {1.46, 2.94, 1.5},		/* times */
  -1, -1			/* winxsize, winysize */
};

void
run (const gchar * name, gint nparams, const GimpParam * param,
     gint * nreturn_vals, GimpParam ** return_vals)
{
  static GimpParam values[1];
  GimpRunMode run_mode;
  GimpDrawable *drawable;
  gint i;

  bindtextdomain("gimp20-wavelet-sharpen-plug-in", LOCALEDIR);
  textdomain("gimp20-wavelet-sharpen-plug-in");
  bind_textdomain_codeset("gimp20-wavelet-sharpen-plug-in", "UTF-8");

  timer = g_timer_new ();

  /* Setting mandatory output values */
  *nreturn_vals = 1;
  *return_vals = values;
  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = GIMP_PDB_SUCCESS;

  /* restore settings saved in GIMP core */
  gimp_get_data ("plug-in-wavelet-sharpen", &settings);

  drawable = gimp_drawable_get (param[2].data.d_drawable);
  channels = gimp_drawable_bpp (drawable->drawable_id);

  /* allocate buffers */
  /* FIXME: replace by GIMP funcitons */
  for (i = 0; i < channels; i++)
    {
      fimg[i] = (float *) malloc (drawable->width * drawable->height
				  * sizeof (float));
    }
  buffer[1] = (float *) malloc (drawable->width * drawable->height
				* sizeof (float));
  buffer[2] = (float *) malloc (drawable->width * drawable->height
				* sizeof (float));

  /* run GUI if in interactiv mode */
  run_mode = param[0].data.d_int32;
  if (run_mode == GIMP_RUN_INTERACTIVE)
    {
      if (!user_interface (drawable))
	{
	  gimp_drawable_detach (drawable);
	  /* FIXME: should return error status here */
	  return;
	}
    }
  else
    {
      if (nparams == 6)
	{
	  settings.sharpen_amount = param[3].data.d_float;
	  settings.sharpen_radius = param[4].data.d_float;
	  settings.luminance_only = !!(param[5].data.d_int8);
	}
    }

  sharpen (drawable, NULL);

  /* free buffers */
  /* FIXME: replace by GIMP functions */
  for (i = 0; i < channels; i++)
    {
      free (fimg[i]);
    }
  free (buffer[1]);
  free (buffer[2]);

  gimp_displays_flush ();
  gimp_drawable_detach (drawable);

  /* save settings in the GIMP core */
  gimp_set_data ("plug-in-wavelet-sharpen", &settings,
		 sizeof (wavelet_settings));
}
