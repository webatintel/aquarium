//
// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "PlatformContextDawn.h"

#include "../Assert.h"
#include "GLFW/glfw3native.h"
#include "dawn/dawn_wsi.h"
#include "dawn/webgpu.h"

#if defined(DAWN_ENABLE_BACKEND_METAL)
#import <CoreGraphics/CGGeometry.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#endif

#if defined(GLFW_EXPOSE_NATIVE_COCOA)
#import <AppKit/NSView.h>
#import <AppKit/NSWindow.h>
#endif

namespace dawn_native {

class NativeSwapChainImpl {
#if defined(DAWN_ENABLE_BACKEND_METAL)
public:
  class Metal {
  public:
    Metal(NativeSwapChainImpl *nativeSwapchainImpl,
          DawnWSIContextMetal *wsiContext)
        : mNativeSwapchainImpl(nativeSwapchainImpl),
          mDevice(wsiContext->device),
          mQueue(wsiContext->queue),
          mDrawable(nil),
          mTexture(nil) {
      mNativeSwapchainImpl->mPreferredFormat =
          WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm;
    }

    void destroy() {
      [mDrawable release];
      [mTexture release];
      delete this;
    }

    DawnSwapChainError configure(WGPUTextureFormat format,
                                 WGPUTextureUsage allowedUsage,
                                 uint32_t width,
                                 uint32_t height) {
      CAMetalLayer *layer = [CAMetalLayer layer];
      [layer setDevice:mDevice];
      switch (format) {
      case WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm:
        [layer setPixelFormat:MTLPixelFormatBGRA8Unorm];
        break;
      default:
        ASSERT(false);
      }
      if ((~WGPUTextureUsage::WGPUTextureUsage_OutputAttachment &
           ~WGPUTextureUsage::WGPUTextureUsage_Present & allowedUsage) ==
          WGPUTextureUsage::WGPUTextureUsage_None) {
        [layer setFramebufferOnly:YES];
      }
      [layer setDrawableSize:CGSizeMake(width, height)];
      mNativeSwapchainImpl->mLayer = layer;
      return DAWN_SWAP_CHAIN_NO_ERROR;
    }

    DawnSwapChainError getNextTexture(DawnSwapChainNextTexture *nextTexture) {
      CAMetalLayer *layer =
          static_cast<CAMetalLayer *>(mNativeSwapchainImpl->mLayer);
      [mDrawable release];
      mDrawable = [layer nextDrawable];
      [mDrawable retain];
      [mTexture release];
      mTexture = mDrawable.texture;
      [mTexture retain];
      nextTexture->texture.ptr = mTexture;
      return DAWN_SWAP_CHAIN_NO_ERROR;
    }

    DawnSwapChainError present() {
      id<MTLCommandBuffer> commandBuffer = [mQueue commandBuffer];
      [commandBuffer presentDrawable:mDrawable];
      [commandBuffer commit];
      return DAWN_SWAP_CHAIN_NO_ERROR;
    }

  private:
    ~Metal() {}

    NativeSwapChainImpl *mNativeSwapchainImpl;
    id<MTLDevice> mDevice;
    id<MTLCommandQueue> mQueue;
    id<CAMetalDrawable> mDrawable;
    id<MTLTexture> mTexture;
  };

protected:
  void init(DawnWSIContextMetal *wsiContext) {
    mWSIContext = new Metal(this, wsiContext);
  }
#endif  // DAWN_ENABLE_BACKEND_METAL

#if defined(GLFW_EXPOSE_NATIVE_COCOA)
public:
  class Cocoa {
  public:
    Cocoa(NativeSwapChainImpl *nativeSwapchainImpl, NSWindow *window)
        : mNativeSwapchainImpl(nativeSwapchainImpl), mWindow(window) {
      mNativeSwapchainImpl->mPreferredFormat =
          WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm;
    }

    void destroy() { delete this; }

