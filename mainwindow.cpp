#include <QtDebug>
#include <QtGui/QApplication>
#include <QtCore/QTranslator>
#include <QtCore/QSettings>
#include <QtGui/QMessageBox>
#include <QtGui/QCloseEvent>

#include "ui_settings.h"
#include "branchwidget.h"
#include "mainwindow.h"

const QSize cellSize = QSize(64, 64);

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.actionNewGame, SIGNAL(triggered()), this, SLOT(newGame()));

	QSettings settings;
	fieldSize = settings.value("game/fieldsize", QSize(8, 8)).toSize();

	newGame();

	resize(settings.value("mainwindow/size", size()).toSize());
	move(settings.value("mainwindow/pos", pos()).toPoint());
	if(settings.value("mainwindow/maximized", false).toBool())
		setWindowState(Qt::WindowMaximized);

}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("mainwindow/maximized", windowState().testFlag(Qt::WindowMaximized));
	if(!windowState().testFlag(Qt::WindowMaximized))
	{
		settings.setValue("mainwindow/size", size());
		settings.setValue("mainwindow/pos", pos());
	}

	settings.setValue("game/fieldsize", fieldSize);
}

void MainWindow::on_actionSettings_triggered()
{
	QDialog dialog(this);
	Ui::Settings dialogUi;
	dialogUi.setupUi(&dialog);

	dialogUi.widthEdit->setValue(fieldSize.width());
	dialogUi.heightEdit->setValue(fieldSize.height());
	if(dialog.exec()) {
		fieldSize.setWidth(dialogUi.widthEdit->value());
		fieldSize.setHeight(dialogUi.heightEdit->value());
		newGame();
	}
}

void MainWindow::on_actionHelp_triggered()
{
	QMessageBox::information(this, tr("Help"), tr(
				"Goal of the game is to connect all branches alltogether.\n"
				"Circuit must be closed.\n"
				"Be careful: all the branches in the circuit must be connected.\n"
				"There must be no loose ends!\n"
				"\n"
				"Left click on a cell will turn it counter-clockwise.\n"
				"\n"
				"There is one cell which serves as power source.\n"
				"You'll see it."
				), QMessageBox::Ok);
}

void MainWindow::newGame()
{
	BranchWidget * w = new BranchWidget(fieldSize, cellSize);
	connect(w, SIGNAL(gameFinished()),   this, SLOT(gameFinished()));
	connect(w, SIGNAL(requestToClose()), this, SLOT(close()));
	ui.scrollArea->setWidget(w);
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	if(QMessageBox::question(this, tr("Exit"), tr("Do you really want to quit?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
		event->accept();
	} else {
		event->ignore();
	}
}

void MainWindow::gameFinished()
{
	statusBar()->showMessage(tr("Game is finished"));
}

int main(int argc, char ** argv) {
	qsrand(time(NULL));

	QApplication app(argc, argv);
	app.setApplicationName("branchgame");
	app.setOrganizationName("antifin");

	QTranslator translator;
	translator.load(QString("%1_%2").arg(app.applicationName()).arg(QLocale::system().name()));
	app.installTranslator(&translator);

	MainWindow wnd;
	wnd.show();
	return app.exec();
}
