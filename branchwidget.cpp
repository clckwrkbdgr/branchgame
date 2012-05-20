#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>

#include "branchwidget.h"
#include "branchgame.h"

const QSize fieldSize(4, 4);
const QSize cellSize(64, 64);
const QColor backColor = Qt::black;
const QColor offColor  = Qt::blue;
const QColor onColor   = Qt::red;
const QColor foreColor = offColor;

// TODO constants and magic numbers
// TODO PG graphics - to another module

QImage getShiningImage(int imageSize, int bulbSize)
{
	QImage light(QSize(imageSize, imageSize), QImage::Format_ARGB32);
	light.fill(qRgba(0, 0, 0, 0));
	QPainter lightPainter(&light);

	QRadialGradient lightFlow(light.rect().center(), imageSize / 2, light.rect().center());
	lightFlow.setColorAt(0, QColor(255, 192, 0, 255));
	lightFlow.setColorAt(1, QColor(0, 0, 0, 128));

	lightPainter.setBrush(lightFlow);
	lightPainter.setPen(Qt::NoPen);
	lightPainter.drawEllipse(light.rect().adjusted(-2, -2, 2, 2));
	return light;
}

QImage getLightOnImage(int imageSize, int bulbSize)
{
	QImage light(QSize(imageSize, imageSize), QImage::Format_ARGB32);
	light.fill(qRgba(0, 0, 0, 0));
	QPainter lightPainter(&light);
	QRect lightbulbRect((imageSize - bulbSize) / 2, (imageSize - bulbSize) / 2, bulbSize, bulbSize);

	QRadialGradient lightOn(lightbulbRect.center(), bulbSize, lightbulbRect.center());
	lightOn.setColorAt(0, QColor(255, 192, 0));
	lightOn.setColorAt(1, QColor(128, 64, 0));

	lightPainter.setPen(Qt::white);
	lightPainter.setBrush(lightOn);
	lightPainter.drawEllipse(lightbulbRect);
	return light;
}

QImage getLightOffImage(int imageSize, int bulbSize)
{
	QImage light(QSize(imageSize, imageSize), QImage::Format_ARGB32);
	light.fill(qRgba(0, 0, 0, 0));
	QPainter lightPainter(&light);
	QRect lightbulbRect((imageSize - bulbSize) / 2, (imageSize - bulbSize) / 2, bulbSize, bulbSize);

	lightPainter.setPen(Qt::white);
	lightPainter.setBrush(Qt::black);
	lightPainter.drawEllipse(lightbulbRect);
	return light;
}

QImage getTubeImage(int length, int width, Qt::Orientation orientation)
{
	QPointF stop = orientation == Qt::Vertical ? QPointF(width / 2., 0) : QPointF(0, width / 2.);
	QLinearGradient gradient(QPointF(0, 0), stop);
	gradient.setSpread(QGradient::ReflectSpread);
	gradient.setColorAt(0, QColor(32, 32, 32));
	gradient.setColorAt(1, QColor(192, 192, 192));

	QSize size = orientation == Qt::Horizontal ? QSize(length, width) : QSize(width, length);
	QImage tube(size, QImage::Format_ARGB32);
	QPainter painter(&tube);
	painter.fillRect(tube.rect(), gradient);
	painter.setPen(Qt::gray);
	if(orientation == Qt::Vertical) {
		painter.drawLine(tube.rect().topLeft(), tube.rect().bottomLeft());
		painter.drawLine(tube.rect().topRight(), tube.rect().bottomRight());
	} else {
		painter.drawLine(tube.rect().topLeft(), tube.rect().topRight());
		painter.drawLine(tube.rect().bottomLeft(), tube.rect().bottomRight());
	}
	return tube;
}