    DawnSwapChainError configure(WGPUTextureFormat format,
                                 WGPUTextureUsage allowedUsage,
                                 uint32_t width,
                                 uint32_t height) {
      NSView *view = mWindow.contentView;
      [view setWantsLayer:YES];
      [view setLayer:static_cast<CALayer *>(mNativeSwapchainImpl->mLayer)];
      return DAWN_SWAP_CHAIN_NO_ERROR;
    }

    DawnSwapChainError getNextTexture(DawnSwapChainNextTexture *nextTexture) {
      return DAWN_SWAP_CHAIN_NO_ERROR;
    }

    DawnSwapChainError present() { return DAWN_SWAP_CHAIN_NO_ERROR; }

  private:
    ~Cocoa() {}

    NativeSwapChainImpl *mNativeSwapchainImpl;
    NSWindow *mWindow;
  };

protected:
  explicit NativeSwapChainImpl(NSWindow *window)
      : mWSIContext(nullptr),
        mWindow(new Cocoa(this, window)),
        mPreferredFormat(WGPUTextureFormat::WGPUTextureFormat_Undefined),
        mLayer(nil) {}
#endif  // GLFW_EXPOSE_NATIVE_COCOA

public:
  WGPUTextureFormat getPreferredFormat() const { return mPreferredFormat; }

protected:
  virtual ~NativeSwapChainImpl() {}

  template <typename U, typename V>
  void destroy() {
    static_cast<U *>(mWSIContext)->destroy();
    static_cast<V *>(mWindow)->destroy();
    delete this;
  }

  template <typename U, typename V>
  DawnSwapChainError configure(WGPUTextureFormat format,
                               WGPUTextureUsage allowedUsage,
                               uint32_t width,
                               uint32_t height) {
    DawnSwapChainError error;
    error = static_cast<U *>(mWSIContext)
                ->configure(format, allowedUsage, width, height);
    if (error != DAWN_SWAP_CHAIN_NO_ERROR)
      return error;
    error = static_cast<V *>(mWindow)->configure(format, allowedUsage, width,
                                                 height);
    if (error != DAWN_SWAP_CHAIN_NO_ERROR)
      return error;
    return DAWN_SWAP_CHAIN_NO_ERROR;
  }

  template <typename U, typename V>
  DawnSwapChainError getNextTexture(DawnSwapChainNextTexture *nextTexture) {
    DawnSwapChainError error;
    error = static_cast<U *>(mWSIContext)->getNextTexture(nextTexture);
    if (error != DAWN_SWAP_CHAIN_NO_ERROR)
      return error;
    error = static_cast<V *>(mWindow)->getNextTexture(nextTexture);
    if (error != DAWN_SWAP_CHAIN_NO_ERROR)
      return error;
    return DAWN_SWAP_CHAIN_NO_ERROR;
  }

  template <typename U, typename V>
  DawnSwapChainError present() {
    DawnSwapChainError error;
    error = static_cast<U *>(mWSIContext)->present();
    if (error != DAWN_SWAP_CHAIN_NO_ERROR)
      return error;
    error = static_cast<V *>(mWindow)->present();
    if (error != DAWN_SWAP_CHAIN_NO_ERROR)
      return error;
    return DAWN_SWAP_CHAIN_NO_ERROR;
  }

private:
  void *mWSIContext;
  void *mWindow;
  WGPUTextureFormat mPreferredFormat;
  union {
    id mLayer;
  };
};

#if defined(DAWN_ENABLE_BACKEND_METAL)

namespace metal {

class NativeSwapChainImpl : public dawn_native::NativeSwapChainImpl {
  using dawn_native::NativeSwapChainImpl::init;
  using dawn_native::NativeSwapChainImpl::destroy;
  using dawn_native::NativeSwapChainImpl::configure;
  using dawn_native::NativeSwapChainImpl::getNextTexture;
  using dawn_native::NativeSwapChainImpl::present;

public:
  template <typename T>
  explicit NativeSwapChainImpl(T window)
      : dawn_native::NativeSwapChainImpl(window) {}

