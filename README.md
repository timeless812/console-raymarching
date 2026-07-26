<br />
<div align="center">
  <h2 align="center">The Raymarhing engine written in C with rendering from console.</h2>
  <a href="https://github.com/timeless812/console-raymarching">
    <img src="pic/arch_linux_kitty.jpg" alt="screenshot" width="450" height="450">
    <img src="pic/windows(wineconsole).jpg" alt="screenshot" width="450" height="450">
  </a>
  <p align="center">
    Works simply from the console using <a href="https://en.wikipedia.org/wiki/Ncurses">ncurses</a>.
  </p>
</div>

## Overview

This engine can only take the object's SDF function (write it to src/cpu_engine.c or kernel.cl,
depending on whether you want to run on CPU or GPU) and render it using raymarching.
In my engine, there is only light calculation, no shadows or reflections.

The camera is controlled from the keyboard (implemented via getch() in ncurses).
In the previous version, there was control over the rotation of the camera with the mouse,
which I implemented by reading events in /dev/input/mice, but this kind of requires sudo, so I removed that.

## Camera Control

W, A, S, D - movement, <br>
I, J, K, L - rotate.

Also you may change the speed and sensitivity in src/camera.c.

## CPU and GPU engine

In this project, there are two raymarching engines: one for CPU (src/cpu_engine.c), the other for GPU (kernel.cl).
for the GPU engine, you will need the OpenCL library on the system.
I also note that during normal startup it is the CPU engine that works (for GPU, add the -g flag).

## How to Build
the project is built through the Makefile. I haven't tested it on Windows, but I'll show you how to cross-compile on Linux.

The build requires the ncurses and OpenCL libraries. If you won't try rendering through the GPU, you can only install OpenCL header files.

I have Arch linux and I installed this
```sh
sudo pacman -S opencl-headers
```

## Installing OpenCL (Optional!)

I have intel UHD graphics and I installed OpenCL like this

```sh
sudo pacman -S intel-compute-runtime ocl-icd
```

To verify that OpenCL works, you can check via clinfo
```
sudo pacman -S clinfo
clinfo
```

## Build

Finally, we can build the project

```sh
make all
# or
make clean compile
```

You can start it manually
```
./build/raymarching
```

I also note that there are -g (GPU rendering) and -c (color mode) flags. 
The color mode displays colored spaces instead of symbols. It may not work on Windows.

## Cross-Compile

I didn't build this project on Windows, so I'll show you how I cross-compiled.

First, you need to install a cross compiler
```sh
sudo pacman -S mingw-w64-gcc
```

Now you need the right libraries.

There are none in pacman repositories, so I install them via AUR
```sh
yay -S mingw-w64-ncurses mingw-w64-opencl-headers mingw-w64-opencl-icd
```

And here is the cross-compilation itself
```sh
x86_64-w64-mingw32-gcc \
  -DNCURSES_STATIC \
  -I/usr/x86_64-w64-mingw32/include/ncursesw \
  -L/usr/x86_64-w64-mingw32/lib \
  main.c src/*.c \
  -Wl,-Bstatic -lncursesw -lwinpthread \
  -Wl,-Bdynamic -lOpenCL \
  -lm \
  -o build/raymarching.exe
```

I've been testing this with wineconsole
```sh
wineconsole build/raymarching.exe
```

## To do List

* Bring back the control of the camera rotation with the mouse.
* Add loading the SDF function (or scene) directly from main.c.
* Add shadows and reflections.
* Add a more interesting scene to test.
