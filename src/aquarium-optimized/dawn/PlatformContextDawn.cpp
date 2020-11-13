//
// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "PlatformContextDawn.h"

#if defined(GLFW_EXPOSE_NATIVE_X11)
// workaround conflict between poorly named X macro and 3rd-party code
#include <X11/X.h>
#undef None
#undef Success
#undef Always
#endif

#include "GLFW/glfw3native.h"
#include "build/build_config.h"
#include "dawn/webgpu.h"

#include "common/AQUARIUM_ASSERT.h"

#if defined(OS_WIN)
#include <Windows.h>
#endif
#if defined(OS_LINUX) && !defined(OS_CHROMEOS)
#include <dlfcn.h>
#endif

#if defined(DAWN_ENABLE_BACKEND_VULKAN) && defined(GLFW_EXPOSE_NATIVE_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#if defined(DAWN_ENABLE_BACKEND_VULKAN) && defined(GLFW_EXPOSE_NATIVE_X11)
#include <X11/Xlib.h>
#include <xcb/xcb.h>

#define VK_USE_PLATFORM_XCB_KHR
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#if defined(DAWN_ENABLE_BACKEND_D3D12)
#include "dawn_native/D3D12Backend.h"
#endif
#if defined(DAWN_ENABLE_BACKEND_VULKAN)
#include <string>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "dawn_native/VulkanBackend.h"
#include "vulkan/vulkan.h"
#endif

PlatformContextDawn::PlatformContextDawn(BACKENDTYPE backendType)
    : ContextDawn(backendType) {
}

PlatformContextDawn::~PlatformContextDawn() {
}

DawnSwapChainImplementation *PlatformContextDawn::getSwapChainImplementation(
    wgpu::BackendType backendType) {
#if defined(DAWN_ENABLE_BACKEND_D3D12) && defined(GLFW_EXPOSE_NATIVE_WIN32)
  if (backendType == wgpu::BackendType::D3D12) {
    HWND window = glfwGetWin32Window(mWindow);
    mSwapchainImpl =
        dawn_native::d3d12::CreateNativeSwapChainImpl(mDevice.Get(), window);
    return &mSwapchainImpl;
  }
#endif  // DAWN_ENABLE_BACKEND_D3D12 && GLFW_EXPOSE_NATIVE_WIN32
#if defined(VK_USE_PLATFORM_WIN32_KHR)
  if (backendType == wgpu::BackendType::Vulkan) {
    uint32_t extensionCount;
    const char **extensions =
        glfwGetRequiredInstanceExtensions(&extensionCount);
    for (uint32_t i = 0; i < extensionCount; i++) {
      if (std::string(extensions[i]) == VK_KHR_WIN32_SURFACE_EXTENSION_NAME) {
        auto vkCreateWin32SurfaceKHR =
            reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
                dawn_native::vulkan::GetInstanceProcAddr(
                    mDevice.Get(), "vkCreateWin32SurfaceKHR"));
        VkInstance instance = dawn_native::vulkan::GetInstance(mDevice.Get());
        VkWin32SurfaceCreateInfoKHR createInfo;
        VkSurfaceKHR surface;
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
#if defined(OS_WIN)
        createInfo.hinstance = GetModuleHandle(nullptr);
#else
        ASSERT(false);
#endif
        createInfo.hwnd = glfwGetWin32Window(mWindow);
        vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);
        mSwapchainImpl = dawn_native::vulkan::CreateNativeSwapChainImpl(
            mDevice.Get(), surface);
        return &mSwapchainImpl;
      }
    }
  }
