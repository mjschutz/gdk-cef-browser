/*
 * Copyright (c) 2015 Mauro Joel Schütz
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _CEFRENDER_H
#define _CEFRENDER_H

#include <cef_app.h>
#include <cef_client.h>
#include <cef_render_handler.h>
#include <cairo/cairo.h>
#include <gdk/gdk.h>

class WindowContext;

class RenderHandler : public CefRenderHandler, public CefLifeSpanHandler
{
public:
	struct ScreenBuffer
	{
		int _width;
		int _height;
		int _length;
		char* _data;
		
		ScreenBuffer(): _width(0), _height(0), _length(0), _data(NULL)
		{
			
		}
		
		~ScreenBuffer()
		{
			if (_data != NULL)
				delete _data;
		}
		
		void copyData(const void* data, int width, int height)
		{
			int len = width*4*height;
			
			if (len > _length)
			{
				if (_data != NULL)
					delete _data;
				_length = len;
				_data = new char[len];
			}
			
			_width = width;
			_height = height;
			memcpy(_data, data, len);
		}
	};
	
    WindowContext* windowContext;

    RenderHandler(WindowContext* windowContext);

    // CefRenderHandler interface
public:
    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect);
	bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info);
    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height);
	void OnCursorChange( CefRefPtr< CefBrowser > browser, CefCursorHandle cursor, CefRenderHandler::CursorType type, const CefCursorInfo& custom_cursor_info);
	bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             const CefString& target_url,
                             const CefString& target_frame_name,
                             WindowOpenDisposition target_disposition,
                             bool user_gesture,
                             const CefPopupFeatures& popupFeatures,
                             CefWindowInfo& windowInfo,
                             CefRefPtr<CefClient>& client,
                             CefBrowserSettings& settings,
                             bool* no_javascript_access);
	virtual void OnAfterCreated( CefRefPtr< CefBrowser > browser );

    // CefBase interface
public:
    IMPLEMENT_REFCOUNTING(RenderHandler);
};


// for manual render handler
class BrowserClient : public CefClient
{
public:
    BrowserClient(RenderHandler *renderHandler)
        : m_renderHandler(renderHandler)
    {;}

    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() {
        return m_renderHandler;
    }
	
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() {
		return m_renderHandler;
	}

    CefRefPtr<RenderHandler> m_renderHandler;

    IMPLEMENT_REFCOUNTING(BrowserClient);
};

#endif