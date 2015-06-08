/*
 * Copyright (c) 2015 Mauro Joel Schütz
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gdk/gdk.h>
#include <glib.h>
#include <cairo/cairo.h>
#include "cefrender.h"

// Window procedure for our main window.
void event_func(GdkEvent *ev, gpointer data);

extern RenderHandler::ScreenBuffer browser_data;
extern CefRefPtr<CefBrowser> browser;
extern GMainLoop *mainloop;

#endif
