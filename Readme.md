# Breakout

A simple 2D game based on the 1976 [Atari game](https://atari.com/pages/breakout) and following the breakout game [learnopengl tutorial](https://learnopengl.com/In-Practice/2D-Game/Breakout). This project is built using C++ and OpenGL and serves as a demonstration of basic game development concepts such as rendering, input handling, and game logic. The boilerplate code is forked from [Glitter](https://github.com/Polytonic/Glitter), which provides a simple OpenGL setup and window management along with a CMake configuration linking a handful of useful libraries for OpenGL graphics and basic game development.

<p align="center">
<img width="800" alt="Screenshot 2025-06-16 at 1 26 02 PM" src="https://github.com/user-attachments/assets/b00b35f5-a006-4529-a844-0be190e295a8" />
</p>

## Getting Started (mostly copied from Glitter's Readme)

This project has a single dependency: [cmake](http://www.cmake.org/download/), which is used to generate platform-specific makefiles or project files. Start by cloning this repository, making sure to pass the `--recursive` flag to grab all the dependencies. If you forgot, then you can `git submodule update --init` instead.

```bash
git clone --recursive https://github.com/mackcooper1408/Breakout
cd Breakout
cd Build
```

Now generate a project file or makefile for your platform. If you want to use a particular IDE, make sure it is installed; don't forget to set the Start-Up Project in Visual Studio or the Target in Xcode.

```bash
# UNIX Makefile
cmake ..

# Mac OSX
cmake -G "Xcode" ..

# Microsoft Windows
cmake -G "Visual Studio 14" ..
cmake -G "Visual Studio 14 Win64" ..
...
```

## License for using Glitter

> The MIT License (MIT)

> Copyright (c) 2015 Kevin Fung

> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
