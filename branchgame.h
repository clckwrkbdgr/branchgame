#pragma once

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QPoint>
#include <QtCore/QVector>
#include <QtCore/QMap>

namespace Branch {
	enum {
		NONE = 0,
		UP        = 1 << 3,
		RIGHT     = 1 << 2,
		DOWN      = 1 << 1,
		LEFT      = 1 << 0,
		UPDOWN    = UP | DOWN,
		UPRIGHT   = UP | RIGHT,
		UPLEFT    = UP | LEFT,
		LEFTRIGHT = LEFT | RIGHT,
		DOWNLEFT  = DOWN | LEFT,
		DOWNRIGHT = DOWN | RIGHT,
		T_DOWN    = LEFTRIGHT | DOWN,
		T_UP      = LEFTRIGHT | UP,
		T_LEFT    = UPDOWN | LEFT,
		T_RIGHT   = UPDOWN | RIGHT,
		CROSS     = UPDOWN | LEFTRIGHT,
	};

	enum { CELL_EMPTY, CELL_OFF, CELL_ON };

	bool hasUpEnd(int branch);
	bool hasDownEnd(int branch);
	bool hasLeftEnd(int branch);
	bool hasRightEnd(int branch);
};

typedef QPair<int, QPoint> NeighPair; // Parent branch direction, neighbour pos.
typedef QList<NeighPair> NeighList;

class BranchGame {
	Q_DISABLE_COPY(BranchGame);
public:
	BranchGame(const QSize & fieldSize);
	virtual ~BranchGame() {}

	const QSize & fieldSize() const { return size; }
	int width() const { return size.width(); }
	int height() const { return size.height(); }
	bool isFinished() const { return finished; }
	int cell(int x, int y) const;
	int cellState(int x, int y) const;

	void rotateBranch(const QPoint & pos);

private:
	QSize size;
	bool finished;
	QVector<int> field;
	QVector<int> flags;
	QPoint start;
	QMap<int, int> nextStates;

	bool isValid(int x, int y) const;
	bool isValid(const QPoint & p) const { return isValid(p.x(), p.y()); }

	int & rCell(const QPoint & pos) { return rCell(pos.x(), pos.y()); }
	const int & rCell(const QPoint & pos) const { return rCell(pos.x(), pos.y()); }
	int & rCell(int x, int y) { return field[x + y * size.width()]; }
	const int & rCell(int x, int y) const { return field[x + y * size.width()]; }

	bool isLinked(const QPoint & a, const QPoint & b) const;
	void calculateFinishedState();
	void generateField();
	bool isCoherent();
	NeighList getNeighbours(const QPoint & p) const;
	NeighList getLinkedNeighbours(const QPoint & p) const;
};
