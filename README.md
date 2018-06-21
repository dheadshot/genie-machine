# genie-machine
The Genie Machine is/will be a simple cross-platform Genealogy Database program written in C.

## Motivations

### Why Both Windows and Linux?

While Windows is the most popular Operating System, the Genie Machine will also work on Linux.  A large proportion of those documenting their family-history will be retired as age tends to focus the mind on these things.  They will thus have a reduced income.  A decent Windows PC is a substantial investment for those on a budget.  However, most people already have a TV and therefore the Raspberry Pi is a reasonably priced alternative.  The Raspberry Pi primarily runs Linux, thus it makes sense to use technologies for the Genie Machine that can run on both platforms.

## Compilation Dependencies

```
IUP
->CD
  ->IM
    ->Lua (Development files)
->iupmatrixex.h from previous version
->srcmatrixex/* from previous version
->Webkit Development files?
->tecmake
SQLite3
```

### IUP

IUP is a library for creating applications that use Native Win32 API on Windows and GTK+ on Linux.  It is licenced using the MIT Licence and is available for download from http://webserver2.tecgraf.puc-rio.br/iup .  If a precompiled version does not exist for your system, you will need to compile it from the sources.  The Genie Machine uses IUP version 3.24.  This version requires the following dependencies when it is compiled from sources:

- CD Canvas Draw (see below)
- IM Image Library (see below)
- The Lua 5.1 development files
- OpenGL Development files
- Standard Windows development libraries (for the Windows version): gdi32, user32, comdlg32, comctl32, uuid, ole32
- Standard GTK+ development libraries (for the Linux version): gtk-x11-2.0, gdk-x11-2.0, pangox-1.0, gdk_pixbuf-2.0, pango-1.0, gobject-2.0, gmodule-2.0, glib-2.0
- Webkit development files (for the Linux version)
- Tecmake 4 - http://webserver2.tecgraf.puc-rio.br/tecmake/

Additionally, the following files need to be copied from IUP version 3.23 as they are required by IUP 3.24 but not present for some reason:

- "iupmatrixex.h"
- The directory "srcmatrixex" and all its files and subdirectories.


### CD Canvas Draw

CD is a library/toolkit for doing vector graphics using the native GDI on Windows and XLIB on Linux.  It is licenced using the MIT Licence and is available for download from http://webserver2.tecgraf.puc-rio.br/cd .  If a precompiled version does not exist for your system, you will need to compile it from the sources.  The Genie Machine uses CD Canvas Draw version 3.11.1.  This version requires the following dependencies when it is compiled from sources:

- FreeType (should be included in the package)
- zLib (should be included in the package)
- pdflib (should be included in the package)
- ftgl (should be included in the package)
- OpenGL development files
- FTGL development files
- Standard Windows development libraries (for the Windows version): gdi32, user32, comdlg32, gdiplus
- Lua 5.1
- X11 libraries (for the Linux version)
- fontconfig library (for the Linux version)
- Xrender and Xft libraries (for the Linux version)
- Cairo libraries (for the Linux version): pangocairo-1.0, cairo 
- Parts of IM Image Library, specifically imlua51 (see below)
- Tecmake 4 - http://webserver2.tecgraf.puc-rio.br/tecmake/


### IM Image Library

The IM Image Library is a library/toolkit for image representation, storage, capture and processing that runs in Windows and Linux.  It is licenced using the MIT Licence and is available for download from http://webserver2.tecgraf.puc-rio.br/im .  If a precompiled version does not exist for your system, you will need to compile it from the sources.  The Genie Machine uses IM version 3.12.  This version requires the following dependencies when it is compiled from sources:

- libjpeg (should be included in the package)
- libpng (should be included in the package)
- libtiff (should be included in the package)
- zLib (should be included in the package)
- liblzf (should be included in the package)
- libexif (should be included in the package)
- libJasper (should be included in the package)
- fftw (should be included in the package)
- vfw32 (for the Windows version)
- wmvcore (for the Windows version)
- NCSEcw library
- strmiids (for the Windows version)
- Lua 5.1
- Tecmake 4 - http://webserver2.tecgraf.puc-rio.br/tecmake/


### SQLite 3

SQLite 3 is a small and lightweight but powerful single-file relational database managament system.  It is public domain and can be downloaded from https://sqlite.org/index.html .  It comes as a C file which will need to be compiled for your system into a static library before it can be linked with the Genie Machine.  It has no external dependencies.


