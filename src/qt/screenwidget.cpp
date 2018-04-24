#include "screenwidget.h"
#include <QPainter>
#include <algorithm>

namespace ui
{
	ScreenWidget::ScreenWidget(int buffer_width, int buffer_height, QWidget * parent)
		: QWidget(parent)
		, buffer_width(buffer_width)
		, buffer_height(buffer_height)
	{
		framebuffer = std::shared_ptr<u08i>(new u08i[buffer_width * buffer_height * 4], std::default_delete<u08i[]>());
		framebuffer_image = std::make_shared<QImage>(framebuffer.get(), buffer_width, buffer_height, QImage::Format_RGB32);
	}

	void ScreenWidget::force_repaint()
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
	}

	void ScreenWidget::draw(u08i x, u08i y, u08i r, u08i g, u08i b)
	{
		int offset = 4 * (y * buffer_width + x);
		framebuffer.get()[offset+0] = r;
		framebuffer.get()[offset+1] = g;
		framebuffer.get()[offset+2] = b;
	}
}