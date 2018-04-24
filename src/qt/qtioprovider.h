#ifndef __qtioprovider_h
#define __qtioprovider_h

#include "../io/io_provider.h"
#include "screenwidget.h"
#include "keyeventmapper.h"

namespace ui
{
	/// <summary>
	/// This class acts as a bridge between the emulation and the main window. That way, we don't need to give the emualtion
	/// a direct reference to the window. We should not give it a direct reference since the emulation is not in control of the
	/// lifetime of the window.
	/// </summary>
	class QtIoProvider : public emu::IOProvider
	{
	private:
		ScreenWidget * screen_widget;
		KeyEventMapper * key_event_mapper;
		bool destroyed;
	public:
		QtIoProvider(ScreenWidget * screen_widget, KeyEventMapper * key_event_mapper);
		void destroy();

		void init(const std::string & wintitle);
		void poll();

		void draw(u08i x, u08i y, u08i r, u08i g, u08i b);
		void display();

		bool isClosed();

		void printDebugString(const std::string & str);

		void registerButtonCallback(Button btn, on_press_t onPress, on_release_t onRelease);

		void handleError(const std::exception & exception);
	};
}

#endif //defined(__qtioprovider_h)