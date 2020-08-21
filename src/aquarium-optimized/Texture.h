//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Texture.h: Define abstract Texture for Aquarium.

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>

class Texture
{
  public:
    virtual ~Texture(){}
    Texture() {}
    Texture(const std::string &name, const std::vector<std::string> &urls, bool flip) : mUrls(urls), mFlip(flip), mName(name) {}
    Texture(const std::string &name, const std::string &url, bool flip);
    std::string getName() { return mName; }
    virtual void loadTexture() = 0;
    void generateMipmap(uint8_t *input_pixels,
                        int input_w,
                        int input_h,
                        int input_stride_in_bytes,
                        std::vector<uint8_t *> &output_pixels,
                        int output_w,
                        int output_h,
                        int output_stride_in_bytes,
                        int num_channels,
                        bool is256padding);

  protected:
    bool isPowerOf2(int);
    bool loadImage(const std::vector<std::string> &urls, std::vector<uint8_t *>* pixels);
    void DestoryImageData(std::vector<uint8_t *>& pixelVec);
    void copyPaddingBuffer(unsigned char *dst,
                           unsigned char *src,
                           int width,
                           int height,
                           int kPadding);

    std::vector<std::string> mUrls;
    int mWidth;
    int mHeight;
    bool mFlip;

    std::string mName;
};

#endif  // TEXTURE_H
