/*
KVMGFX Client - A KVM Client for VGA Passthrough
Copyright (C) 2017-2019 Geoffrey McRae <geoff@hostfission.com>
https://looking-glass.hostfission.com

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "common/framebuffer.h"
#include "common/debug.h"

#include <string.h>
#define FB_CHUNK_SIZE 1024

bool framebuffer_read(const FrameBuffer frame, void * dst, size_t size)
{
  while(size)
  {
    /* spinlock */
    while(*frame.rp == *frame.wp) { }

    /* copy what we can */
    uint64_t avail = *frame.wp - *frame.rp;
    memcpy(dst, frame.data + *frame.rp, avail);
    *frame.rp += avail;
    size      -= avail;
  }

  return true;
}

bool framebuffer_read_fn(const FrameBuffer frame, FrameBufferReadFn fn, size_t size, void * opaque)
{
  while(size)
  {
    /* spinlock */
    while(*frame.rp == *frame.wp) { }

    /* copy what we can */
    uint64_t avail = *frame.wp - *frame.rp;
    if (!fn(opaque, frame.data + *frame.rp, avail))
      return false;
    *frame.rp += avail;
    size      -= avail;
  }

  return true;
}

bool framebuffer_write(const FrameBuffer frame, const void * src, size_t size)
{
  /* copy in chunks */
  while(size)
  {
    size_t copy = size > FB_CHUNK_SIZE ? FB_CHUNK_SIZE : size;
    memcpy(frame.data + *frame.wp, src, copy);
    *frame.wp += copy;
    size      -= copy;
  }
  return true;
}