#endif  // VK_USE_PLATFORM_WIN32_KHR
#if defined(VK_USE_PLATFORM_XCB_KHR)
  if (backendType == wgpu::BackendType::Vulkan) {
    uint32_t extensionCount;
    const char **extensions =
        glfwGetRequiredInstanceExtensions(&extensionCount);
    for (uint32_t i = 0; i < extensionCount; i++) {
      if (std::string(extensions[i]) == VK_KHR_XCB_SURFACE_EXTENSION_NAME) {
        // GLFW should never prefer VK_KHR_xcb_surface when the system provides
        // no way to get the XCB connection for an Xlib Display. IOW, if
        // Xlib/XCB is missing, VK_KHR_xlib_surface should be blessed.
        // Otherwise, the system is insane.
        xcb_connection_t *connection = nullptr;
#if defined(OS_LINUX) && !defined(OS_CHROMEOS)
        {
          void *handle = dlopen("libX11-xcb.so.1", RTLD_NOW);
          if (handle) {
            typedef xcb_connection_t *(*XGetXCBConnectionPtr)(Display *);
            auto XGetXCBConnection = reinterpret_cast<XGetXCBConnectionPtr>(
                dlsym(handle, "XGetXCBConnection"));
            Display *display = glfwGetX11Display();
            connection = XGetXCBConnection(display);
            dlclose(handle);
          }
        }
#else
        ASSERT(false);
#endif
        if (connection) {
          auto vkCreateXcbSurfaceKHR =
              reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(
                  dawn_native::vulkan::GetInstanceProcAddr(
                      mDevice.Get(), "vkCreateXcbSurfaceKHR"));
          VkInstance instance = dawn_native::vulkan::GetInstance(mDevice.Get());
          VkXcbSurfaceCreateInfoKHR createInfo;
          VkSurfaceKHR surface;
          createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
          createInfo.pNext = nullptr;
          createInfo.flags = 0;
          createInfo.connection = connection;
          createInfo.window = glfwGetX11Window(mWindow);
          vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &surface);
          mSwapchainImpl = dawn_native::vulkan::CreateNativeSwapChainImpl(
              mDevice.Get(), surface);
          return &mSwapchainImpl;
        }
      }
    }
  }
#endif  // VK_USE_PLATFORM_XCB_KHR
#if defined(VK_USE_PLATFORM_XLIB_KHR)
  if (backendType == wgpu::BackendType::Vulkan) {
    uint32_t extensionCount;
    const char **extensions =
        glfwGetRequiredInstanceExtensions(&extensionCount);
    for (uint32_t i = 0; i < extensionCount; i++) {
      if (std::string(extensions[i]) == VK_KHR_XLIB_SURFACE_EXTENSION_NAME) {
        auto vkCreateXlibSurfaceKHR =
            reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(
                dawn_native::vulkan::GetInstanceProcAddr(
                    mDevice.Get(), "vkCreateXlibSurfaceKHR"));
        VkInstance instance = dawn_native::vulkan::GetInstance(mDevice.Get());
        VkXlibSurfaceCreateInfoKHR createInfo;
        VkSurfaceKHR surface;
        createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.dpy = glfwGetX11Display();
        createInfo.window = glfwGetX11Window(mWindow);
        vkCreateXlibSurfaceKHR(instance, &createInfo, nullptr, &surface);
        mSwapchainImpl = dawn_native::vulkan::CreateNativeSwapChainImpl(
            mDevice.Get(), surface);
        return &mSwapchainImpl;
      }
    }
  }
#endif  // VK_USE_PLATFORM_XLIB_KHR
  return nullptr;
}

wgpu::TextureFormat PlatformContextDawn::getPreferredSwapChainTextureFormat(
    wgpu::BackendType backendType) {
  WGPUTextureFormat format = WGPUTextureFormat::WGPUTextureFormat_Undefined;
  switch (backendType) {
#if defined(DAWN_ENABLE_BACKEND_D3D12)
  case wgpu::BackendType::D3D12:
    format =
        dawn_native::d3d12::GetNativeSwapChainPreferredFormat(&mSwapchainImpl);
    break;
#endif
#if defined(DAWN_ENABLE_BACKEND_VULKAN)
  case wgpu::BackendType::Vulkan:
    format =
        dawn_native::vulkan::GetNativeSwapChainPreferredFormat(&mSwapchainImpl);
    break;
#endif
  default:
    ASSERT(false);
  }
  return static_cast<wgpu::TextureFormat>(format);
}
