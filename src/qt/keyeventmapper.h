#ifndef __keyeventmapper_h
#define __keyeventmapper_h

#include <QObject>
#include <unordered_map>
#include <utility>
#include "../types.h"
#include "../io/io_provider.h"

namespace ui {

	class KeyEventMapper : public QObject
	{
		Q_OBJECT
	private:

		enum event_type
		{
			PRESS,
			RELEASE
		};

		static std::unordered_map<Qt::Key, Button> keymap;
		std::unordered_map<Button, std::pair<emu::IOProvider::on_press_t, emu::IOProvider::on_release_t>> callbacks;
		bool handleKeyEvent(event_type type, Qt::Key qt_key);

	public:
		KeyEventMapper(QObject * parent = nullptr);
		bool eventFilter(QObject * object, QEvent * event);
		void registerButtonCallback(Button btn, emu::IOProvider::on_press_t onPress, emu::IOProvider::on_release_t onRelease);
	};

}
#endif //defined(__keyeventmapper_h)