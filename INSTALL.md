# Installation

## Installation on Linux

gSculpt must be compiled from source on Linux. Given that it has not been in active development since 2009, it relies on rather old and out of date software and libaries, e.g. Python 2.4. You will need to install the following packages in order to build gSculpt:

- [Python 2.4](https://www.python.org/download/releases/2.4/)
- GTK+ 2.8 - GUI toolkit used by gSculpt
- PyGTK 2.8 - Python bindings for GTK+
- GTKGLext 1.0.6 - OpenGL for GTK
- pyGTKGLext 1.0.1 - Python bindings for GTKGLext.
- Boost 1.32 - Boost libraries; Boost::python MUST be included.
- PyOpenGL v2.0 - Python bindings for OpenGL.
- PIL 1.1.5 - Python Imaging Library (prior to the release of Pillow)
- Scons 0.96 - The Scons build system is used to build the gSculpt C++ components

Having installed the above packages, unpack the gSculpt package. Go into the directory that was created, and at the command prompt type:

> scons

To build gSculpt. Once this is done, gSculpt can be started by typing:

> ./gsculpt

If that fails, try:

> python gsculpt.py

A warning about gSculpt being alpha release software will attempt to scare you away. If you are tough enough, click 'OK', and the main gSculpt window will appear.

## Installation on Windows

Download the gSculpt MSI installer from the [releases](https://github.com/Britefury/gsculpt/releases) page and run it to install gSculpt. A shortcut will be created in the start menu.

A warning about gSculpt being alpha release software will attempt to scare you away. If you've got this far, you probably don't care.
