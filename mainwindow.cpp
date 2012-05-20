#include <QtGui/QApplication>

#include "branchwidget.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	BranchWidget * w = new BranchWidget();
	connect(w, SIGNAL(gameFinished()), this, SLOT(gameFinished()));
	setCentralWidget(w);
}

void MainWindow::gameFinished()
{
	statusBar()->showMessage(tr("Game is finished"));
}

int main(int argc, char ** argv) {
	QApplication app(argc, argv);
	qsrand(time(NULL));

	MainWindow wnd;
	wnd.show();
	return app.exec();
}
