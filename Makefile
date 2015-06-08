# This Makefile will build the MinGW Win32 application.

ARCH = $(shell gcc -dumpmachine)

HEADERS = include/resource.h include/callbacks.h include/cefrender.h
OBJS =	obj/resource.o obj/gdkmain.o obj/callbacks.o obj/cefrender.o 
INCLUDE_DIRS = -I./include -I/usr/local/include -I/c/builds/cef/include -I/c/builds/cef \
	-I/usr/local/include/cairo -I/usr/local/include/pango-1.0 -I/usr/local/include/gtk-3.0 \
	-I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include -I/usr/local/include/gdk-pixbuf-2.0

WARNS = -Wall -g

CXX = g++
LDFLAGS = -s -Wl,--subsystem,windows -mwindows -mconsole -L/usr/local/lib -lcairo -L/c/builds/cef/Release -L/c/builds/cef/${ARCH}-obj -lcef_dll -lcef -lgdk-3 -lglib-2.0.dll
RC = windres

# Compile ANSI build only if CHARSET=ANSI
ifeq (${CHARSET}, ANSI)
  CFLAGS= -O3 -std=c++11 -D _WIN32_IE=0x0500 -D WINVER=0x500 ${WARNS}
else
  CFLAGS= -O3 -std=c++11 -D UNICODE -D _UNICODE -D _WIN32_IE=0x0500 -D WINVER=0x500 ${WARNS}
endif


all: GdkCef.exe

GdkCef.exe: ${OBJS}
	${CXX} -o "$@" ${OBJS} ${LDFLAGS}

clean:
	rm obj/*.o "GdkCef.exe"

obj/%.o: src/%.cpp ${HEADERS}
	${CXX} ${CFLAGS} ${INCLUDE_DIRS} -c $< -o $@

obj/resource.o: res/resource.rc res/Application.manifest res/Application.ico include/resource.h
	${RC} -I.\include -I.\res -i $< -o $@