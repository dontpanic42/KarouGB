#include "screenwidget.h"
#include <QPainter>
#include <algorithm>
#include <iostream>

/// <summary>
/// Interval to average the fps over
/// </summary>
#define FPS_COUNTER_INTERVAL_MILLIS 500

namespace ui
{
	ScreenWidget::ScreenWidget(int buffer_width, int buffer_height, QWidget * parent)
		: QWidget(parent)
		, buffer_width(buffer_width)
		, buffer_height(buffer_height)
		, old_frametime(std::chrono::system_clock::now())
		, old_update_frametime(std::chrono::system_clock::now())
		, fps_sum(0)
		, fps_avg(0)
		, fps_tick_count(0)
	{
		// Array with four chars for each pixel (r, g, b, a)
		framebuffer = std::shared_ptr<u08i>(new u08i[buffer_width * buffer_height * 4], std::default_delete<u08i[]>());
		// QImage that wraps the framebuffer to use with qt
		framebuffer_image = std::make_shared<QImage>(framebuffer.get(), buffer_width, buffer_height, QImage::Format_RGB32);
	}

	/// <summary>
	/// Force drawing the buffer
	/// </summary>
	void ScreenWidget::forceRepaint()
	{
		repaint();
	}

	void ScreenWidget::paintEvent(QPaintEvent * event)
	{
		QPainter painter(this);

		// Calculate the scale factor to make the render as big as possible
		float scaleX = ((float)width()) / (float)buffer_width;
		float scaleY = ((float)height()) / (float)buffer_height;
		// We want the scaling to be proportional and not overflowing the widget
		float scale = std::min(scaleX, scaleY);

		// Calculate the final width
		int drawWidth = buffer_width * scale;
		// Calculate the final height
		int drawHeight = buffer_height * scale;

		// Horizontally center the render (if render width < widget width)
		int offsetX = width() - drawWidth;
		offsetX = offsetX ? offsetX / 2 : 0;
		// Vertically center the render (if render height < widget height)
		int offsetY = height() - drawHeight;
		offsetY = offsetY ? offsetY / 2 : 0;
		// Define the drawing area (scaled)
		QRect drawArea(offsetX, offsetY, drawWidth, drawHeight);
		// Draw the buffer
		painter.drawImage(drawArea, *framebuffer_image);

		// Advance the framerate
		tickFrameCounter();
		// Draw the framerate
		painter.drawText(10, 20, (std::string("FPS: ") + std::to_string((int)getAverageFramerate())).c_str());
	}

	/// <summary>
	/// Draws a single pixel on the screen
	/// </summary>
	void ScreenWidget::draw(u08i x, u08i y, u08i r, u08i g, u08i b)
	{
		int offset = 4 * (y * buffer_width + x);
		framebuffer.get()[offset+0] = r;
		framebuffer.get()[offset+1] = g;
		framebuffer.get()[offset+2] = b;
	}

	/// <summary>
	/// Counts a single frame for calculating the average framerate
	/// </summary>
	void ScreenWidget::tickFrameCounter()
	{
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = now - old_frametime;
		old_frametime = std::chrono::system_clock::now();

		double fps = 1 / elapsed_seconds.count();
		fps_sum += fps;
		fps_tick_count++;
	}

	/// <summary>
	/// Returns the average framerate. The time interval the average framerate is calculated over is controlled
	/// by <code>FPS_COUNTER_INTERVAL_MILLIS</code>
	/// </summary>
	double ScreenWidget::getAverageFramerate()
	{
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = now - old_update_frametime;

		if (std::chrono::duration<double, std::milli>(elapsed_seconds).count() > FPS_COUNTER_INTERVAL_MILLIS)
		{
			old_update_frametime = now;
			fps_avg = fps_sum == 0 || fps_tick_count == 0 ? 0.0 : fps_sum / fps_tick_count;
			fps_sum = 0.0;
			fps_tick_count = 0;
		}

		return fps_avg;
	}
}