  static void init(void *userData, void *wsiContext) {
    auto nativeSwapchainImpl = static_cast<NativeSwapChainImpl *>(userData);
    nativeSwapchainImpl->init(static_cast<DawnWSIContextMetal *>(wsiContext));
  }
  template <typename T>
  static void destroy(void *userData) {
    auto nativeSwapchainImpl = static_cast<NativeSwapChainImpl *>(userData);
    nativeSwapchainImpl->destroy<Metal, T>();
  }
  template <typename T>
  static DawnSwapChainError configure(void *userData,
                                      WGPUTextureFormat format,
                                      WGPUTextureUsage allowedUsage,
                                      uint32_t width,
                                      uint32_t height) {
    auto nativeSwapchainImpl = static_cast<NativeSwapChainImpl *>(userData);
    return nativeSwapchainImpl->configure<Metal, T>(format, allowedUsage, width,
                                                    height);
  }
  template <typename T>
  static DawnSwapChainError getNextTexture(
      void *userData,
      DawnSwapChainNextTexture *nextTexture) {
    auto nativeSwapchainImpl = static_cast<NativeSwapChainImpl *>(userData);
    return nativeSwapchainImpl->getNextTexture<Metal, T>(nextTexture);
  }
  template <typename T>
  static DawnSwapChainError present(void *userData) {
    auto nativeSwapchainImpl = static_cast<NativeSwapChainImpl *>(userData);
    return nativeSwapchainImpl->present<Metal, T>();
  }

private:
  ~NativeSwapChainImpl() override {}
};

}  // namespace metal

#endif  // DAWN_ENABLE_BACKEND_METAL

}  // namespace dawn_native

PlatformContextDawn::PlatformContextDawn(BACKENDTYPE backendType)
    : ContextDawn(backendType) {
}

PlatformContextDawn::~PlatformContextDawn() {
}

DawnSwapChainImplementation *PlatformContextDawn::getSwapChainImplementation(
    wgpu::BackendType backendType) {
#if defined(DAWN_ENABLE_BACKEND_METAL) && defined(GLFW_EXPOSE_NATIVE_COCOA)
  if (backendType == wgpu::BackendType::Metal) {
    NSWindow *window = glfwGetCocoaWindow(mWindow);
    mSwapchainImpl.Init = dawn_native::metal::NativeSwapChainImpl::init;
    mSwapchainImpl.Destroy = dawn_native::metal::NativeSwapChainImpl::destroy<
        dawn_native::metal::NativeSwapChainImpl::Cocoa>;
    mSwapchainImpl.Configure =
        dawn_native::metal::NativeSwapChainImpl::configure<
            dawn_native::metal::NativeSwapChainImpl::Cocoa>;
    mSwapchainImpl.GetNextTexture =
        dawn_native::metal::NativeSwapChainImpl::getNextTexture<
            dawn_native::metal::NativeSwapChainImpl::Cocoa>;
    mSwapchainImpl.Present = dawn_native::metal::NativeSwapChainImpl::present<
        dawn_native::metal::NativeSwapChainImpl::Cocoa>;
    mSwapchainImpl.userData =
        new dawn_native::metal::NativeSwapChainImpl(window);
    return &mSwapchainImpl;
  }
#endif  // DAWN_ENABLE_BACKEND_METAL && GLFW_EXPOSE_NATIVE_COCOA
  return nullptr;
}

wgpu::TextureFormat PlatformContextDawn::getPreferredSwapChainTextureFormat(
    wgpu::BackendType backendType) {
  WGPUTextureFormat format = WGPUTextureFormat::WGPUTextureFormat_Undefined;
  switch (backendType) {
#if defined(DAWN_ENABLE_BACKEND_METAL)
  case wgpu::BackendType::Metal:
    format = static_cast<dawn_native::metal::NativeSwapChainImpl *>(
                 mSwapchainImpl.userData)
                 ->getPreferredFormat();
    break;
#endif
  default:
    ASSERT(false);
  }
  return static_cast<wgpu::TextureFormat>(format);
}
