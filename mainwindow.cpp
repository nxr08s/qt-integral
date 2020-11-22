#include "mainwindow.h"
//#include "ui_mainwindow.h"

#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QResizeEvent>

#include "math.h"

#include "chartwindow.h"
#include "graph.h"
#include "optionsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	chartWidget(new ChartWindow)
{
	
	QRect geom = geometry();
	geom.setSize(QSize(800, 600));
	setGeometry(geom);
	
	setWindowTitle("Chartie");
	setMinimumSize(QSize(320, 240));
	
	setCentralWidget(chartWidget);
	chartWidget->setFocus(Qt::OtherFocusReason);
	chartWidget->resetOffset();
	
	optButton = new QPushButton("Параметры", this);
	optButton->show();
	
	QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
	shadow->setBlurRadius(10);
	shadow->setXOffset(1);
	shadow->setYOffset(1);
	shadow->setColor(Qt::darkGray);
	optButton->setGraphicsEffect(shadow);
	
	Graph *graph = new Graph(ChartWindow::func1, ChartWindow::difFunc1);
	graph->setColor(Qt::darkMagenta);
	chartWidget->addChart(graph);

	connect(optButton, SIGNAL(clicked()), this, SLOT(optButtonClicked()));
}

void MainWindow::optButtonClicked(){
	optDialog = new OptionsDialog(chartWidget, this);
	optDialog->show();
	optButton->hide();
	
	connect(optDialog, SIGNAL(destroyed()), this, SLOT(optDialogDestroyed()));
}

void MainWindow::optDialogDestroyed(){
	optButton->show();
}

void MainWindow::resizeEvent(QResizeEvent *event){
	QRect geom = optButton->geometry();
	geom.moveTo(width() - geom.width() - 70, height() - geom.height() - 60);
	optButton->setGeometry(geom);
	
	QMainWindow::resizeEvent(event);
}
