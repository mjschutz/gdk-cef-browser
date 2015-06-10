/*
 * Copyright (c) 2015 Mauro Joel Schütz
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <cefrender.h>
#include <windows.h>
#include "callbacks.h"
#include <iostream>
RenderHandler::RenderHandler(WindowContext* windowContext)
        : windowContext(windowContext)
{
}

bool RenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
	if (windowContext->window == nullptr) {
		rect = CefRect(0, 0, windowContext->attributes.width, windowContext->attributes.height);
		return true;
	}
	
	rect = CefRect(0, 0, gdk_window_get_width(windowContext->window), gdk_window_get_height(windowContext->window));
	return true;
}

bool RenderHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
	if (windowContext->window == nullptr) {
		screen_info.rect = CefRect(0, 0, windowContext->attributes.width, windowContext->attributes.height);
		screen_info.available_rect = CefRect(0, 0, windowContext->attributes.width, windowContext->attributes.height);
		return true;
	}
	
	screen_info.rect = CefRect(0, 0, gdk_window_get_width(windowContext->window), gdk_window_get_height(windowContext->window));
	screen_info.available_rect = CefRect(0, 0, gdk_window_get_width(windowContext->window), gdk_window_get_height(windowContext->window));
	return true;
}

void RenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
{
	if (windowContext->window == nullptr) return;
	
	windowContext->browser_data.copyData(buffer, width, height);
	gdk_window_invalidate_rect(windowContext->window, NULL, TRUE);
}

void RenderHandler::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, CefRenderHandler::CursorType type, const CefCursorInfo& custom_cursor_info)
{
	if (windowContext->window == nullptr) return;
	
	GdkCursorType cursor_type = GDK_LEFT_PTR;
	
	switch (type)
	{
		case CT_CROSS:
			cursor_type = GDK_CROSS;
		break;
		case CT_HAND:
			cursor_type = GDK_HAND2;
		break;
		case CT_IBEAM:
			cursor_type = GDK_XTERM;
		break;
		case CT_WAIT:
			cursor_type = GDK_WATCH;
		break;
		case CT_HELP:
			cursor_type = GDK_QUESTION_ARROW;
		break;
		case CT_EASTPANNING:
		case CT_EASTRESIZE:
			cursor_type = GDK_RIGHT_SIDE;
		break;
		case CT_NORTHPANNING:
		case CT_NORTHRESIZE:
			cursor_type = GDK_TOP_SIDE;
		break;
		case CT_NORTHEASTPANNING:
		case CT_NORTHEASTRESIZE:
			cursor_type = GDK_TOP_RIGHT_CORNER;
		break;
		case CT_NORTHWESTPANNING:
		case CT_NORTHWESTRESIZE:
			cursor_type = GDK_TOP_LEFT_CORNER;
		break;
		case CT_SOUTHPANNING:
		case CT_SOUTHRESIZE:
			cursor_type = GDK_BOTTOM_SIDE;
		break;
		case CT_SOUTHEASTPANNING:
		case CT_SOUTHEASTRESIZE:
			cursor_type = GDK_BOTTOM_RIGHT_CORNER;
		break;
		case CT_SOUTHWESTPANNING:
		case CT_SOUTHWESTRESIZE:
			cursor_type = GDK_BOTTOM_LEFT_CORNER;
		break;
		case CT_WESTPANNING:
		case CT_WESTRESIZE:
			cursor_type = GDK_LEFT_SIDE;
		break;
		case CT_NORTHSOUTHRESIZE:
		case CT_ROWRESIZE:
			cursor_type = GDK_SB_V_DOUBLE_ARROW;
		break;
		case CT_COLUMNRESIZE:
		case CT_EASTWESTRESIZE:
			cursor_type = GDK_SB_H_DOUBLE_ARROW;
		break;
		case CT_MIDDLEPANNING:
		case CT_NORTHEASTSOUTHWESTRESIZE:
		case CT_NORTHWESTSOUTHEASTRESIZE:
		case CT_MOVE:
			cursor_type = GDK_FLEUR;
		break;
		case CT_CELL:
			cursor_type = GDK_PLUS;
		break;
		case CT_PROGRESS:
			cursor_type = GDK_CLOCK;
		break;
		case CT_NOTALLOWED:
			cursor_type = GDK_X_CURSOR;
		break;
		case CT_NODROP:
		case CT_NONE:
			cursor_type =  GDK_BLANK_CURSOR;
		break;
		case CT_ALIAS:
			cursor_type = GDK_TARGET;
		break;
		case CT_COPY:
			cursor_type = GDK_TCROSS;
		break;
		/*
		case CT_VERTICALTEXT:
			cursor_type = GDK_XTERM;
		break;
		case CT_CONTEXTMENU:
			cursor_type = GDK_XTERM;
		break;
		case CT_ZOOMIN:
			cursor_type = GDK_XTERM;
		break;
		case CT_ZOOMOUT:
			cursor_type = GDK_XTERM;
		break;
		case CT_GRAB:
			cursor_type = GDK_XTERM;
		break;
		case CT_GRABBING:
			cursor_type = GDK_XTERM;
		break;
		case CT_CUSTOM:
			cursor_type = GDK_XTERM;
		break;*/
		case CT_POINTER:
		default:
			cursor_type = GDK_LEFT_PTR;
	}
	gdk_window_set_cursor(windowContext->window, gdk_cursor_new_for_display(gdk_window_get_display(windowContext->window), cursor_type));
}

void RenderHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	windowContext->browser = browser;
	windowContext->browser->GetHost()->WasResized();
	windowContext->browser->GetHost()->SetMouseCursorChangeDisabled(false);
	windowContext->browser->GetHost()->SendFocusEvent(true);
}

bool RenderHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             const CefString& target_url,
                             const CefString& target_frame_name,
                             WindowOpenDisposition target_disposition,
                             bool user_gesture,
                             const CefPopupFeatures& popupFeatures,
                             CefWindowInfo& windowInfo,
                             CefRefPtr<CefClient>& client,
                             CefBrowserSettings& settings,
                             bool* no_javascript_access)
{
	if (windowContext->window == nullptr) return true;
	
	WindowContext* wContext = new WindowContext;
	windowFactory.addWindowContext(wContext);
	
	*no_javascript_access = false;

	wContext->attributes.event_mask = GDK_ALL_EVENTS_MASK;
	wContext->attributes.window_type = GDK_WINDOW_TOPLEVEL;
	wContext->attributes.wclass = GDK_INPUT_OUTPUT;
	wContext->attributes.x = windowInfo.x < 0 ? 0 : windowInfo.x;
	wContext->attributes.y = windowInfo.y < 0 ? 0 : windowInfo.y;
	wContext->attributes.width = windowInfo.width < 0 ? 500 : windowInfo.width;
	wContext->attributes.height = windowInfo.height < 0 ? 500 : windowInfo.height;
	
	windowInfo.SetAsWindowless(nullptr, false); // false means no transparency (site background colour)
	wContext->browserClient = client = new BrowserClient(new RenderHandler(wContext));
	
	wContext->showWindow = true;

	return false;
}
