/*
 * Copyright (c) 2015 Mauro Joel Schütz
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <vector>
#include <algorithm>

#include <gdk/gdk.h>
#include <glib.h>
#include <cairo/cairo.h>
#include "cefrender.h"

// Rename this and create a new file for it
struct WindowContext
{
	GdkWindow* window;
	CefRefPtr<CefBrowser> browser;
	CefRefPtr<CefClient> browserClient;
	RenderHandler::ScreenBuffer browser_data;
	bool showWindow;
	GdkWindowAttr attributes;
	
	WindowContext():
		window(nullptr),
		browser(nullptr),
		browserClient(nullptr),
		showWindow(false)
	{
	}
	
	~WindowContext()
	{
		if (window != nullptr)
			gdk_window_destroy(window);
		
		browserClient = nullptr;
		browser = nullptr;
	}
};

// this too
class WindowFactory
{
	std::vector<WindowContext*> windows;
	
	struct find_context : std::unary_function<WindowContext*, bool>
	{
		GdkWindow* window;
		
		find_context(GdkWindow* window): window(window) { }
		
		bool operator()(WindowContext* const& windowContext) const
		{
			return windowContext->window == window;
		}
	};
	
public:
	GdkWindow* mainWindow()
	{
		if (windows.empty())
			return NULL;
		
		return windows[0]->window;
	}
	
	WindowContext* getWindowContext(GdkWindow* window)
	{

		std::vector<WindowContext*>::iterator it = std::find_if(windows.begin(), windows.end(), find_context(window));
		
		if (it == windows.end())
			return NULL;
		
		return *it;
	}
	
	void showWindows()
	{
		for (std::vector<WindowContext*>::iterator wContextIt = windows.begin(); wContextIt != windows.end(); wContextIt++)
			if ((*wContextIt)->showWindow)
			{
				(*wContextIt)->showWindow = false;
				(*wContextIt)->window = gdk_window_new(NULL, &(*wContextIt)->attributes, 0);
				gdk_window_show((*wContextIt)->window);
			}
	}
	
	GdkWindow* createWindow(GdkWindow *parent,
                GdkWindowAttr *attributes,
                gint attributes_mask, const char* web_addr)
	{
		
		WindowContext* windowContext = new WindowContext;
		
		windowContext->window = gdk_window_new(parent, attributes, attributes_mask);
		
		RenderHandler* renderHandler = new RenderHandler(windowContext);
		{
			CefWindowInfo window_info;
			CefBrowserSettings browserSettings;

			window_info.SetAsWindowless(nullptr, false); // false means no transparency (site background colour)

			windowContext->browserClient = new BrowserClient(renderHandler);

			windowContext->browser = CefBrowserHost::CreateBrowserSync(window_info, windowContext->browserClient.get(), web_addr, browserSettings, nullptr);
			windowContext->browser->GetHost()->SetMouseCursorChangeDisabled(false);
			windowContext->browser->GetHost()->SendFocusEvent(true);
		}
		
		windows.push_back(windowContext);
		return windowContext->window;
	}
	
	void addWindowContext(WindowContext* windowContext)
	{
		windows.push_back(windowContext);
	}
	
	void destroyAll()
	{	
		for (std::vector<WindowContext*>::iterator wContextIt = windows.begin(); wContextIt != windows.end(); wContextIt++)
			delete *wContextIt;

		windows.clear();
	}
	
	void destroyWindow(GdkWindow* window)
	{
		WindowContext* wContext = getWindowContext(window);
		if (wContext == NULL)
			return;

		windows.erase(std::remove(windows.begin(), windows.end(), wContext), windows.end());
		delete wContext;
	}
};

// Window procedure for our main window.
void event_func(GdkEvent *ev, gpointer data);

extern GMainLoop *mainloop;
extern WindowFactory windowFactory;

#endif
