#include "qtioprovider.h"
#include <QMessageBox>

namespace ui
{
	QtIoProvider::QtIoProvider(ScreenWidget * screen_widget, KeyEventMapper * key_event_mapper)
		: screen_widget(screen_widget)
		, key_event_mapper(key_event_mapper)
		, destroyed(false)
	{
	}

	/// <summary>
	/// Called by the parent to indicate that io is no longer possible
	/// </summary>
	void QtIoProvider::destroy()
	{
		screen_widget = nullptr;
		key_event_mapper = nullptr;
		destroyed = true;
	}

	/// <summary>
	/// Stub (not implemented right now)
	/// </summary>
	void QtIoProvider::init(const std::string & wintitle)
	{
	}

	/// <summary>
	/// Stub (not required for qt)
	/// </summary>
	void QtIoProvider::poll()
	{
	}

	/// <summary>
	/// Draws a single pixel
	/// </summary>
	void QtIoProvider::draw(u08i x, u08i y, u08i r, u08i g, u08i b)
	{
		if (!destroyed)
		{
			screen_widget->draw(x, y, r, g, b);
		}
	}

	/// <summary>
	/// Triggers a new frame to be displayed.
	/// </summary>
	void QtIoProvider::display()
	{
		if (!destroyed)
		{
			screen_widget->force_repaint();
		}
	}

	/// <summary>
	/// Returns whether or not the emulator window is still visible
	/// </summary>
	bool QtIoProvider::isClosed()
	{
		return destroyed;
	}

	/// <summary>
	/// Stub (not implemented right now)
	/// </summary>
	void QtIoProvider::printDebugString(const std::string & str)
	{
	}

	/// <summary>
	/// Registers a new key event handler
	/// </summary>
	void QtIoProvider::registerButtonCallback(Button btn, on_press_t onPress, on_release_t onRelease)
	{
		if (!destroyed)
		{
			key_event_mapper->registerButtonCallback(btn, onPress, onRelease);
		}
	}

	/// <summary>
	/// Shows an exception in a message box
	/// </summary>
	void QtIoProvider::handleError(const std::exception & exception) 
	{
		if (!destroyed)
		{
			QMessageBox msgBox;
			msgBox.setText("An error occured.");
			msgBox.setInformativeText("An error occured during emulation");
			msgBox.setDetailedText(exception.what());
			msgBox.setIcon(QMessageBox::Icon::Critical);
			msgBox.show();
		}
	}
}