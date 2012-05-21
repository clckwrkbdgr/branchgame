#include <QtDebug>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>

#include "branchwidget.h"
#include "spritegen.h"
#include "branchgame.h"

BranchWidget::BranchWidget(const QSize & newFieldSize, const QSize & newCellSize, QWidget * parent)
	: QWidget(parent), game(new BranchGame(newFieldSize)),
	fieldSize(newFieldSize), cellSize(newCellSize)
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
		spritesOn [branch] = Sprites::makeSprite(branch, cellSize, true);
		spritesOff[branch] = Sprites::makeSprite(branch, cellSize, false);
	}
}

void BranchWidget::keyPressEvent(QKeyEvent * event)
{
	switch(event->key()) {
		case Qt::Key_Escape: case Qt::Key_Q: emit requestToClose();
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
	painter.fillRect(rect(), Sprites::backgroundColor());

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
		static int cellRadius = qMin(cellSize.width(), cellSize.height());
		static QImage shining = Sprites::getShiningImage(cellRadius);

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

