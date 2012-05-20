#pragma once
#include <QtGui/QWidget>
#include <QtCore/QMap>

class BranchGame;
class Image;

class MainWindow : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(MainWindow);
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow();

protected:
	virtual void keyPressEvent(QKeyEvent*);
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void paintEvent(QPaintEvent*);
private:
	BranchGame * game;
	QMap<int, QImage*> spritesOff;
	QMap<int, QImage*> spritesOn;

	void makeSprites();
};
