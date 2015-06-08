/*
 * Copyright (c) 2015 Mauro Joel Schütz
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "callbacks.h"

CefRefPtr<CefBrowser> browser;

gboolean MessageLoopTimeout (gpointer data) {
	CefDoMessageLoopWork();
	return TRUE;
}

int main(int argc, char **argv) {
   gdk_init(&argc, &argv);
     
  CefMainArgs args;
  
  {
	CefSettings settings;
	bool result = CefInitialize(args, settings, nullptr, nullptr);
	
	// CefInitialize creates a sub-proccess and executes the same executeable, as calling CefInitialize, if not set different in settings.browser_subprocess_path
	// if you create an extra program just for the childproccess you only have to call CefExecuteProcess(...) in it.
	if (!result)
	{
		return -1;
	}
  }

   GdkWindowAttr attr;
   attr.title = argv[0];
   attr.event_mask = GDK_ALL_EVENTS_MASK;
   attr.window_type = GDK_WINDOW_TOPLEVEL;
   attr.wclass = GDK_INPUT_OUTPUT;
   attr.width = 1000;
   attr.height = 700;
   GdkWindow *win = gdk_window_new(NULL, &attr, 0);
   
	RenderHandler* renderHandler = new RenderHandler(win, browser_data);
	
	// create browser-window
    CefRefPtr<BrowserClient> browserClient;
    {
        CefWindowInfo window_info;
        CefBrowserSettings browserSettings;

        // browserSettings.windowless_frame_rate = 60; // 30 is default

        // in linux set a gtk widget, in windows a hwnd. If not available set nullptr - may cause some render errors, in context-menu and plugins.
        //std::size_t windowHandle = 0;
        window_info.SetAsWindowless(nullptr, false); // false means no transparency (site background colour)

        browserClient = new BrowserClient(renderHandler);

        browser = CefBrowserHost::CreateBrowserSync(window_info, browserClient.get(), "http://www.google.com/", browserSettings, nullptr);
		browser->GetHost()->SetMouseCursorChangeDisabled(false);
		browser->GetHost()->SendFocusEvent(true);
    }
	
   gdk_window_show(win);
   
  // g_thread_new(NULL, CefMessaLoop, NULL);
  
  GMainContext *context = g_main_context_default();
  GSource *source =  g_timeout_source_new(10);

   gdk_event_handler_set(event_func, NULL, NULL); // this is used by GTK+ internally. We just use it for ourselves here
   mainloop = g_main_loop_new(context, FALSE); // we use the default main context because that's what GDK uses internally
   
   /* Set callback to be called in each timeout */
    g_source_set_callback(source,
                          (GSourceFunc)MessageLoopTimeout,
                          mainloop,
                          NULL);
    g_source_attach(source,
                    context);  
   g_main_loop_run(mainloop);
   gdk_window_destroy(win);
    g_source_destroy(source);
    g_source_unref(source);
    g_main_loop_unref(mainloop);
   
   browser = nullptr;
  browserClient = nullptr;
  CefShutdown();

   return 0;
}
