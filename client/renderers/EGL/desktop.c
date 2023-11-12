/**
 * Looking Glass
 * Copyright © 2017-2023 The Looking Glass Authors
 * https://looking-glass.io
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "desktop.h"
#include "common/debug.h"
#include "common/option.h"
#include "common/locking.h"
#include "common/array.h"

#include "app.h"
#include "texture.h"
#include "shader.h"
#include "desktop_rects.h"
#include "cimgui.h"

#include <stdlib.h>
#include <string.h>

// these headers are auto generated by cmake
#include "desktop.vert.h"
#include "desktop_rgb.frag.h"
#include "desktop_rgb.def.h"

#include "postprocess.h"
#include "filters.h"

struct DesktopShader
{
  EGL_Shader * shader;
  GLint uTransform;
  GLint uDesktopSize;
  GLint uSamplerType;
  GLint uScaleAlgo;
  GLint uNVGain;
  GLint uCBMode;
  GLint uIsHDR;
  GLint uMapHDRtoSDR;
  GLint uMapHDRGain;
  GLint uMapHDRPQ;
};

struct EGL_Desktop
{
  EGL * egl;
  EGLDisplay * display;

  EGL_Texture          * texture;
  struct DesktopShader dmaShader, shader;
  EGL_DesktopRects     * mesh;
  CountedBuffer        * matrix;

  // internals
  int               width, height;
  bool              hdr;
  bool              hdrPQ;
  LG_RendererRotate rotate;

  bool useSpice;
  int spiceWidth, spiceHeight;
  EGL_Texture * spiceTexture;

  // scale algorithm
  int scaleAlgo;

  // night vision
  int nvMax;
  int nvGain;

  // colorblind mode
  int cbMode;

  bool useDMA;
  LG_RendererFormat format;

  // map HDR content to SDR
  bool  mapHDRtoSDR;
  int   peakLuminance;
  int   maxCLL;

  EGL_PostProcess * pp;
  _Atomic(bool) processFrame;
};

// forwards
void toggleNV(int key, void * opaque);

static bool egl_initDesktopShader(
  struct DesktopShader * shader,
  const char * vertex_code  , size_t vertex_size,
  const char * fragment_code, size_t fragment_size,
  bool useDMA
)
{
  if (!egl_shaderInit(&shader->shader))
    return false;

  if (!egl_shaderCompile(shader->shader,
        vertex_code  , vertex_size,
        fragment_code, fragment_size,
        useDMA, NULL))
  {
    return false;
  }

  shader->uDesktopSize  = egl_shaderGetUniform(shader->shader, "desktopSize" );
  shader->uTransform    = egl_shaderGetUniform(shader->shader, "transform"   );
  shader->uScaleAlgo    = egl_shaderGetUniform(shader->shader, "scaleAlgo"   );
  shader->uNVGain       = egl_shaderGetUniform(shader->shader, "nvGain"      );
  shader->uCBMode       = egl_shaderGetUniform(shader->shader, "cbMode"      );
  shader->uIsHDR        = egl_shaderGetUniform(shader->shader, "isHDR"       );
  shader->uMapHDRtoSDR  = egl_shaderGetUniform(shader->shader, "mapHDRtoSDR" );
  shader->uMapHDRGain   = egl_shaderGetUniform(shader->shader, "mapHDRGain"  );
  shader->uMapHDRPQ     = egl_shaderGetUniform(shader->shader, "mapHDRPQ"    );

  return true;
}

bool egl_desktopInit(EGL * egl, EGL_Desktop ** desktop_, EGLDisplay * display,
    bool useDMA, int maxRects)
{
  EGL_Desktop * desktop = calloc(1, sizeof(EGL_Desktop));
  if (!desktop)
  {
    DEBUG_ERROR("Failed to malloc EGL_Desktop");
    return false;
  }
  *desktop_ = desktop;

  desktop->egl     = egl;
  desktop->display = display;

  if (!egl_textureInit(&desktop->texture, display,
        useDMA ? EGL_TEXTYPE_DMABUF : EGL_TEXTYPE_FRAMEBUFFER))
  {
    DEBUG_ERROR("Failed to initialize the desktop texture");
    return false;
  }

  if (!egl_desktopRectsInit(&desktop->mesh, maxRects))
  {
    DEBUG_ERROR("Failed to initialize the desktop mesh");
    return false;
  }

  desktop->matrix = countedBufferNew(6 * sizeof(GLfloat));
  if (!desktop->matrix)
  {
    DEBUG_ERROR("Failed to allocate the desktop matrix buffer");
    return false;
  }

  if (!egl_initDesktopShader(
    &desktop->shader,
    b_shader_desktop_vert    , b_shader_desktop_vert_size,
    b_shader_desktop_rgb_frag, b_shader_desktop_rgb_frag_size,
    false))
  {
    DEBUG_ERROR("Failed to initialize the desktop shader");
    return false;
  }

  if (useDMA)
    if (!egl_initDesktopShader(
      &desktop->dmaShader,
      b_shader_desktop_vert    , b_shader_desktop_vert_size,
      b_shader_desktop_rgb_frag, b_shader_desktop_rgb_frag_size,
      true))
    {
      DEBUG_ERROR("Failed to initialize the desktop DMA shader");
      return false;
    }

  app_registerKeybind(0, 'N', toggleNV, desktop,
      "Toggle night vision mode");

  desktop->nvMax     = option_get_int("egl", "nvGainMax");
  desktop->nvGain    = option_get_int("egl", "nvGain"   );
  desktop->cbMode    = option_get_int("egl", "cbMode"   );
  desktop->scaleAlgo = option_get_int("egl", "scale"    );
  desktop->useDMA    = useDMA;

  desktop->mapHDRtoSDR   = option_get_bool("egl", "mapHDRtoSDR"  );
  desktop->peakLuminance = option_get_int ("egl", "peakLuminance");
  desktop->maxCLL        = option_get_int ("egl", "maxCLL"       );

  if (!egl_postProcessInit(&desktop->pp))
  {
    DEBUG_ERROR("Failed to initialize the post process manager");
    return false;
  }

  // this MUST be first
  egl_postProcessAdd(desktop->pp, &egl_filter24bitOps);

  egl_postProcessAdd(desktop->pp, &egl_filterDownscaleOps);
  egl_postProcessAdd(desktop->pp, &egl_filterFFXCASOps   );
  egl_postProcessAdd(desktop->pp, &egl_filterFFXFSR1Ops  );
  return true;
}

void toggleNV(int key, void * opaque)
{
  EGL_Desktop * desktop = (EGL_Desktop *)opaque;
  if (desktop->nvGain++ == desktop->nvMax)
    desktop->nvGain = 0;

       if (desktop->nvGain == 0) app_alert(LG_ALERT_INFO, "NV Disabled");
  else if (desktop->nvGain == 1) app_alert(LG_ALERT_INFO, "NV Enabled");
  else app_alert(LG_ALERT_INFO, "NV Gain + %d", desktop->nvGain - 1);

  app_invalidateWindow(true);
}

bool egl_desktopScaleValidate(struct Option * opt, const char ** error)
{
  if (opt->value.x_int >= 0 && opt->value.x_int < EGL_SCALE_MAX)
    return true;

  *error = "Invalid scale algorithm number";
  return false;
}

void egl_desktopFree(EGL_Desktop ** desktop)
{
  if (!*desktop)
    return;

  egl_textureFree    (&(*desktop)->texture         );
  egl_textureFree    (&(*desktop)->spiceTexture    );
  egl_shaderFree     (&(*desktop)->shader   .shader);
  egl_shaderFree     (&(*desktop)->dmaShader.shader);
  egl_desktopRectsFree(&(*desktop)->mesh           );
  countedBufferRelease(&(*desktop)->matrix         );

  egl_postProcessFree(&(*desktop)->pp);

  free(*desktop);
  *desktop = NULL;
}

static const char * algorithmNames[EGL_SCALE_MAX] = {
  [EGL_SCALE_AUTO]    = "Automatic (downscale: linear, upscale: nearest)",
  [EGL_SCALE_NEAREST] = "Nearest",
  [EGL_SCALE_LINEAR]  = "Linear",
};

void egl_desktopConfigUI(EGL_Desktop * desktop)
{
  igText("Scale algorithm:");
  igPushItemWidth(igGetWindowWidth() - igGetStyle()->WindowPadding.x * 2);
  if (igBeginCombo("##scale", algorithmNames[desktop->scaleAlgo], 0))
  {
    for (int i = 0; i < EGL_SCALE_MAX; ++i)
    {
      bool selected = i == desktop->scaleAlgo;
      if (igSelectable_Bool(algorithmNames[i], selected, 0,
            (ImVec2) { 0.0f, 0.0f }))
        desktop->scaleAlgo = i;
      if (selected)
        igSetItemDefaultFocus();
    }
    igEndCombo();
  }
  igPopItemWidth();

  igText("Night vision mode:");
  igSameLine(0.0f, -1.0f);
  igPushItemWidth(igGetWindowWidth() - igGetCursorPosX() - igGetStyle()->WindowPadding.x);

  const char * format;
  switch (desktop->nvGain)
  {
    case 0: format = "off"; break;
    case 1: format = "on";  break;
    default: format = "gain: %d";
  }
  igSliderInt("##nvgain", &desktop->nvGain, 0, desktop->nvMax, format, 0);
  igPopItemWidth();

  bool mapHDRtoSDR   = desktop->mapHDRtoSDR;
  int  peakLuminance = desktop->peakLuminance;
  int  maxCLL        = desktop->maxCLL;

  igSeparator();
  igCheckbox("Map HDR content to SDR", &mapHDRtoSDR);
  igSliderInt("Peak Luminance", &peakLuminance, 1, 10000,
      "%d nits",
      ImGuiInputTextFlags_CharsDecimal);
  igSliderInt("Max content light level", &maxCLL, 1, 10000,
      "%d nits", ImGuiInputTextFlags_CharsDecimal);

  if (mapHDRtoSDR   != desktop->mapHDRtoSDR   ||
      peakLuminance != desktop->peakLuminance ||
      maxCLL        != desktop->maxCLL)
  {
    desktop->mapHDRtoSDR   = mapHDRtoSDR;
    desktop->peakLuminance = max(1, peakLuminance);
    desktop->maxCLL        = max(1, maxCLL);
    app_invalidateWindow(true);
  }
}

bool egl_desktopSetup(EGL_Desktop * desktop, const LG_RendererFormat format)
{
  memcpy(&desktop->format, &format, sizeof(LG_RendererFormat));

  enum EGL_PixelFormat pixFmt;
  switch(format.type)
  {
    case FRAME_TYPE_BGRA:
      pixFmt = EGL_PF_BGRA;
      break;

    case FRAME_TYPE_RGBA:
      pixFmt = EGL_PF_RGBA;
      break;

    case FRAME_TYPE_RGBA10:
      pixFmt = EGL_PF_RGBA10;
      break;

    case FRAME_TYPE_RGBA16F:
      pixFmt = EGL_PF_RGBA16F;
      break;

    case FRAME_TYPE_BGR_32:
      pixFmt = EGL_PF_BGR_32;
      break;

    case FRAME_TYPE_RGB_24:
      pixFmt = EGL_PF_RGB_24;
      // the data width is correct per the format, but we are going to use a
      // 32-bit texture to load the data, so we need to alter the width for the
      // different bpp
      desktop->format.dataWidth =
        desktop->format.pitch / 4;
      break;

    default:
      DEBUG_ERROR("Unsupported frame format");
      return false;
  }

  desktop->width  = format.frameWidth;
  desktop->height = format.frameHeight;
  desktop->hdr    = format.hdr;
  desktop->hdrPQ  = format.hdrPQ;

  if (!egl_textureSetup(
    desktop->texture,
    pixFmt,
    desktop->format.dataWidth,
    desktop->format.dataHeight,
    desktop->format.stride,
    desktop->format.pitch
  ))
  {
    DEBUG_ERROR("Failed to setup the desktop texture");
    return false;
  }

  return true;
}

bool egl_desktopUpdate(EGL_Desktop * desktop, const FrameBuffer * frame, int dmaFd,
    const FrameDamageRect * damageRects, int damageRectsCount)
{
  if (likely(desktop->useDMA && dmaFd >= 0))
  {
    if (likely(egl_textureUpdateFromDMA(desktop->texture, frame, dmaFd)))
    {
      atomic_store(&desktop->processFrame, true);
      return true;
    }

    DEBUG_WARN("DMA update failed, disabling DMABUF imports");

    const char * vendor  = (const char *)glGetString(GL_VENDOR);
    if (strstr(vendor, "NVIDIA"))
    {
      DEBUG_WARN("NVIDIA's DMABUF support is incomplete, please direct your complaints to NVIDIA");
      DEBUG_WARN("This is not a bug in Looking Glass");
    }

    desktop->useDMA = false;

    const char * gl_exts = (const char *)glGetString(GL_EXTENSIONS);
    if (!util_hasGLExt(gl_exts, "GL_EXT_buffer_storage"))
    {
      DEBUG_ERROR("GL_EXT_buffer_storage is needed to use EGL backend");
      return false;
    }

    egl_textureFree(&desktop->texture);
    if (!egl_textureInit(&desktop->texture, desktop->display,
          EGL_TEXTYPE_FRAMEBUFFER))
    {
      DEBUG_ERROR("Failed to initialize the desktop texture");
      return false;
    }

    if (!egl_desktopSetup(desktop, desktop->format))
      return false;
  }

  if (likely(egl_textureUpdateFromFrame(desktop->texture, frame,
        damageRects, damageRectsCount)))
  {
    atomic_store(&desktop->processFrame, true);
    return true;
  }

  return false;
}

void egl_desktopResize(EGL_Desktop * desktop, int width, int height)
{
  atomic_store(&desktop->processFrame, true);
}

bool egl_desktopRender(EGL_Desktop * desktop, unsigned int outputWidth,
    unsigned int outputHeight, const float x, const float y,
    const float scaleX, const float scaleY, enum EGL_DesktopScaleType scaleType,
    LG_RendererRotate rotate, const struct DamageRects * rects)
{
  EGL_Texture * tex;
  int width, height;

  if (unlikely(desktop->useSpice))
  {
    tex    = desktop->spiceTexture;
    width  = desktop->spiceWidth;
    height = desktop->spiceHeight;
  }
  else
  {
    tex    = desktop->texture;
    width  = desktop->width;
    height = desktop->height;
  }

  if (unlikely(outputWidth == 0 && outputHeight == 0))
    DEBUG_FATAL("outputWidth || outputHeight == 0");

  enum EGL_TexStatus status;
  if (unlikely((status = egl_textureProcess(tex)) != EGL_TEX_STATUS_OK))
  {
    if (status != EGL_TEX_STATUS_NOTREADY)
      DEBUG_ERROR("Failed to process the desktop texture");
  }

  int scaleAlgo = EGL_SCALE_NEAREST;

  egl_desktopRectsMatrix((float *)desktop->matrix->data,
      width, height, x, y, scaleX, scaleY, rotate);
  egl_desktopRectsUpdate(desktop->mesh, rects, width, height);

  if (atomic_exchange(&desktop->processFrame, false) ||
      egl_postProcessConfigModified(desktop->pp))
    egl_postProcessRun(desktop->pp, tex, desktop->mesh,
        width, height, outputWidth, outputHeight, desktop->useDMA);

  unsigned int finalSizeX, finalSizeY;
  EGL_Texture * texture = egl_postProcessGetOutput(desktop->pp,
      &finalSizeX, &finalSizeY);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  egl_resetViewport(desktop->egl);

  glActiveTexture(GL_TEXTURE0);
  egl_textureBind(texture);

  if (finalSizeX > width || finalSizeY > height)
    scaleType = EGL_DESKTOP_DOWNSCALE;

  switch (desktop->scaleAlgo)
  {
    case EGL_SCALE_AUTO:
      switch (scaleType)
      {
        case EGL_DESKTOP_UPSCALE:
          scaleAlgo = EGL_SCALE_NEAREST;
          break;

        case EGL_DESKTOP_NOSCALE:
        case EGL_DESKTOP_DOWNSCALE:
          scaleAlgo = EGL_SCALE_LINEAR;
          break;
      }
      break;

    default:
      scaleAlgo = desktop->scaleAlgo;
  }

  const struct DesktopShader * shader =
    desktop->useDMA && texture == desktop->texture ?
      &desktop->dmaShader : &desktop->shader;

  const float mapHDRGain =
    (float)desktop->maxCLL / desktop->peakLuminance;

  EGL_Uniform uniforms[] =
  {
    {
      .type        = EGL_UNIFORM_TYPE_1I,
      .location    = shader->uScaleAlgo,
      .i           = { scaleAlgo },
    },
    {
      .type        = EGL_UNIFORM_TYPE_2F,
      .location    = shader->uDesktopSize,
      .f           = { width, height },
    },
    {
      .type        = EGL_UNIFORM_TYPE_M3x2FV,
      .location    = shader->uTransform,
      .m.transpose = GL_FALSE,
      .m.v         = desktop->matrix
    },
    {
      .type        = EGL_UNIFORM_TYPE_1F,
      .location    = shader->uNVGain,
      .f           = { (float)desktop->nvGain }
    },
    {
      .type        = EGL_UNIFORM_TYPE_1I,
      .location    = shader->uCBMode,
      .f           = { desktop->cbMode }
    },
    {
      .type        = EGL_UNIFORM_TYPE_1I,
      .location    = shader->uIsHDR,
      .i           = { desktop->hdr }
    },
    {
      .type        = EGL_UNIFORM_TYPE_1I,
      .location    = shader->uMapHDRtoSDR,
      .i           = { desktop->mapHDRtoSDR }
    },
    {
      .type        = EGL_UNIFORM_TYPE_1F,
      .location    = shader->uMapHDRGain,
      .f           = { mapHDRGain }
    },
    {
      .type        = EGL_UNIFORM_TYPE_1I,
      .location    = shader->uMapHDRPQ,
      .f           = { desktop->hdrPQ }
    }
  };

  egl_shaderSetUniforms(shader->shader, uniforms, ARRAY_LENGTH(uniforms));
  egl_shaderUse(shader->shader);
  egl_desktopRectsRender(desktop->mesh);
  glBindTexture(GL_TEXTURE_2D, 0);
  return true;
}

void egl_desktopSpiceConfigure(EGL_Desktop * desktop, int width, int height)
{
  if (!desktop->spiceTexture)
    if (!egl_textureInit(&desktop->spiceTexture, desktop->display,
          EGL_TEXTYPE_BUFFER_MAP))
    {
      DEBUG_ERROR("Failed to initialize the spice desktop texture");
      return;
    }

  if (!egl_textureSetup(
    desktop->spiceTexture,
    EGL_PF_BGRA,
    width,
    height,
    width,
    width * 4
  ))
  {
    DEBUG_ERROR("Failed to setup the spice desktop texture");
    return;
  }

  desktop->spiceWidth  = width;
  desktop->spiceHeight = height;
}

void egl_desktopSpiceDrawFill(EGL_Desktop * desktop, int x, int y, int width,
    int height, uint32_t color)
{
  /* this is a fairly hacky way to do this, but since it's only for the fallback
   * spice display it's not really an issue */

  uint32_t line[width];
  for(int x = 0; x < width; ++x)
    line[x] = color;

  for(; y < height; ++y)
    egl_textureUpdateRect(desktop->spiceTexture,
        x, y, width, 1, width, sizeof(line), (uint8_t *)line, false);

  atomic_store(&desktop->processFrame, true);
}

void egl_desktopSpiceDrawBitmap(EGL_Desktop * desktop, int x, int y, int width,
    int height, int stride, uint8_t * data, bool topDown)
{
  egl_textureUpdateRect(desktop->spiceTexture,
      x, y, width, height, width, stride, data, topDown);
  atomic_store(&desktop->processFrame, true);
}

void egl_desktopSpiceShow(EGL_Desktop * desktop, bool show)
{
  desktop->useSpice = show;
}
