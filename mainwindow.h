#pragma once

#include <QtGui/QMainWindow>

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow {
	Q_OBJECT
	Q_DISABLE_COPY(MainWindow);
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow();

protected:
	virtual void closeEvent(QCloseEvent *);
private slots:
	void gameFinished();

	void on_actionSettings_triggered();
	void on_actionHelp_triggered();
	void newGame();
private:
	Ui::MainWindow ui;
	QSize fieldSize;
};
