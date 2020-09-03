//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Texture.cpp: Use stb image loader to loading images from files.

#include "Texture.h"

#include <algorithm>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize.h"

#include "../common/AQUARIUM_ASSERT.h"

Texture::Texture(const std::string &name, const std::string &url, bool flip)
    : mUrls(), mWidth(0), mHeight(0), mFlip(flip), mName(name)
{
  std::string urlpath = url;
  mUrls.push_back(urlpath);
}

// Force loading 3 channel images to 4 channel by stb becasue Dawn doesn't
// support 3 channel formats currently. The group is discussing on whether
// webgpu shoud support 3 channel format.
// https://github.com/gpuweb/gpuweb/issues/66#issuecomment-410021505
bool Texture::loadImage(const std::vector<std::string> &urls,
                        std::vector<uint8_t *> *pixels)
{
  stbi_set_flip_vertically_on_load(mFlip);
  for (auto filename : urls)
  {
    uint8_t *pixel = stbi_load(filename.c_str(), &mWidth, &mHeight, 0, 4);
    if (pixel == 0)
    {
      std::cout << stderr << "Couldn't open input file" << filename
                << std::endl;
      return false;
    }
    pixels->push_back(pixel);
  }
  return true;
}

bool Texture::isPowerOf2(int value)
{
  return (value & (value - 1)) == 0;
}

// Free image data after upload to gpu
void Texture::DestoryImageData(std::vector<uint8_t *> &pixelVec)
{
  for (auto &pixels : pixelVec)
  {
    free(pixels);
    pixels = nullptr;
  }
}

void Texture::copyPaddingBuffer(unsigned char *dst,
                                unsigned char *src,
                                int width,
                                int height,
                                int kPadding)
{
  unsigned char *s = src;
  unsigned char *d = dst;
  for (int i = 0; i < height; ++i)
  {
    memcpy(d, s, width * 4);
    s += width * 4;
    d += kPadding * 4;
  }
}

void Texture::generateMipmap(uint8_t *input_pixels,
                             int input_w,
                             int input_h,
                             int input_stride_in_bytes,
                             std::vector<uint8_t *> &output_pixels,
                             int output_w,
                             int output_h,
                             int output_stride_in_bytes,
                             int num_channels,
                             bool is256padding)
{
  int mipmapLevel =
      static_cast<uint32_t>(floor(log2(std::max(output_w, output_h)))) + 1;
  output_pixels.resize(mipmapLevel);
  int height = output_h;
  int width  = output_w;

  if (!is256padding)
  {
    for (int i = 0; i < mipmapLevel; ++i)
    {
      output_pixels[i] =
          (unsigned char *)malloc(output_w * height * 4 * sizeof(char));
      stbir_resize_uint8(input_pixels, input_w, input_h, input_stride_in_bytes,
                         output_pixels[i], width, height,
                         output_stride_in_bytes, num_channels);

      height >>= 1;
      width >>= 1;
      if (height == 0)
      {
        height = 1;
      }
    }
  }
  else
  {
    uint8_t *pixels =
        (unsigned char *)malloc(output_w * height * 4 * sizeof(char));

    for (int i = 0; i < mipmapLevel; ++i)
    {
      output_pixels[i] =
          (unsigned char *)malloc(output_w * height * 4 * sizeof(char));
      stbir_resize_uint8(input_pixels, input_w, input_h, input_stride_in_bytes,
                         pixels, width, height, output_stride_in_bytes,
                         num_channels);
      copyPaddingBuffer(output_pixels[i], pixels, width, height, output_w);

      height >>= 1;
      width >>= 1;
      if (height == 0)
      {
        height = 1;
      }
    }
    free(pixels);
  }
}
