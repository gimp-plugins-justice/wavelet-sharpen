/* 
 * Wavelet sharpen GIMP plugin
 * 
 * sharpen.c
 * Copyright 2008 by Marco Rossini
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2+
 * as published by the Free Software Foundation.
 */

#include "plugin.h"

void
sharpen (GimpDrawable * drawable, GimpPreview * preview)
{
  GimpPixelRgn rgn_in, rgn_out;
  gint i, x1, y1, x2, y2, width, height, x, c;
  guchar *line;
  float val[4], times[3], totaltime;
  int channels_sharpened;

  if (preview)
    {
      gimp_preview_get_position (preview, &x1, &y1);
      gimp_preview_get_size (preview, &width, &height);
      x2 = x1 + width;
      y2 = y1 + height;
    }
  else
    {
      gimp_drawable_mask_bounds (drawable->drawable_id, &x1, &y1, &x2, &y2);
      width = x2 - x1;
      height = y2 - y1;
    }

  gimp_pixel_rgn_init (&rgn_in, drawable, x1, y1, width, height, FALSE,
		       FALSE);
  gimp_pixel_rgn_init (&rgn_out, drawable, x1, y1, width, height,
		       preview == NULL, TRUE);

  /* cache some tiles to make reading/writing faster */
  gimp_tile_cache_ntiles (drawable->width / gimp_tile_width () + 1);

  totaltime = settings.times[0];
  /* FIXME: exclude alpha channel */
  if (settings.luminance_only != TRUE)
    {
      if (channels % 2 == 0)
	totaltime += (channels - 1) * settings.times[1];
      else
	totaltime += channels * settings.times[1];
    }
  else
    totaltime += settings.times[1];
  totaltime += settings.times[2];


  /* FIXME: replace by GIMP functions */
  line = (guchar *) malloc (channels * width * sizeof (guchar));

  /* read the full image from GIMP */
  if (!preview)
    gimp_progress_init (_("Wavelet sharpening..."));
  times[0] = g_timer_elapsed (timer, NULL);
  for (i = 0; i < y2 - y1; i++)
    {
      if (!preview && i % 10 == 0)
	gimp_progress_update (settings.times[0] * i
			      / (double) height / totaltime);
      gimp_pixel_rgn_get_row (&rgn_in, line, x1, i + y1, width);

      /* convert pixel values to float and do colour model conv. */
      for (x = 0; x < width; x++)
	{
	  for (c = 0; c < channels; c++)
	    val[c] = (float) line[x * channels + c];
	  if (channels > 2 && settings.luminance_only == TRUE)
	    rgb2ycbcr (&(val[0]), &(val[1]), &(val[2]));
	  /* save pixel values and scale for sharpening */
	  for (c = 0; c < channels; c++)
	    fimg[c][i * width + x] = val[c] / 255.0;
	  /* FIXME: do gamma correction */
	}
    }
  times[0] = g_timer_elapsed (timer, NULL) - times[0];

  /* sharpen the channels individually */
  times[1] = g_timer_elapsed (timer, NULL);
  /* FIXME: variable abuse (x) */
  channels_sharpened = 0;
  /* FIXME: do not sharpen alpha channel */
  for (c = 0; c < ((channels % 2) ? channels : channels - 1); c++)
    {
      double a, b;
      if (settings.luminance_only == TRUE && c > 0)
	continue;
      buffer[0] = fimg[c];
      b = preview ? 0.0 : settings.times[1] / totaltime;
      a = settings.times[0] + channels_sharpened * settings.times[1];
      a /= totaltime;
      if (settings.sharpen_amount > 0)
	{
	  wavelet_sharpen (buffer, width, height,
			   settings.sharpen_amount,
			   settings.sharpen_radius, a, b);
	  channels_sharpened++;
	}
    }
  times[1] = g_timer_elapsed (timer, NULL) - times[1];
  times[1] /= channels_sharpened;

  /* retransform the image data */
  for (c = 0; c < channels; c++)
    for (i = 0; i < width * height; i++)
      fimg[c][i] = fimg[c][i] * 255;

  /* set alpha to full opacity in single channel preview mode */
  if (preview && channels % 2 == 0)
    for (i = 0; i < width * height; i++)
      fimg[channels - 1][i] = 255;

  /* convert to RGB if necessary */
  if (channels > 2 && settings.luminance_only == TRUE)
    for (i = 0; i < width * height; i++)
      ycbcr2rgb (&(fimg[0][i]), &(fimg[1][i]), &(fimg[2][i]));

  /* clip the values */
  for (c = 0; c < channels; c++)
    {
      for (i = 0; i < width * height; i++)
	{
	  fimg[c][i] = CLIP (fimg[c][i], 0, 255);
	}
    }

  /* write the image back to GIMP */
  times[2] = g_timer_elapsed (timer, NULL);
  for (i = 0; i < height; i++)
    {
      if (!preview)
	gimp_progress_update ((settings.times[0] + channels_sharpened
			       * settings.times[1] + settings.times[2]
			       * i / (double) height) / totaltime);

      /* convert back to guchar */
      for (c = 0; c < channels; c++)
	for (x = 0; x < width; x++)
	  /* avoiding rounding errors !!! */
	  line[x * channels + c] = (guchar) (fimg[c][i * width + x] + 0.5);
      gimp_pixel_rgn_set_row (&rgn_out, line, x1, i + y1, width);
    }
  times[2] = g_timer_elapsed (timer, NULL) - times[2];

  if (!preview)
    {
      settings.times[0] = times[0];
      if (channels_sharpened > 0)
	settings.times[1] = times[1];
      settings.times[2] = times[2];
    }

  /* FIXME: replace by gimp functions */
  free (line);

  if (preview)
    {
      gimp_drawable_preview_draw_region (GIMP_DRAWABLE_PREVIEW (preview),
					 &rgn_out);
      return;
    }
  gimp_drawable_flush (drawable);
  gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
  gimp_drawable_update (drawable->drawable_id, x1, y1, width, height);
}
