#include <QtDebug>

#include "branchgame.h"

namespace Branch {
	bool hasUpEnd(int branch) {
		return branch & UP;
	}
	bool hasDownEnd(int branch) {
		return branch & DOWN;
	}
	bool hasLeftEnd(int branch) {
		return branch & LEFT;
	}
	bool hasRightEnd(int branch) {
		return branch & RIGHT;
	}
};

NeighList neighShifts(int branch)
{
	NeighList result;
	if(Branch::hasUpEnd(branch)) {
		result << qMakePair(int(Branch::UP),    QPoint(0 ,-1));
	}
	if(Branch::hasDownEnd(branch)) {
		result << qMakePair(int(Branch::DOWN),  QPoint(0 , 1));
	}
	if(Branch::hasLeftEnd(branch)) {
		result << qMakePair(int(Branch::LEFT),  QPoint(-1, 0));
	}
	if(Branch::hasRightEnd(branch)) {
		result << qMakePair(int(Branch::RIGHT), QPoint(1 , 0));
	}
	return result;
}

QMap<int, int> makeNextStatesMap()
{
	QMap<int, int> map;
	map[Branch::UP       ] = Branch::LEFT;
	map[Branch::RIGHT    ] = Branch::UP;
	map[Branch::DOWN     ] = Branch::RIGHT;
	map[Branch::LEFT     ] = Branch::DOWN;
	map[Branch::UPDOWN   ] = Branch::LEFTRIGHT;
	map[Branch::UPRIGHT  ] = Branch::UPLEFT;
	map[Branch::UPLEFT   ] = Branch::DOWNLEFT;
	map[Branch::LEFTRIGHT] = Branch::UPDOWN;
	map[Branch::DOWNLEFT ] = Branch::DOWNRIGHT;
	map[Branch::DOWNRIGHT] = Branch::UPRIGHT;
	map[Branch::T_DOWN   ] = Branch::T_RIGHT;
	map[Branch::T_UP     ] = Branch::T_LEFT;
	map[Branch::T_LEFT   ] = Branch::T_DOWN;
	map[Branch::T_RIGHT  ] = Branch::T_UP;
	map[Branch::CROSS    ] = Branch::CROSS;
	return map;
}

BranchGame::BranchGame(const QSize & fieldSize)
	: size(fieldSize), finished(false), field(fieldSize.width() * fieldSize.height(), Branch::NONE),
	flags(field.size(), Branch::CELL_EMPTY)
{
	nextStates = makeNextStatesMap();
	generateField();

	while(start.isNull() || rCell(start) == Branch::NONE) {
		start = QPoint(
				1 + qrand() % (width() - 2),
				1 + qrand() % (height() - 2)
				);
	}

	for(int i = 0; i < field.size(); ++i) {
		if(field[i]) {
			int count = qrand() % 4;
			while(count--) {
				field[i] = nextStates[field[i]];
			}
		}
	}

	calculateFinishedState();
}

void BranchGame::generateField()
{
	for(int i = 0; i < field.size(); ++i) {
		field[i] = Branch::CROSS;
	}

	for(int x = 0; x < width(); ++x) {
		rCell(x, 0)            &= ~Branch::UP;
		rCell(x, height() - 1) &= ~Branch::DOWN;
	}
	for(int y = 0; y < height(); ++y) {
		rCell(0          , y) &= ~Branch::LEFT;
		rCell(width() - 1, y) &= ~Branch::RIGHT;
	}

	int deadLinkCount = (width() - 1) * height() + width() * (height() - 1);
	deadLinkCount = deadLinkCount * 3 / 4; // TODO magic number
	for(int i = 0; i < deadLinkCount; ++i) {
		if(qrand() % 2) {
			QPoint p(qrand() % width(), qrand() % height() - 1);
			rCell(p.x(), p.y()    ) &= ~Branch::DOWN;
			rCell(p.x(), p.y() + 1) &= ~Branch::UP;
			if(!isCoherent()) {
				rCell(p.x(), p.y()    ) |= Branch::DOWN;
				rCell(p.x(), p.y() + 1) |= Branch::UP;
			}
		} else {
			QPoint p(qrand() % width() - 1, qrand() % height());
			rCell(p.x(),     p.y()) &= ~Branch::RIGHT;
			rCell(p.x() + 1, p.y()) &= ~Branch::LEFT;
			if(!isCoherent()) {
				rCell(p.x(),     p.y()) |= Branch::RIGHT;
				rCell(p.x() + 1, p.y()) |= Branch::LEFT;
			}
		}
	}
}

