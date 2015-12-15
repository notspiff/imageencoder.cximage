/*
 *      Copyright (C) 2012-2013 Team XBMC
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
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */
#include "cximage.h"
#include <algorithm>
#include <string.h>

#include "kodi_imageenc_dll.h"
#include "TextureFormats.h"
#include "libXBMC_addon.h"

ADDON::CHelper_libXBMC_addon* KODI = NULL;
std::string mimetype;

CXImage::CXImage(const std::string& strMimeType): 
  m_strMimeType(strMimeType), m_thumbnailbuffer(NULL)
{
  m_hasAlpha = false;
  memset(&m_image, 0, sizeof(m_image));
}

CXImage::~CXImage()
{
  if (m_thumbnailbuffer)
    FreeMemory(m_thumbnailbuffer);

  ReleaseImage(&m_image);
}

bool CXImage::LoadImageFromMemory(unsigned char* buffer, unsigned int bufSize,
                                  unsigned int width, unsigned int height)
{
  memset(&m_image, 0, sizeof(m_image));

  std::string strExt = m_strMimeType;
  size_t nPos = strExt.find('/');
  if (nPos != std::string::npos)
    strExt.erase(0, nPos + 1);

  if(!::LoadImageFromMemory(buffer, bufSize, strExt.c_str(), width, height, &m_image))
  {
    //CLog::Log(LOGERROR, "Texture manager unable to load image from memory");
    return false;
  }

  m_hasAlpha = NULL != m_image.alpha;
  m_width = m_image.width;
  m_height = m_image.height;
  //m_orientation = m_image.exifInfo.Orientation;
  m_originalWidth = m_image.originalwidth;
  m_originalHeight = m_image.originalheight;
  return true;
}

bool CXImage::Decode(unsigned char* const pixels, unsigned int width,
                     unsigned int height, unsigned int pitch, unsigned int format)
{
  if (m_image.width == 0 || m_image.height == 0)
    return false;

  unsigned int copyWidth = std::min(m_width, width);
  unsigned int copyHeight = std::min(m_height, height);

  unsigned int dstPitch = pitch;
  unsigned int srcPitch = ((m_image.width + 1)* 3 / 4) * 4; // bitmap row length is aligned to 4 bytes

  unsigned char *dst = (unsigned char*)pixels;
  unsigned char *src = m_image.texture + (m_height - 1) * srcPitch;

  for (unsigned int y = 0; y < copyHeight; y++)
  {
    unsigned char *dst2 = dst;
    unsigned char *src2 = src;
    for (unsigned int x = 0; x < copyWidth; x++, dst2 += 4, src2 += 3)
    {
      dst2[0] = src2[0];
      dst2[1] = src2[1];
      dst2[2] = src2[2];
      dst2[3] = 0xff;
    }
    src -= srcPitch;
    dst += dstPitch;
  }

  if(m_image.alpha)
  {
    dst = (unsigned char*)pixels + 3;
    src = m_image.alpha + (m_height - 1) * m_width;

    for (unsigned int y = 0; y < copyHeight; y++)
    {
      unsigned char *dst2 = dst;
      unsigned char *src2 = src;

      for (unsigned int x = 0; x < copyWidth; x++,  dst2+=4, src2++)
        *dst2 = *src2;
      src -= m_width;
      dst += dstPitch;
    }
  }

  ReleaseImage(&m_image);
  memset(&m_image, 0, sizeof(m_image));
  return true;
}

bool CXImage::CreateThumbnailFromSurface(unsigned char* bufferin, unsigned int width, unsigned int height, unsigned int format, unsigned int pitch, const std::string& destFile, 
                                         unsigned char* &bufferout, unsigned int &bufferoutSize)
{
  if (!bufferin) 
    return false;

  bool ret = CreateThumbnailFromSurface2(bufferin, width, height, pitch, destFile.c_str(), m_thumbnailbuffer, bufferoutSize);
  bufferout = m_thumbnailbuffer;
  return ret;
}

void CXImage::ReleaseThumbnailBuffer()
{
  FreeMemory(m_thumbnailbuffer);
  m_thumbnailbuffer = NULL;
}

extern "C"
{
ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  if (!KODI)
    KODI = new ADDON::CHelper_libXBMC_addon;

  if (!KODI->RegisterMe(hdl))
  {
    delete KODI, KODI=NULL;
    return ADDON_STATUS_PERMANENT_FAILURE;
  }

  IMAGEENC_PROPS* prop = static_cast<IMAGEENC_PROPS*>(props);

  mimetype = prop->mimetype;

  return ADDON_STATUS_OK;
}

void ADDON_Stop()
{
}

void ADDON_Destroy()
{
}

ADDON_STATUS ADDON_GetStatus()
{
  return ADDON_STATUS_OK;
}

bool ADDON_HasSettings()
{
  return false;
}

unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  return 0;
}

ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void *value)
{
  return ADDON_STATUS_OK;
}

void ADDON_FreeSettings()
{
}

void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
}

void* LoadImage(unsigned char* buffer, unsigned int bufSize,
                unsigned int* width, unsigned int* height)
{
  CXImage* result = new CXImage(mimetype);

  if (result->LoadImageFromMemory(buffer, bufSize, *width, *height))
  {
    *width = result->m_width;
    *height = result->m_height;
    return result;
  }

  delete result;

  return NULL;
}

bool Decode(void* image, unsigned char* pixels, unsigned int width,
            unsigned int height, unsigned int pitch, unsigned int format)
{
  if (!image)
    return false;
  CXImage* cx = static_cast<CXImage*>(image);

  return cx->Decode(pixels, width, height, pitch, format);
}

bool CreateThumbnail(unsigned char* bufferin,
                     unsigned int width,
                     unsigned int height,
                     unsigned int pitch,
                     unsigned int format,
                     const char* destFile)
{
  CXImage cx(mimetype);

  unsigned char* bufferout;
  unsigned int bufferoutSize;
  return cx.CreateThumbnailFromSurface(bufferin, width, height, format, pitch, destFile, bufferout, bufferoutSize); 
}

void Close(void* image)
{
  delete static_cast<CXImage*>(image);
}

}
