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

RenderHandler::RenderHandler(GdkWindow* hWnd, RenderHandler::ScreenBuffer &rbuffer)
        : render_buffer(rbuffer),
		hWindow(hWnd)
{
}

bool RenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
	rect = CefRect(0, 0, gdk_window_get_width(hWindow), gdk_window_get_height(hWindow));
	return false;
}

bool RenderHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
	screen_info.rect = CefRect(0, 0, gdk_window_get_width(hWindow), gdk_window_get_height(hWindow));
	screen_info.available_rect = CefRect(0, 0, gdk_window_get_width(hWindow), gdk_window_get_height(hWindow));
	return false;
}

void RenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
{
	render_buffer.copyData(buffer, width, height);
	gdk_window_invalidate_rect(hWindow, NULL, TRUE);
}

void RenderHandler::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, CefRenderHandler::CursorType type, const CefCursorInfo& custom_cursor_info)
{
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
	gdk_window_set_cursor(hWindow, gdk_cursor_new_for_display(gdk_window_get_display(hWindow), cursor_type));
}
