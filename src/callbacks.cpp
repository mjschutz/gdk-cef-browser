/*
 * Copyright (c) 2015 Mauro Joel Schütz
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "callbacks.h"
#include <iostream>

RenderHandler::ScreenBuffer browser_data;
GMainLoop *mainloop;

int GetCefMouseModifiers(GdkDisplay* display, guint state) {
  int modifiers = 0;
  
  if (state & GDK_CONTROL_MASK)
    modifiers |= EVENTFLAG_CONTROL_DOWN;
  if (state & GDK_SHIFT_MASK)
    modifiers |= EVENTFLAG_SHIFT_DOWN;
  if (state & GDK_MOD1_MASK)
    modifiers |= EVENTFLAG_ALT_DOWN;
  if (state & GDK_BUTTON1_MASK)
    modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
  if (state & GDK_BUTTON2_MASK)
    modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
  if (state & GDK_BUTTON3_MASK)
    modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

  GdkKeymap* keymap = gdk_keymap_get_for_display(display);

  // Low bit set from GetKeyState indicates "toggled".
  if (gdk_keymap_get_num_lock_state(keymap))
    modifiers |= EVENTFLAG_NUM_LOCK_ON;
  if (gdk_keymap_get_caps_lock_state(keymap))
    modifiers |= EVENTFLAG_CAPS_LOCK_ON;

  return modifiers;
}

int GetCefKeyboardModifiers(GdkDisplay* display, guint state, guint keyval) {
  int modifiers = 0;
  
  if (state & GDK_CONTROL_MASK)
    modifiers |= EVENTFLAG_CONTROL_DOWN;
  if (state & GDK_SHIFT_MASK)
    modifiers |= EVENTFLAG_SHIFT_DOWN;
  if (state & GDK_MOD1_MASK)
    modifiers |= EVENTFLAG_ALT_DOWN;

  // Low bit set from GetKeyState indicates "toggled".
  GdkKeymap* keymap = gdk_keymap_get_for_display(display);

  // Low bit set from GetKeyState indicates "toggled".
  if (gdk_keymap_get_num_lock_state(keymap))
    modifiers |= EVENTFLAG_NUM_LOCK_ON;
  if (gdk_keymap_get_caps_lock_state(keymap))
    modifiers |= EVENTFLAG_CAPS_LOCK_ON;

  switch (keyval) {
  case GDK_KEY_KP_Enter:
  case GDK_KEY_KP_Insert:
  case GDK_KEY_KP_Delete:
  case GDK_KEY_KP_Home:
  case GDK_KEY_KP_End:
  case GDK_KEY_KP_Prior:
  case GDK_KEY_KP_Next:
  case GDK_KEY_KP_Up:
  case GDK_KEY_KP_Down:
  case GDK_KEY_KP_Left:
  case GDK_KEY_KP_Right:
      modifiers |= EVENTFLAG_IS_KEY_PAD;
    break;
  case GDK_KEY_Num_Lock:
  case GDK_KEY_KP_0:
  case GDK_KEY_KP_1:
  case GDK_KEY_KP_2:
  case GDK_KEY_KP_3:
  case GDK_KEY_KP_4:
  case GDK_KEY_KP_5:
  case GDK_KEY_KP_6:
  case GDK_KEY_KP_7:
  case GDK_KEY_KP_8:
  case GDK_KEY_KP_9:
  case GDK_KEY_KP_Divide:
  case GDK_KEY_KP_Multiply:
  case GDK_KEY_KP_Subtract:
  case GDK_KEY_KP_Add:
  case GDK_KEY_KP_Decimal :
  case GDK_KEY_Clear:
    modifiers |= EVENTFLAG_IS_KEY_PAD;
    break;
  case GDK_KEY_Shift_L:
  case GDK_KEY_Control_L:
  case GDK_KEY_Alt_L:
  case GDK_KEY_Meta_L:
	modifiers |= EVENTFLAG_IS_LEFT;
  break;
  case GDK_KEY_Shift_R:
  case GDK_KEY_Control_R:
  case GDK_KEY_Alt_R:
  case GDK_KEY_Meta_R:
	modifiers |= EVENTFLAG_IS_RIGHT;
  break;
  }
  return modifiers;
}

void event_func(GdkEvent *ev, gpointer data)
{
	GdkWindow* hWindow = ((GdkEventAny*)ev)->window;
	
	if (gdk_window_is_destroyed(hWindow)) return;
	
	WindowContext* windowContext = windowFactory.getWindowContext(hWindow);
	RenderHandler::ScreenBuffer* browser_data = &windowContext->browser_data;
	CefBrowser* browser = windowContext->browser;
		
   switch(ev->type) {
	  case GDK_CONFIGURE:	
		browser->GetHost()->WasResized();
	  break;
	  case GDK_EXPOSE:
	  {
		if (browser_data->_data == NULL) return;
		
		browser->GetHost()->NotifyMoveOrResizeStarted();
		
		gdk_window_begin_paint_rect(hWindow, &((GdkEventExpose*)ev)->area);
		
		cairo_surface_t *paint_surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, browser_data->_width, browser_data->_height);
	
		cairo_surface_flush(paint_surface);
		unsigned char* data = cairo_image_surface_get_data(paint_surface);
		std::memcpy(data, browser_data->_data, browser_data->_width*4*browser_data->_height);
		cairo_surface_mark_dirty(paint_surface);
		
		cairo_t* cr = gdk_cairo_create(hWindow);
		
		cairo_set_source_rgba (cr, 0xFF, 0xFF, 0xFF, 0xFF);
		cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint (cr);
		
		int _w = gdk_window_get_width(hWindow);
		int _h = gdk_window_get_height(hWindow);
		
		cairo_set_source_surface (cr, paint_surface, 0, 0);
		cairo_rectangle (cr, 0, 0, browser_data->_width > _w ? _w : browser_data->_width, browser_data->_height > _h ? _h : browser_data->_height);
		cairo_fill(cr);
		
		cairo_destroy(cr);
		
		cairo_surface_destroy(paint_surface);
		
		gdk_window_end_paint(hWindow);
		
		browser->GetHost()->WasResized();
	  }
	  break;
      case GDK_KEY_PRESS:
	  {
		GdkEventKey * kev = (GdkEventKey*)ev;
		CefKeyEvent keyEvent;
		keyEvent.windows_key_code = kev->hardware_keycode;
		keyEvent.modifiers = GetCefKeyboardModifiers(gdk_window_get_display(kev->window), kev->state, kev->keyval);
		keyEvent.is_system_key = kev->state & GDK_MOD1_MASK;
		
		keyEvent.type = KEYEVENT_KEYDOWN;
		browser->GetHost()->SendKeyEvent(keyEvent);
		
		keyEvent.windows_key_code = gdk_keyval_to_unicode(kev->keyval);
		keyEvent.type = KEYEVENT_CHAR;
		browser->GetHost()->SendKeyEvent(keyEvent);
	  }
         break;
		 
	  case GDK_KEY_RELEASE:
	  {
		GdkEventKey * kev = (GdkEventKey*)ev;
		CefKeyEvent keyEvent;
		keyEvent.character = 0;
		keyEvent.windows_key_code = kev->hardware_keycode;
		keyEvent.type = KEYEVENT_KEYUP;
		keyEvent.is_system_key = kev->state & GDK_MOD1_MASK;
		keyEvent.modifiers = GetCefKeyboardModifiers(gdk_window_get_display(kev->window), kev->state, kev->keyval);

		browser->GetHost()->SendKeyEvent(keyEvent);
	  }
	  break;
	  
	case GDK_MOTION_NOTIFY:
	{
		GdkEventMotion* mev = (GdkEventMotion*)ev;
		CefMouseEvent mouseEvent;
		mouseEvent.x = mev->x;
		mouseEvent.y = mev->y;
		mouseEvent.modifiers = GetCefMouseModifiers(gdk_window_get_display(mev->window), mev->state);
		
		browser->GetHost()->SendMouseMoveEvent(mouseEvent, false);
		
		if (mev->is_hint)
			gdk_event_request_motions(mev);
		break;
	}
	
	case GDK_BUTTON_RELEASE:	
	case GDK_2BUTTON_PRESS:
	case GDK_BUTTON_PRESS:
	{
		GdkEventButton* bev = (GdkEventButton*)ev;
		
		CefMouseEvent mouseEvent;
		mouseEvent.x = bev->x;
		mouseEvent.y = bev->y;
		mouseEvent.modifiers = GetCefMouseModifiers(gdk_window_get_display(bev->window), bev->state);
		
		//bev->buttom => 1 left | 2 middle | 3 right
		//cef_mouse_button_type_t => 0 left | 1 middle | 2 right
		browser->GetHost()->SendMouseClickEvent(mouseEvent, (cef_mouse_button_type_t)(bev->button-1), ev->type == GDK_BUTTON_RELEASE,
			ev->type == GDK_2BUTTON_PRESS ? 2 : ev->type == GDK_BUTTON_PRESS ? 1 : 0);
		browser->GetHost()->SendFocusEvent(true);
		
		break;
	}
	
	case GDK_SCROLL:
	{
		GdkEventScroll* sev = (GdkEventScroll*)ev;
		
		CefMouseEvent mouseEvent;
		mouseEvent.x = sev->x;
		mouseEvent.y = sev->y;
		mouseEvent.modifiers = GetCefMouseModifiers(gdk_window_get_display(sev->window), sev->state);
		
		int deltaX = sev->direction == GDK_SCROLL_LEFT ? 1 : -1, deltaY = sev->direction == GDK_SCROLL_UP ? 1 : -1;
		browser->GetHost()->SendMouseWheelEvent(mouseEvent, deltaX*120, deltaY*120);
		break;
	}
		 
      case GDK_DELETE:
		if (windowFactory.mainWindow() == hWindow)
		{
			g_main_loop_quit(mainloop);
		}
		
		browser->GetHost()->CloseBrowser(false);
		windowFactory.destroyWindow(hWindow);
         break;
	  default:
		 break;
   }
}
