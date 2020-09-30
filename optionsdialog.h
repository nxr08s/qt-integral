#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

#include "graph.h"

namespace Ui {
class OptionsDialog;
}

class ChartWindow;
class Graph;

class OptionsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit OptionsDialog(ChartWindow* chart, QWidget *parent = nullptr);
	~OptionsDialog();

private:
	Ui::OptionsDialog *ui;
	ChartWindow *chartWidget;
	Graph *chart;

	void updateRange(QPair<qreal, qreal>);
	void updateIntervalQuantity(int value);

	void syncAreaLabels();
	void syncMethodGroup(Graph::IntegralMethod);
	void syncModeGroup(ChartWindow::GraphType);
	void updateFunction(QString);
	void syncFuncComboBox();

	void updateSeriesChart();


	QList<QPointF> calculatePoints();

public slots:
	void leftRangeChanged(double d);
	void rightRangeChanged(double d);
	void intervalQuantityChanged(int newValue);
	void methodChanged(bool);
	void chartModeChanged(bool);
	void functionChanged(QString text);
};

#endif // OPTIONSDIALOG_H
