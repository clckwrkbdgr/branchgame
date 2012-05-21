#include <QtDebug>
#include <QtGui/QPainter>
#include <QtGui/QGradient>

#include "branchgame.h"
#include "spritegen.h"

namespace Sprites {

namespace Colors {
	const QColor back        = Qt::black;
	const QColor off         = Qt::blue;
	const QColor on          = Qt::red;
	const QColor fore        = Colors::off;
	const QColor transparent = QColor(0, 0, 0, 0);
	const QColor lamp        = QColor(255, 192, 0);
	const QColor lamp2       = QColor(128, 64, 0);
	const QColor lampBorder  = Qt::white;
	const QColor lampOff     = Qt::black;
	const QColor fadeLight   = QColor(0, 0, 0, 128);
	const QColor tube        = QColor(192, 192, 192);
	const QColor tube2       = QColor(32, 32, 32);
	const QColor tubeBorder  = Qt::gray;
};

int lampDiameter(int cellRadius) { return cellRadius * 3 / 8; }

const QColor & backgroundColor()
{
	return Colors::back;
}

QImage getShiningImage(int imageSize)
{
	QImage light(QSize(imageSize, imageSize), QImage::Format_ARGB32);
	light.fill(Colors::transparent.rgba());
	QPainter lightPainter(&light);

	QRadialGradient lightFlow(light.rect().center(), imageSize / 2, light.rect().center());
	lightFlow.setColorAt(0, Colors::lamp);
	lightFlow.setColorAt(1, Colors::fadeLight);

	lightPainter.setBrush(lightFlow);
	lightPainter.setPen(Qt::NoPen);
	lightPainter.drawEllipse(light.rect().adjusted(-2, -2, 2, 2)); // Widened rect for full-covered gradient filling.
	return light;
}

QImage getLightOnImage(int imageSize, int bulbSize)
{
	QImage light(QSize(imageSize, imageSize), QImage::Format_ARGB32);
	light.fill(Colors::transparent.rgba());
	QPainter lightPainter(&light);
	QRect lightbulbRect((imageSize - bulbSize) / 2, (imageSize - bulbSize) / 2, bulbSize, bulbSize);

	QRadialGradient lightOn(lightbulbRect.center(), bulbSize, lightbulbRect.center());
	lightOn.setColorAt(0, Colors::lamp);
	lightOn.setColorAt(1, Colors::lamp2);

	lightPainter.setPen(Colors::lampBorder);
	lightPainter.setBrush(lightOn);
	lightPainter.drawEllipse(lightbulbRect);
	return light;
}

QImage getLightOffImage(int imageSize, int bulbSize)
{
	QImage light(QSize(imageSize, imageSize), QImage::Format_ARGB32);
	light.fill(Colors::transparent.rgba());
	QPainter lightPainter(&light);
	QRect lightbulbRect((imageSize - bulbSize) / 2, (imageSize - bulbSize) / 2, bulbSize, bulbSize);

	lightPainter.setPen(Colors::lampBorder);
	lightPainter.setBrush(Colors::lampOff);
	lightPainter.drawEllipse(lightbulbRect);
	return light;
}

QImage getTubeImage(int length, int width, Qt::Orientation orientation)
{
	QPointF stop = orientation == Qt::Vertical ? QPointF(width / 2., 0) : QPointF(0, width / 2.);
	QLinearGradient gradient(QPointF(0, 0), stop);
	gradient.setSpread(QGradient::ReflectSpread);
	gradient.setColorAt(0, Colors::tube2);
	gradient.setColorAt(1, Colors::tube);

	QSize size = orientation == Qt::Horizontal ? QSize(length, width) : QSize(width, length);
	QImage tube(size, QImage::Format_ARGB32);
	QPainter painter(&tube);
	painter.fillRect(tube.rect(), gradient);
	painter.setPen(Colors::tubeBorder);
	if(orientation == Qt::Vertical) {
		painter.drawLine(tube.rect().topLeft(), tube.rect().bottomLeft());
		painter.drawLine(tube.rect().topRight(), tube.rect().bottomRight());
	} else {
		painter.drawLine(tube.rect().topLeft(), tube.rect().topRight());
		painter.drawLine(tube.rect().bottomLeft(), tube.rect().bottomRight());
	}
	return tube;
}

QImage * makeSprite(int sprite, const QSize & cellSize, bool on)
{
	static QImage tubeHor = getTubeImage(cellSize.width() / 2, cellSize.height() / 4, Qt::Horizontal);
	static QImage tubeVer = getTubeImage(cellSize.height() / 2, cellSize.width() / 4, Qt::Vertical);
	int cellRadius = qMin(cellSize.width(), cellSize.height());
	static QImage lightOff = getLightOffImage(cellRadius, lampDiameter(cellRadius));
	static QImage lightOn  = getLightOnImage (cellRadius, lampDiameter(cellRadius));

	QImage * image = new QImage(cellSize, QImage::Format_RGB32);
	image->fill(Colors::back.rgba());

	QPainter painter(image);
	painter.drawRect(image->rect().adjusted(0, 0, -1, -1));

	if(Branch::hasUpEnd(sprite)) {
		painter.drawImage((image->width() - tubeVer.width()) / 2, 0,                tubeVer); 
	}
	if(Branch::hasDownEnd(sprite)) {
		painter.drawImage((image->width() - tubeVer.width()) / 2, tubeVer.height(), tubeVer); 
	}
	if(Branch::hasLeftEnd(sprite)) {
		painter.drawImage(0,               (image->height() - tubeHor.height()) / 2, tubeHor); 
	}
	if(Branch::hasRightEnd(sprite)) {
		painter.drawImage(tubeHor.width(), (image->height() - tubeHor.height()) / 2, tubeHor); 
	}

	if(sprite != Branch::NONE) {
		painter.drawImage(0, 0, on ? lightOn : lightOff);
	}
	return image;
}

};
