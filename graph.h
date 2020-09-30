#ifndef GRAPH_H
#define GRAPH_H

#include "chartwindow.h"

#include <QColor>
#include <QList>
#include <QPair>

class Graph{

	friend class OptionsDialog;
public:
	enum IntegralMethod { Left, Middle, Right };

private:
	// general options
	ChartWindow::GraphType _type;
	QColor _color;

	// func options
	qreal (*_funcPtr)(qreal) = nullptr;
	qreal (*_diffFuncPtr)(qreal) = nullptr;

	// integral options
	int _intervalsQuantity;
	qreal _numericArea;
	qreal _intervalLength;
	qreal _analyticArea;
	QPair<qreal, qreal> _range;
	IntegralMethod _integralFuncMethod;

	// series options
	QList<QPointF> _seriesPoints;

public:
	Graph(qreal (*)(qreal), qreal (*)(qreal));

	// setters
	void setColor(QColor);
	void setRange(QPair<qreal, qreal>);
	void setIntervalsQuantity(int);
	void setData(QList<QPointF> data);
	void setType(ChartWindow::GraphType);
	void setIntegralMethod(IntegralMethod mt);

	//getters
	int intervalsQuantity() const;
	qreal numericArea() const;
	qreal analyticArea() const;
	QList<QPointF> data() const;
	IntegralMethod integralMethod() const;
	QPair<qreal, qreal> range() const;
	QColor lineColor() const;
	QColor areaColor() const;
	qreal func(qreal x) const;
	qreal integralFunc(qreal x) const;
	ChartWindow::GraphType type() const;
	qreal intervalLength() const;

	void calculateNumericArea();
	void calculateAnalyticArea();
};

#endif // GRAPH_H
