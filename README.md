# Aquarium
Aquarium is a native implementation of [WebGL Aquarium](https://github.com/WebGLSamples/WebGLSamples.github.io). The goal of this project is to compare the performance of WebGL version to its native counterparts, such as OpenGL, D3D, Vulkan, Metal, ANGLE and Dawn.

# Progress
<table>
  <tr align=center>
    <td><strong>OS</td>
    <td><strong>Backend</td>
    <td><strong>Supported</td>
    <td><strong>MSAA</td>
    <td><strong>Dynamic Buffer Offset</td>
    <td><strong>Instanced Draw</td>
    <td><strong>Dynamically choose GPU</td>
  </tr>
  <tr align=left >
    <td>Linux</td>
    <td>OpenGL</td>
    <td>Y</td>
    <td>Y</td>
    <td>Not supported</td>
    <td>N</td>
    <td>Not supported</td>
  </tr>
  <tr align=left>
    <td>Linux</td>
    <td>Dawn/Vulkan</td>
    <td>Y</td>
    <td>Y</td>
    <td>Y</td>
    <td>Y</td>
    <td>Y</td>
  </tr>
  <tr align=left class="supported-row">
    <td>macOS</td>
    <td>OpenGL</td>
    <td>Y</td>
    <td>Y</td>
    <td>Not supported</td>
    <td>N</td>
    <td>Not supported</td>
  </tr>
  <tr align=left class="supported-row">
    <td>macOS</td>
    <td>Dawn/Metal</td>
    <td>Y</td>
    <td>Y</td>
    <td>Y</td>
    <td>Y</td>
    <td>Y</td>
  </tr>
  <tr align=left class="supported-row">
    <td>Windows</td>
    <td>OpenGL</td>
    <td>Y</td>
    <td>Y</td>
    <td>Not supported</td>
    <td>N</td>
    <td>Not supported</td>
  </tr>
  <tr align=left class="supported-row">
    <td>Windows</td>
    <td>ANGLE/D3D11</td>
    <td>Y</td>
    <td>N</td>
    <td>Not supported</td>
    <td>N</td>
    <td>Not supported</td>
  </tr>
  <tr align=left class="supported-row">
    <td>Windows</td>
    <td>Dawn/D3D12</td>
    <td>Y</td>
    <td>Y</td>
    <td>N</td>
    <td>Y</td>
    <td>Y</td>
  </tr>
  <tr align=left class="supported-row">
    <td>Windows</td>
    <td>Dawn/Vulkan</td>
    <td>Y</td>
    <td>Y</td>
    <td>Y</td>
    <td>Y</td>
    <td>Y</td>
  </tr>
  <tr align=left  class="supported-row">
    <td>Windows</td>
    <td>D3D12</td>
    <td>Y</td>
    <td>Y</td>
    <td>Y</td>
    <td>Y</td>
    <td>Y</td>
  </tr>
  <tr align=left>
    <td>Linux</td>
    <td>ANGLE/OpenGL</td>
    <td>N</td>
    <td>N</td>
    <td>Not supported</td>
    <td>N</td>
    <td>Not supported</td>
  </tr>
  <tr align=left>
    <td>Linux</td>
    <td>ANGLE/Vulkan</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
  </tr>
  <tr align=left>
    <td>Linux</td>
    <td>Vulkan</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
  </tr>
  <tr align=left>
    <td>macOS</td>
    <td>ANGLE/OpenGL</td>
    <td>N</td>
    <td>N</td>
    <td>Not supported</td>
    <td>N</td>
    <td>Not supported</td>
  </tr>
  <tr align=left>
    <td>macOS</td>
    <td>Metal</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
  </tr>
  <tr align=left>
    <td>Windows</td>
    <td>ANGLE/OpenGL</td>
    <td>N</td>
    <td>N</td>
    <td>Not supported</td>
    <td>N</td>
    <td>Not supported</td>
  </tr>
  <tr align=left>
    <td>Windows</td>
    <td>ANGLE/Vulkan</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
  </tr>
  <tr align=left>
    <td>Windows</td>
    <td>D3D11</td>
    <td>N</td>
    <td>N</td>
    <td>Not supported</td>
    <td>N</td>
    <td>Not supported</td>
  </tr>
  <tr align=left>
    <td>Windows</td>
    <td>Vulkan</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
    <td>N</td>
  </tr>
</table>

# Required Tools and Configurations
Configure [depot_tools](http://dev.chromium.org/developers/how-tos/install-depot-tools) on all platforms.

## Windows Requirement
The OpenGL version is required to >= 4.5 on Windows.
Windows sdk version is required to be over
[10.0.17134.0](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk) to support D3D12 backend.
Visual Studio 2017 is recommand if you want to debug into the code.
## Linux Requirement
The OpenGL version is required to >= 4.5 on Linux. To run vulkan of Dawn, please install vulkan driver on ubuntu.
If you are using Mesa driver, you should install the following library:
```sh
sudo apt-get install mesa-vulkan-drivers
```
If you are using Nvidia gpu, you should check if the driver support vulkan.
## macOS Requirement
The OpenGL version is required to >= 4.1 on macOS. To run Dawn/Metal backend, please check if your macOS support metal.
```sh
ps aux | grep -i "metal"
```

## Build OpenGL, D3D12 and Dawn backends

Aquarium uses gn to build on Linux, macOS and Windows.

```sh
# Aquarium contains two projects, aquarium-direct-map is a direct map version to WebGL repo, and only has OpenGL backend.  The aquarium
# project contains many backends.

# cd the repo
cd aquarium

# update repo
git pull

# Download thirdparty
gclient sync

# Build on aquarium by ninja on Windows, Linux and macOS.
# On windows, opengl, d3d12 and dawn backends are enabled by default.
# On linux and macOS, opengl and dawn are enabled by default.
# Enable or disable a specific platform, you can add 'enable_opengl', 'enable_d3d12', and 'enable_dawn' to gn args.
# To build a release version, specify 'is_debug=false'.
gn gen out/Release --args="is_debug=false"
ninja -C out/Release aquarium
ninja -C out/Release aquarium-direct-map

# Build on Windows by vs
gn gen out/build --ide=vs
open out/build/all.sln using visual studio.
build aquarium by vs

# Build on macOS by xcode
gn gen out/build --ide=xcode
build aquarium by xcode
```

## Build ANGLE backend

Because ANGLE headers have conflicts with other backends, it can only build individually. To build ANGLE version on Windows， please refer to the following steps (ANGLE backend is only supported on Windows now).
```sh
# cd the repo
cd aquarium

# download thirdparty
gclient sync

#Build aquarium ninja
gn gen out/Release --args="is_debug=false enable_angle=true"
ninja -C out/Release aquarium

# Build on Windows by vs
gn gen out/Release --ide=vs --args="is_debug=false enable_angle=true"
open out/Release/all.sln using visual studio.
build aquarium by vs
```

# Run
```sh
# "--num-fish" : specifies how many fishes will be rendered
# "--backend" : specifies running a certain backend, 'opengl', 'dawn_d3d12', 'dawn_vulkan', 'dawn_metal', 'dawn_opengl', 'angle'
# "--enable-full-screen-mode" : specifies rendering a full screen mode
# "--auto-test [count]" : log fps

# run on Windows
aquarium.exe --num-fish 10000 --backend dawn_d3d12
aquarium.exe --num-fish 10000 --backend dawn_vulkan
aquarium.exe --num-fish 10000 --backend angle

# run on Linux
./aquarium  --num-fish 10000 --backend opengl
./aquarium.exe --num-fish 10000 --backend dawn_vulkan

# run on macOS
./aquarium  --num-fish 10000 --backend opengl
./aquarium.exe --num-fish 10000 --backend dawn_metal

# "--enable-instanced-draws" : specifies rendering fishes by instanced draw. By default fishes
# are rendered by individual draw. Instanced rendering is only supported on dawn and d3d12 backend now.

aquarium.exe --num-fish 10000 --backend dawn_d3d12 --enable-instanced-draws
aquarium.exe --num-fish 10000 --backend dawn_vulkan --enable-instanced-draws
aquarium.exe --num-fish 10000 --backend d3d12 --enable-instanced-draws

# MSAA is disabled by default. To Enable MSAA, "--enable-msaa", 4 samples.
# MSAA of ANGLE is not supported now.

aquarium.exe --num-fish 10000 --backend opengl --enable-msaa


# “--disable-dynamic-buffer-offset” ：The path is to test individual draw by creating many binding groups on dawn backend.
# By default, dynamic buffer offset is enabled. This arg is only supported on dawn backend.

aquarium.exe --num-fish 10000 --backend dawn_d3d12 --disable-dynamic-buffer-offset
aquarium.exe --num-fish 10000 --backend dawn_vulkan --disable-dynamic-buffer-offset

# "--integrated-gpu", "--discrete-gpu": Specifies which gpu to render the application. The two args are exclusive.
# This is an optional arg. By default, a default adapter will be created.
# The option is only supported on dawn and d3d12 backend.
aquarium.exe --num-fish 10000 --backend dawn_d3d12 --integrated-gpu
aquarium.exe --num-fish 10000 --backend dawn_vulkan --discrete-gpu

# "--buffer-mapping-aync" : Test buffer mapping async mode to update fish positions.
# This mode is only implemented for Dawn backend.
aquarium.exe --num-fish 10000 --backend dawn_d3d12 --buffer-mapping-async

# "--enable-full-screen-mode" : Render aquarium in full screen mode instead of window mode.
aquarium.exe --num-fish 10000 --backend dawn_d3d12 --enable-full-screen-mode

# "--record-fps-frequency [count]" : Run aquarium 10 min and print fps log when exit.
aquarium.exe --num-fish 10000 --backend dawn_d3d12 --record-fps-frequency [count]

# "--test-time [second]" : Render the application for some second and then exit, and the application will run 5 min by default.
aquarium.exe --num-fish 10000 --backend dawn_d3d12 --test-time 30

#"--window-size=[width],[height]" : Set window size.
aquarium.exe --num-fish 10000 --backend dawn_d3d12 --window-size=2560,1440

# aquarium-direct-map only has OpenGL backend
# Enable MSAA
./aquarium-direct-map  --num-fish 10000 --backend opengl --enable-msaa

```

# TODO
* Dawn Vulkan backend doesn't work now. We need to implement recreate swap chain in Dawn.
* Debug mode of Dawn Metal backend has some issues to be fixed.
* Extra semicolon in dawn/third_party/shaderc throws warning when build the project. This should be fixed in Dawn.
* Texture without VK_IMAGE_USAGE_TRANSFER_DST_BIT should not be cleared. This is a warning of Dawn Vulkan backend.
* Enable dynamic choosing backend for ANGLE backend.
* Enable MSAA mode for ANGLE backend.
* Support Imgui for Direct Map Aquarium.
* Check and create the highest OpenGL version supported on each of the platforms.
* Check if some feature is available for D3D12 backend.
