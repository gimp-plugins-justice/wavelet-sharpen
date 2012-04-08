/* 
 * Wavelet sharpen GIMP plugin
 * 
 * plugin.h
 * Copyright 2008 by Marco Rossini
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2+
 * as published by the Free Software Foundation.
 */

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <stdlib.h>
#include <math.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>
#include <libintl.h>
#include <locale.h>

#define LOCALEDIR "/usr/share/locale/"

#define _(string) gettext(string)
#define N_(string) string

#define MAX2(x,y) ((x) > (y) ? (x) : (y))
#define MIN2(x,y) ((x) < (y) ? (x) : (y))
#define CLIP(x,min,max) MAX2((min), MIN2((x), (max)))

#define TIMER_READ (1.0 / 10)
#define TIMER_WRITE (1.0 / 7)
#define TIMER_PROCESS (1.0 - TIMER_READ - TIMER_WRITE)

/* Tool-tips */
#define TT_AMOUNT _("Adjusts the amount of sharpening applied.")
#define TT_RADIUS _("Adjusts the radius of the sharpening. For very unsharp images it is recommended to use higher values. Default is 0.5.")
#define TT_LUMINANCE _("Sharpens luminance only (YCbCr luminance channel). This avoids color artifacts to appear and makes the sharpening faster. Colour sharpness in natural images is not critical for the human eye.")
#define TT_RESET _("Resets to the default values.")

void query (void);
void run (const gchar * name, gint nparams, const GimpParam * param,
		 gint * nreturn_vals, GimpParam ** return_vals);
void wavelet_sharpen (float *fimg[3], unsigned int width,
			     unsigned int height, double amount,
			     double radius, float a, float b);
void sharpen (GimpDrawable * drawable, GimpPreview * preview);
void rgb2ycbcr (float *r, float *g, float *b);
void ycbcr2rgb (float *y, float *cb, float *cr);
void set_ycbcr_mode (GtkWidget * w, gpointer data);
void set_amount (GtkWidget * w, gpointer data);
void set_radius (GtkWidget * w, gpointer data);
gboolean user_interface (GimpDrawable * drawable);
void reset_channel (GtkWidget * w, gpointer data);

extern GimpPlugInInfo PLUG_IN_INFO;

typedef struct
{
  double sharpen_amount;
  double sharpen_radius;
  gboolean luminance_only;
  gboolean show_preview;
  float times[3];
  gint winxsize, winysize;
} wavelet_settings;

extern wavelet_settings settings;

float *fimg[4];
float *buffer[3];
gint channels;

GTimer *timer;

#endif /* __PLUGIN_H__ */
