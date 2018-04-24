#ifndef __screenwidget_h
#define __screenwidget_h

#include <QWidget>
#include <QImage>
#include <memory>
#include "../types.h"

namespace ui {
	class ScreenWidget 
		: public QWidget 
	{

		Q_OBJECT

	private:

		std::shared_ptr<u08i> framebuffer;
		std::shared_ptr<QImage> framebuffer_image;

		int buffer_width;
		int buffer_height;

	public:

		ScreenWidget(int buffer_width, int buffer_height, QWidget * parent = nullptr);

		void force_repaint();
		void paintEvent(QPaintEvent * event);
		void draw(u08i x, u08i y, u08i r, u08i g, u08i b);
	};
}

#endif