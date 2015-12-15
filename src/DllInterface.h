#pragma once

/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <stdint.h>

 #ifdef LoadImage
  #undef LoadImage
 #endif

#define EXIF_MAX_COMMENT 1000

struct ImageInfo
{
  unsigned int width;
  unsigned int height;
  unsigned int originalwidth;
  unsigned int originalheight;
  //struct tag_ExifInfo exifInfo;
  unsigned char* texture;
  void* context;
  unsigned char* alpha;
};

bool ReleaseImage(ImageInfo *);
bool LoadImage(const char *, unsigned int, unsigned int, ImageInfo *);
bool LoadImageFromMemory(const uint8_t*, unsigned int, const char *, unsigned int, unsigned int, ImageInfo *);
bool CreateThumbnailFromSurface(unsigned char *, unsigned int, unsigned int, unsigned int, const char *);
bool CreateThumbnailFromSurface2(unsigned char *, unsigned int, unsigned int, unsigned int, const char *, unsigned char* &, unsigned int&);
void FreeMemory(void*);
