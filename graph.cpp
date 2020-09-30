#include "graph.h"

Graph::Graph(qreal (*func)(qreal), qreal (*diffFunc)(qreal))
	: _type(ChartWindow::Function)
	, _color(Qt::black)
	, _funcPtr(func)
	, _diffFuncPtr(diffFunc)
	, _intervalsQuantity(50)
	, _range(QPair<qreal, qreal>(-1, 1))
	, _integralFuncMethod(Middle)
{
	calculateNumericArea();
	calculateAnalyticArea();
}

qreal Graph::func(qreal x) const{
	return _funcPtr(x);
}

qreal Graph::integralFunc(qreal x) const{
	switch(_integralFuncMethod){
	case Left:
		return func(x);
	case Middle:
		return (func(x) + func(x + _intervalLength)) / 2;
	case Right:
		return func(x + _intervalLength);
	}
	return 0;	// never executed, just to remove compiler warning
}

QColor Graph::lineColor() const{
	return _color;
}

void Graph::setColor(QColor col){
	_color = col;
}

ChartWindow::GraphType Graph::type() const{
	return _type;
}

QColor Graph::areaColor() const{
	return _color.lighter(175);
}

QPair<qreal, qreal> Graph::range() const{
	return _range;
}

void Graph::setRange(QPair<qreal, qreal> rng){
	_range = rng;
}

void Graph::setIntervalsQuantity(int value){
	_intervalsQuantity = value;
}

void Graph::calculateNumericArea(){
	_intervalLength = qreal(_range.second - _range.first) / _intervalsQuantity;
	_numericArea = 0;
	qreal x = _range.first;
	for (int i = 0; i < _intervalsQuantity; i++){
		_numericArea += qAbs(integralFunc(x)) *_intervalLength;
		x += _intervalLength;
	}
}

void Graph::calculateAnalyticArea()
{
	qreal step = qInf();
	qreal totalSum = 0;
	int sign = 1;

	if (_diffFuncPtr == ChartWindow::difFunc1)
		step = ChartWindow::func1Step;
	else if (_diffFuncPtr == ChartWindow::difFunc2)
		step = ChartWindow::func2Step;
	else if (_diffFuncPtr == ChartWindow::difFunc3)
		step = _range.second - _range.first;
	else if (_diffFuncPtr == ChartWindow::difFunc4)
		step = ChartWindow::func4Step;

	qreal left = _range.first;
	qreal right = int(left / step) * step;
	if (left == right)
		right += step;

	while (left < _range.second){
		sign = (func(left + 0.0000001) >= 0) ? (1) : (-1);
		totalSum += (_diffFuncPtr(right) - _diffFuncPtr(left)) * sign;
		left = right;
		right += step;
		if (right > _range.second)
			right = _range.second;
	}
	_analyticArea = totalSum;
}

void Graph::setIntegralMethod(Graph::IntegralMethod mt){
	_integralFuncMethod = mt;
}

qreal Graph::numericArea() const{
	return _numericArea;
}

qreal Graph::analyticArea() const{
	return _analyticArea;
}

qreal Graph::intervalLength() const{
	return _intervalLength;
}

int Graph::intervalsQuantity() const{
	return _intervalsQuantity;
}

void Graph::setData(QList<QPointF> data){
	_seriesPoints = data;
}

void Graph::setType(ChartWindow::GraphType type){
	_type = type;
}

QList<QPointF> Graph::data() const{
	return _seriesPoints;
}

Graph::IntegralMethod Graph::integralMethod() const{
	return _integralFuncMethod;
}
