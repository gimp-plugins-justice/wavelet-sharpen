/* 
 * Wavelet sharpen GIMP plugin
 * 
 * colour.c
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

void
rgb2ycbcr (float *r, float *g, float *b)
{
  /* using JPEG conversion here - expecting all channels to be
   * in [0:255] range */
  static float y, cb, cr;
  y = 0.2990 * *r + 0.5870 * *g + 0.1140 * *b;
  cb = -0.1687 * *r - 0.3313 * *g + 0.5000 * *b + 128.0;
  cr = 0.5000 * *r - 0.4187 * *g - 0.0813 * *b + 128.0;
  *r = y;
  *g = cb;
  *b = cr;
}

void
ycbcr2rgb (float *y, float *cb, float *cr)
{
  /* using JPEG conversion here - expecting all channels to be
   * in [0:255] range */
  static float r, g, b;
  r = *y + 1.40200 * (*cr - 128.0);
  g = *y - 0.34414 * (*cb - 128.0) - 0.71414 * (*cr - 128.0);
  b = *y + 1.77200 * (*cb - 128.0);
  *y = r;
  *cb = g;
  *cr = b;
}
