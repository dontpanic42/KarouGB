#include <QEvent>
#include <QKeyEvent>
#include "keyeventmapper.h"

#include <iostream>

using namespace emu;

namespace ui
{
	KeyEventMapper::KeyEventMapper(QObject * parent)
		: QObject(parent)
	{
	}

	/// <summary>
	/// The emulators keymap. TBD: Use qt configuration to make this editable
	/// </summary>
	std::unordered_map<Qt::Key, Button> KeyEventMapper::keymap({
		{ Qt::Key::Key_A, BTN_LEFT },
		{ Qt::Key::Key_W, BTN_UP },
		{ Qt::Key::Key_W, BTN_RIGHT },
		{ Qt::Key::Key_S, BTN_DOWN },
		{ Qt::Key::Key_V, BTN_START },
		{ Qt::Key::Key_B, BTN_SELECT },
		{ Qt::Key::Key_J, BTN_A },
		{ Qt::Key::Key_K, BTN_B }
	});

	/// <summary>
	/// Calback filte method for all events
	/// </summary>
	bool KeyEventMapper::eventFilter(QObject * object, QEvent * event)
	{
		switch (event->type())
		{
			case QEvent::KeyPress:
			{
				QKeyEvent * key_event = static_cast<QKeyEvent *>(event);
				auto qt_key = static_cast<Qt::Key>(key_event->key());
				return handle_key_event(PRESS, qt_key);
			}
			case QEvent::KeyRelease:
			{
				QKeyEvent * key_event = static_cast<QKeyEvent *>(event);
				auto qt_key = static_cast<Qt::Key>(key_event->key());
				return handle_key_event(RELEASE, qt_key);
			}
			default:
			{
				return false;
			}
		}
	}

	/// <summary>
	/// Registers a callback for a specifc key (press and release at the same time)
	/// </summary>
	void KeyEventMapper::registerButtonCallback(Button btn, IOProvider::on_press_t onPress, IOProvider::on_release_t onRelease)
	{
		callbacks[btn] = std::make_pair(onPress, onRelease);
	}

	/// <summary>
	/// Generic key event handler that checks if the key is a known key and (if so) calls the emus callbacks
	/// accordingly.
	/// </summary>
	bool KeyEventMapper::handle_key_event(event_type type, Qt::Key qt_key)
	{
		// Check if the pressed key is a known key
		if (keymap.find(qt_key) != keymap.end())
		{
			Button emu_btn = keymap[qt_key];

			if (callbacks.find(emu_btn) != callbacks.end())
			{
				switch (type)
				{
					case PRESS: callbacks[emu_btn].first(emu_btn); break;;
					case RELEASE: callbacks[emu_btn].second(emu_btn); break;
				}
			}

			// We mark this key as handeled, even if we have no callback for it (yet)
			return true;
		}

		// This is not a button we handle in general
		return false;
	}
}