QImage * makeSprite(int sprite, bool on = false)
{
	static QImage tubeHor = getTubeImage(cellSize.width() / 2, cellSize.height() / 4, Qt::Horizontal);
	static QImage tubeVer = getTubeImage(cellSize.height() / 2, cellSize.width() / 4, Qt::Vertical);
	int cellRadius = qMin(cellSize.width(), cellSize.height());
	static QImage lightOff = getLightOffImage(cellRadius, cellRadius * 3 / 8);
	static QImage lightOn  = getLightOnImage (cellRadius, cellRadius * 3 / 8);

	QImage * image = new QImage(cellSize, QImage::Format_RGB32);
	image->fill(backColor.rgb());

	QPoint up   (cellSize.width() / 2, 0);
	QPoint down (cellSize.width() / 2, cellSize.height());
	QPoint left (0,                    cellSize.height() / 2);
	QPoint right(cellSize.width(),     cellSize.height() / 2);
	QPoint center = image->rect().center();

	QPainter painter(image);
	painter.drawRect(image->rect().adjusted(0, 0, -1, -1));

	if(Branch::hasUpEnd(sprite)) {
		painter.drawImage((image->width() - tubeVer.width()) / 2, 0, tubeVer);
	}
	if(Branch::hasDownEnd(sprite)) {
		painter.drawImage((image->width() - tubeVer.width()) / 2, tubeVer.height(), tubeVer);
	}
	if(Branch::hasLeftEnd(sprite)) {
		painter.drawImage(0, (image->height() - tubeHor.height()) / 2, tubeHor);
	}
	if(Branch::hasRightEnd(sprite)) {
		painter.drawImage(tubeHor.width(), (image->height() - tubeHor.height()) / 2, tubeHor);
	}
	if(sprite != Branch::NONE) {
		painter.drawImage(0, 0, on ? lightOn : lightOff);
	}
	return image;
}

BranchWidget::BranchWidget(QWidget * parent)
	: QWidget(parent), game(new BranchGame(fieldSize))
{
	setMouseTracking(true);
	makeSprites();
	setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

BranchWidget::~BranchWidget()
{
	foreach(QImage * sprite, spritesOn) {
		delete sprite;
		sprite = NULL;
	}
	foreach(QImage * sprite, spritesOff) {
		delete sprite;
		sprite = NULL;
	}
	delete game;
}

QSize BranchWidget::minimumSizeHint() const
{
	if(game)
		return QSize(game->width() * cellSize.width(), game->height() * cellSize.height());
	return QSize();
}

void BranchWidget::makeSprites() {
	QList<int> branches;
	branches << Branch::NONE << Branch::UP << Branch::DOWN << Branch::LEFT << Branch::RIGHT <<
		Branch::UPDOWN << Branch::UPRIGHT << Branch::UPLEFT <<
		Branch::LEFTRIGHT << Branch::DOWNLEFT << Branch::DOWNRIGHT <<
		Branch::T_DOWN << Branch::T_UP << Branch::T_LEFT << Branch::T_RIGHT <<
		Branch::CROSS;

	foreach(int branch, branches) {
		spritesOn [branch] = makeSprite(branch, true);
		spritesOff[branch] = makeSprite(branch, false);
	}
}

void BranchWidget::keyPressEvent(QKeyEvent * event)
{
	switch(event->key()) {
		case Qt::Key_Escape: close();
		case Qt::Key_Q: close();
		default: QWidget::keyPressEvent(event);
	};
}

void BranchWidget::mousePressEvent(QMouseEvent * event)
{
	if(!game)
		return;
	if(game->isFinished())
		return;
	if(event->button() != Qt::LeftButton)
		return;

	QPoint cell;
	QRect fieldRect(0, 0, game->width() * cellSize.width(), game->height() * cellSize.height());
	fieldRect.moveCenter(rect().center());
	if(!fieldRect.contains(event->pos()))
		return;

	cell = event->pos() - fieldRect.topLeft();
	cell.rx() /= cellSize.width();
	cell.ry() /= cellSize.height();

	game->rotateBranch(cell);
	if(game->isFinished()) {
		emit gameFinished();
	}

	update();
}

void BranchWidget::paintEvent(QPaintEvent *)
{
	if(!game)
		return;
	
	QPainter painter(this);
	painter.fillRect(rect(), backColor);

	QRect fieldRect(0, 0, game->width() * cellSize.width(), game->height() * cellSize.height());
	fieldRect.moveCenter(rect().center());

	for(int x = 0; x < game->width(); ++x) {
		for(int y = 0; y < game->height(); ++y) {
			QPoint cell(x * cellSize.width(), y * cellSize.height());
			QImage * sprite = (game->cellState(x, y) == Branch::CELL_ON) ? spritesOn[game->cell(x, y)] : spritesOff[game->cell(x, y)];
			painter.drawImage(fieldRect.topLeft() + cell, *sprite);
		}
	}

	static int cellRadius = qMin(cellSize.width(), cellSize.height());
	static QImage shining = getShiningImage(cellRadius, cellRadius * 3 / 8);
	if(game->isFinished()) {
		for(int x = 0; x < game->width(); ++x) {
			for(int y = 0; y < game->height(); ++y) {
				QPoint cell(x * cellSize.width(), y * cellSize.height());
				if(game->cellState(x, y) == Branch::CELL_ON) {
					painter.drawImage(fieldRect.topLeft() + cell, shining);
				}
			}
		}
	}
}

