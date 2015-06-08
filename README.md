This is a web browser project using Chromium as a motor engine via [CEF (Chromium Embedded Framework)](https://cefbuilds.com) off-screen, all windowing and user interface events is based on [GDK](https://developer.gnome.org/gdk3/3.16/) without using of [GTK+](http://www.gtk.org).

The aim of this project is to serve as source code example of:

* Using GDK only for UI
* Using CEF off-screen in Cairo Graphics surfaces
* and of course use CEF as a GUI

Build considerations:

* Cygwin/MSYS2 makefile