#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QQuickWindow()
{

	// transparent background
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background:transparent;");

	// no window decorations
    setWindowFlags(Qt::FramelessWindowHint);

    // set QML file
	setSource(QUrl("mainwidget.qml"));
}

MainWindow::~MainWindow()
{
}