bool BranchGame::isValid(int x, int y) const
{
	return (0 <= x && x < size.width()) &&
		(0 <= y && y < size.height());
}

int BranchGame::cell(int x, int y) const
{
	if(!isValid(x, y))
		return Branch::NONE;
	return rCell(x, y);
}

bool BranchGame::isLinked(const QPoint & a, const QPoint & b) const
{
	int cellA = rCell(a);
	int cellB = rCell(b);
	QPoint d = (a - b);
	if(d == QPoint(0, -1) && Branch::hasDownEnd(cellA) && Branch::hasUpEnd(cellB)) {
		return true;
	}
	if(d == QPoint(0, 1) && Branch::hasUpEnd(cellA) && Branch::hasDownEnd(cellB)) {
		return true;
	}
	if(d == QPoint(-1, 0) && Branch::hasRightEnd(cellA) && Branch::hasLeftEnd(cellB)) {
		return true;
	}
	if(d == QPoint(1, 0) && Branch::hasLeftEnd(cellA) && Branch::hasRightEnd(cellB)) {
		return true;
	}

	return false;
}

void BranchGame::rotateBranch(const QPoint & pos)
{
	if(finished)
		return;
	int x = pos.x(), y = pos.y();
	if(!isValid(x, y))
		return;
	if(rCell(x, y) == Branch::NONE)
		return;

	int nextState = nextStates[rCell(x, y)];
	rCell(x, y) = nextState;
	calculateFinishedState();
}

bool BranchGame::isCoherent()
{
	for(int i = 0; i < field.size(); ++i) {
		flags[i] = field[i] ? Branch::CELL_OFF : Branch::CELL_EMPTY;
	}

	flags[start.x() + width() * start.y()] = Branch::CELL_ON;
	NeighList neighbours = getLinkedNeighbours(start);

	while(neighbours.size()) {
		NeighList newNeighs;
		foreach(NeighPair pair, neighbours) {
			const QPoint & n = pair.second;
			if(flags[n.x() + width() * n.y()] == Branch::CELL_ON)
				continue;

			flags[n.x() + width() * n.y()] = Branch::CELL_ON;
			newNeighs.append(getLinkedNeighbours(n));
		}
		neighbours = newNeighs;
	}

	foreach(int flag, flags) {
		if(flag == Branch::CELL_OFF)
			return false;
	}

	for(int x = 0; x < width() - 1; ++x) {
		for(int y = 0; y < height(); ++y) {
			if(Branch::hasRightEnd(rCell(x, y)) != Branch::hasLeftEnd(rCell(x + 1, y))) {
				return false;
			}
		}
	}
	for(int y = 0; y < height() - 1; ++y) {
		for(int x = 0; x < width(); ++x) {
			if(Branch::hasDownEnd(rCell(x, y)) != Branch::hasUpEnd(rCell(x, y + 1))) {
				return false;
			}
		}
	}
	return true;
}

int BranchGame::cellState(int x, int y) const
{
	if(!isValid(x, y))
		return Branch::CELL_EMPTY;
	return flags[x + y * size.width()];
}

void BranchGame::calculateFinishedState()
{
	finished = isCoherent();
}

NeighList BranchGame::getNeighbours(const QPoint & p) const
{
	NeighList shifts = neighShifts(rCell(p));
	NeighList result;
	for(int i = 0; i < shifts.count(); ++i) {
		QPoint n = p + shifts[i].second;
		if(isValid(n)) {
			result << qMakePair(shifts[i].first, n);
		}
	}
	return result;
}

NeighList BranchGame::getLinkedNeighbours(const QPoint & p) const
{
	NeighList shifts = neighShifts(rCell(p));
	NeighList result;
	for(int i = 0; i < shifts.count(); ++i) {
		QPoint n = p + shifts[i].second;
		if(isValid(n) && isLinked(p, n)) {
			result << qMakePair(shifts[i].first, n);
		}
	}
	return result;
}

