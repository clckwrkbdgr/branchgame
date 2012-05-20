#include <QtGui/QApplication>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>

#include "branchgame.h"
#include "mainwindow.h"

const QSize fieldSize(8, 8);
const QSize cellSize(64, 64);
const QColor backColor = Qt::black;
const QColor offColor  = Qt::blue;
const QColor onColor   = Qt::red;
const QColor foreColor = offColor;

MainWindow::MainWindow(QWidget * parent)
	: QWidget(parent), game(new BranchGame(fieldSize))
{
	setMouseTracking(true);
	makeSprites();
}

MainWindow::~MainWindow()
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

QImage * makeSprite(int sprite, bool on = false) {
	QImage * image = new QImage(cellSize, QImage::Format_RGB32);
	image->fill(backColor.rgb());

	QPoint up   (cellSize.width() / 2, 0);
	QPoint down (cellSize.width() / 2, cellSize.height());
	QPoint left (0,                    cellSize.height() / 2);
	QPoint right(cellSize.width(),     cellSize.height() / 2);
	QPoint center = image->rect().center();

	QPainter painter(image);
	painter.setPen(foreColor);
	painter.setBrush(backColor);
	painter.drawRect(image->rect().adjusted(0, 0, -1, -1));

	painter.setPen(on ? onColor : offColor);
	if(Branch::hasUpEnd(sprite)) {
		painter.drawLine(up, center);
	}
	if(Branch::hasDownEnd(sprite)) {
		painter.drawLine(down, center);
	}
	if(Branch::hasLeftEnd(sprite)) {
		painter.drawLine(left, center);
	}
	if(Branch::hasRightEnd(sprite)) {
		painter.drawLine(right, center);
	}
	return image;
}

void MainWindow::makeSprites() {
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

void MainWindow::keyPressEvent(QKeyEvent * event)
{
	switch(event->key()) {
		case Qt::Key_Escape: close();
		case Qt::Key_Q: close();
		default: QWidget::keyPressEvent(event);
	};
}

void MainWindow::mousePressEvent(QMouseEvent * event)
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

	update();
}

void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
	if(!game)
		return;

	QPoint cell;
	QRect fieldRect(0, 0, game->width() * cellSize.width(), game->height() * cellSize.height());
	fieldRect.moveCenter(rect().center());
	if(!fieldRect.contains(event->pos()))
		return;

	cell = event->pos() - fieldRect.topLeft();
	cell.rx() /= cellSize.width();
	cell.ry() /= cellSize.height();
	setWindowTitle(tr("%1, %2").arg(cell.x()).arg(cell.y()));
}

void MainWindow::paintEvent(QPaintEvent *)
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
	if(game->isFinished()) {
		painter.setPen(foreColor);
		painter.drawText(0, 100, tr("Game finished"));
	}
}

int main(int argc, char ** argv) {
	QApplication app(argc, argv);
	qsrand(time(NULL));

	MainWindow wnd;
	wnd.show();
	return app.exec();
}
