#pragma once
#include <QtGui/QWidget>
#include <QtCore/QMap>

class BranchGame;
class Image;

class BranchWidget : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(BranchWidget);
public:
	BranchWidget(const QSize & newFieldSize, const QSize & newCellSize, QWidget * parent = 0);
	virtual ~BranchWidget();

	virtual QSize minimumSizeHint() const;
	virtual QSize sizeHint() const { return minimumSizeHint(); }

signals:
	void gameFinished();
	void requestToClose();

protected:
	virtual void keyPressEvent(QKeyEvent*);
	virtual void mousePressEvent(QMouseEvent*);
	virtual void paintEvent(QPaintEvent*);
private:
	BranchGame * game;
	QMap<int, QImage*> spritesOff;
	QMap<int, QImage*> spritesOn;
	const QSize fieldSize;
	const QSize cellSize;

	void makeSprites();
};
