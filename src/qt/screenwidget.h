#ifndef __screenwidget_h
#define __screenwidget_h

#include <QWidget>
#include <QImage>
#include <memory>
#include <chrono>
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

		std::chrono::system_clock::time_point old_frametime;
		std::chrono::system_clock::time_point old_update_frametime;
		double fps_sum;
		double fps_avg;
		int fps_tick_count;

		void tick_frame_counter();
		double get_average_frame_rate();

	public:

		ScreenWidget(int buffer_width, int buffer_height, QWidget * parent = nullptr);

		void force_repaint();
		void paintEvent(QPaintEvent * event);
		void draw(u08i x, u08i y, u08i r, u08i g, u08i b);
	};
}

#endif