#include "chartwindow.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>

#include <QDebug>

#include <cmath>

#include "graph.h"

qreal ChartWindow::func1Step = M_PI;
qreal ChartWindow::func2Step = M_PI;
qreal ChartWindow::func4Step = .25;

void ChartWindow::drawGrid(QPainter& painter){
	painter.save();
	// draw axis y values
	qreal row = mapToChartY(0);
	qreal nrow = int(row / deltaY) * deltaY;
	int yPos;
	while ((yPos = mapFromChartY(nrow)) < height()){
		painter.setPen(Qt::lightGray);
		painter.drawLine(0, yPos, width(), yPos);
		painter.setPen(Qt::black);

		// ----- smooth disaapearance of extreme labels -----
		if (yPos < 25)
			painter.setOpacity(qreal(yPos) / 25);
		else if (yPos > height() - 25)
			painter.setOpacity(qreal(height() - yPos) / 25);

		//		if (yPos < height() - 25 && yPos > 25){
		if (labels & LeftLabels){
			painter.rotate(-90);
			painter.drawText(QPoint(-yPos - 20, 15), QString::number(nrow,'f',4));
			painter.rotate(90);
		}
		if (labels & RightLabels){
			painter.rotate(90);
			painter.drawText(QPoint(yPos - 17, -width() + 20), QString::number(nrow, 'f', 4));
			painter.rotate(-90);
		}
		//		}
		nrow -= deltaY;
		painter.setOpacity(1.);
	}

	// draw axis x values
	qreal line = mapToChart(0,0).x();
	qreal nearest = int(line / deltaX) * deltaX;
	int xPos;
	while ((xPos = mapFromChartX(nearest)) < width()){
		painter.setPen(Qt::lightGray);
		painter.drawLine(xPos, 0, xPos, height());
		painter.setPen(Qt::black);
		// ----- smooth disaapearance of extreme labels -----
		if (xPos < 40)
			painter.setOpacity(qreal(xPos) / 40);
		else if (xPos > width() - 40)
			painter.setOpacity(qreal(width() - xPos) / 40);
		//		if (xPos > 40 && xPos < width() - 20){
		if (labels & BottomLabels)
			painter.drawText(QPoint(xPos - 20, height() - 5), QString::number(nearest, 'f', 4));
		if (labels & TopLabels)
			painter.drawText(QPoint(xPos - 20, 15), QString::number(nearest, 'f', 4));
		//		}
		nearest += deltaX;
		painter.setOpacity(1.);
	}
	painter.restore();
}

void ChartWindow::drawZeroAxis(QPainter &painter){
	painter.save();
	painter.setPen(QPen(Qt::black, 1.5));
	QPointF zeroPoint = mapFromChart(0,0);
	painter.drawLine(zeroPoint.x(), 0, zeroPoint.x(), height()); // y - axis
	painter.drawLine(0, zeroPoint.y(), width(), zeroPoint.y());	 // x - axis
	painter.restore();
}

void ChartWindow::drawInfoWindow(QPainter &painter){
	painter.save();

	// vertical line
	painter.setPen(QPen(Qt::black, 1, Qt::DashLine));
	painter.drawLine(mousePos.x(), 0, mousePos.x(), height());

	painter.setRenderHint(QPainter::Antialiasing);
	for (auto chart : _charts){

		// info box
		qreal calcX = mapToChartX(mousePos.x());
		qreal result = 0;
		if (chart->type() == Series){
			auto data = chart->data();

			// skip all that if x not on chart
			if (calcX < data.first().x() || calcX > data.last().x())
				continue;

			// "interpolate" line to find y
			for (int i = 1; i < data.size(); i++)
				if (data.at(i).x() > calcX){
					result = (calcX - data.at(i - 1).x()) / (data.at(i).x() - data.at(i - 1).x());
					result = data.at(i - 1).y() + (data.at(i).y() - data.at(i - 1).y()) * result;
					break;
				}
		} else
			result = chart->func(calcX);

		painter.setPen(QPen(chart->lineColor(), 2));
		QPainterPath path;
		path.addRoundedRect(mousePos.x(), mapFromChartY(result), 65, 30, 5, 5);
		painter.fillPath(path, QBrush(Qt::white));
		painter.drawPath(path);

		// info box labels
		painter.setPen(Qt::black);
		painter.drawText(QPoint(mousePos.x() + 5, mapFromChartY(result) + 13),
						 "x: " + QString::number(calcX, 'f', 4));
		painter.drawText(QPoint(mousePos.x() + 5, mapFromChartY(result) + 25),
						 "y: " + QString::number(result, 'f', 4));

		// point on chart
		QPainterPath elPath;
		elPath.addEllipse(QPoint(mousePos.x(), mapFromChartY(result)), 5, 5);
		painter.fillPath(elPath, chart->lineColor());
	}
	painter.restore();
}

void ChartWindow::drawFunctionChart(QPainter &painter, Graph *chart){
	qreal xPrev = mapToChartX(0);
	qreal yPrev = chart->func(xPrev);
	qreal x = xPrev + step;
	qreal y = chart->func(x);
	qreal xCoord = 1;
	qreal left = chart->range().first;
	qreal right = chart->range().second;
	bool inRange = false;

	painter.save();

	painter.setPen(Qt::black);
	while (xCoord < width()){
		if (x >= right && inRange){
			painter.setPen(QPen(Qt::black, 1));
			inRange = false;
		}else{
			if (x >= left && x <= right && !inRange){
				painter.setPen(QPen(chart->lineColor(), 2));
				inRange = true;
			}
		}
		painter.drawLine(xCoord - 1, mapFromChartY(yPrev),
						 xCoord,	 mapFromChartY(y));
		xCoord++;
		xPrev = x;
		x += step;
		yPrev = y;
		y = chart->func(x);
	}
	painter.restore();
}

void ChartWindow::drawSeriesChart(QPainter &painter, Graph *chart){
	painter.save();

	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(QPen(QBrush(chart->lineColor()),
						3,
						Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	QList<QPointF> points = chart->data();
	QPainterPath path;
	path.moveTo(mapFromChart(points.at(0)));
	for (int i = 1; i < points.size(); i++){
		path.lineTo(mapFromChart(points.at(i)));
	}
	painter.drawPath(path);

	painter.restore();
}

void ChartWindow::drawIntervals(QPainter &painter, Graph *chart){
	qreal left = chart->range().first;
	qreal length = chart->intervalLength() * (qreal(1) / step);
	qreal x = left;
	qreal xCoord = mapFromChartX(left);

	painter.save();

	painter.setPen(QPen(Qt::black, 1));
	QPainterPath path;
	qreal fRes;
	int intervals = chart->intervalsQuantity();
	for (int i = 0; i < intervals; i++){
		fRes = chart->integralFunc(x);
		path.addRect(xCoord, mapFromChartY(fRes), length, fRes / yScale);

		xCoord += length;
		x += chart->intervalLength();
	}
	painter.fillPath(path, QBrush(chart->areaColor(), Qt::Dense4Pattern));
	painter.drawPath(path);

	painter.restore();
}

qreal ChartWindow::func1(qreal x){
	return sin(x);
}

qreal ChartWindow::func2(qreal x){
	return sinh(x);
}

qreal ChartWindow::func3(qreal x){
	return x*x;
}

qreal ChartWindow::func4(qreal x){
	if (x == 0)
		return 1;
	return exp(-qAbs(x)) * cos(2 * M_PI * x);
}

qreal ChartWindow::difFunc1(qreal x){
	return -cos(x);
}

qreal ChartWindow::difFunc2(qreal x){
	return cosh(x);
}

qreal ChartWindow::difFunc3(qreal x){
	return x*x*x / 3;
}

qreal ChartWindow::difFunc4(qreal x){
	if (x == 0)
		return difFunc4(0.000000000001);
	return (x*exp(-abs(x))*(2*M_PI*sin(2*M_PI*qAbs(x))-cos(2*M_PI*qAbs(x))))
			/((4*M_PI*M_PI+1)*qAbs(x));
}

QPointF ChartWindow::totalOffset() const{
	return tempOffset + permOffset;
}

QPointF ChartWindow::mapFromChart(const QPointF &chartPoint) const{
	QPointF offset = totalOffset();
	return QPoint(chartPoint.x() / step + offset.x(),
				  offset.y() - chartPoint.y() / yScale);
}

QPointF ChartWindow::mapFromChart(qreal x, qreal y) const{
	QPointF offset = totalOffset();
	return QPoint(x / step + offset.x(),
				  offset.y() - y / yScale);
}

qreal ChartWindow::mapFromChartX(qreal x) const{
	return x / step + totalOffset().x();
}

qreal ChartWindow::mapFromChartY(qreal y) const{
	return totalOffset().y() - y / yScale;
}

QPointF ChartWindow::mapToChart(const QPoint &windowPoint) const{
	QPointF offset = totalOffset();
	return QPointF((windowPoint.x() - offset.x()) * step,
				   -(windowPoint.y() - offset.y()) * yScale);
}

QPointF ChartWindow::mapToChart(int x, int y) const{
	QPointF offset = totalOffset();
	return QPointF((x - offset.x()) * step,
				   -(y - offset.y()) * yScale);
}

qreal ChartWindow::mapToChartX(int x) const{
	return (x - totalOffset().x()) * step;
}

qreal ChartWindow::mapToChartY(int y) const{
	return -(y - totalOffset().y()) * yScale;
}

ChartWindow::ChartWindow(QWidget *parent)
	: QWidget(parent)
	, mousePos(QPoint(0, 0))
	, mousePressPos(QPoint(0, 0))
	, mousePressed(false)
	, altPressed(false)
	, _drawGrid(true)
	, labels(AllLabels)
	, _isMovable(true)
	, _isScalable(true)
	, courner(NoAttach)
	, _type(Function)
	, tempOffset(QPoint(0, 0))
	, permOffset(QPoint(0, 0))
{
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
}

void ChartWindow::scale(qreal value, ZoomAxis axis, bool centerOnPointer){

	QPointF oldChartPos = mapToChart(mousePos);

	if (axis & AxisX){
		step /= value;
		tempDeltaX /= value;

		if (tempDeltaX > 2 * deltaX)
			deltaX *= 2;
		if (deltaX > 2 * tempDeltaX)
			deltaX /= 2;

		if (_type == Series)
			deltaX /= value;
	}

	if (axis & AxisY){
		yScale /= value;
		tempDeltaY /= value;

		if (tempDeltaY > 2 * deltaY)
			deltaY *= 2;
		if (deltaY > 2 * tempDeltaY)
			deltaY /= 2;

		if (_type == Series)
			deltaY /= value;
	}

	if (centerOnPointer){
		QPoint posOnChart = mapFromChart(oldChartPos).toPoint();
		permOffset -= posOnChart - mousePos;
	}

	update();
}

void ChartWindow::resetOffset(){
	if (isMovable())
		permOffset = QPoint(width() / 2, height() / 2);
}

void ChartWindow::setAttachment(int cor){
	courner = static_cast<AttachToCourner>(cor);
}

void ChartWindow::setGridLabels(int lab){
	labels = static_cast<GridLabels>(lab);
}

void ChartWindow::fitSeries(){
	qreal maxX = 0;
	qreal maxY = 0;
	for (auto chart : _charts){
		if (chart->type() != Series)
			continue;
		QList<QPointF> data = chart->data();

		qreal maxLocalX = (*std::max_element(data.begin(), data.end(), [](QPointF& a, QPointF &b){
							   return a.x() < b.x();})).x();
		maxX = qMax(maxX, maxLocalX);

		qreal maxLocalY = (*std::max_element(data.begin(), data.end(), [](QPointF& a, QPointF &b){
							   return a.y() < b.y();})).y();
		maxY = qMax(maxY, maxLocalY);
	}
	scale(mapToChartX(width()) / maxX / 1.05, AxisX);
	scale(mapToChartY(0) / maxY / 1.05, AxisY);

}

void ChartWindow::setMode(GraphType type){
	_type = type;
	if (type == Series){
		setAttachment(BottomLeft);
		setGridLabels(LeftLabels | BottomLabels);
		setMovable(false);
		setScalable(false);
		permOffset = QPoint(40, height() - 40);
		fitSeries();
	}else if (type == Function){
		setAttachment(NoAttach);
		setGridLabels(AllLabels);
		setMovable(true);
		setScalable(true);
		resetScale();
		resetOffset();
	}
	update();
}

void ChartWindow::setMovable(bool enable){
	_isMovable = enable;
}

void ChartWindow::setScalable(bool enable){
	_isScalable = enable;
}

bool ChartWindow::isMovable() const{
	return _isMovable;
}

bool ChartWindow::isScalable() const{
	return _isScalable;
}

ChartWindow::GraphType ChartWindow::type() const{
	return _type;
}

QList<Graph *> ChartWindow::charts() const{
	return _charts;
}

void ChartWindow::resetScale(){
	step = .01;
	yScale = .01;
	tempDeltaX = 1;
	deltaX = 1;
	tempDeltaY = 1;
	deltaY = 1;
}

void ChartWindow::addChart(Graph *chart){
	_charts << chart;
}

void ChartWindow::paintEvent(QPaintEvent *){
	QPainter painter(this);
	painter.fillRect(rect(), QBrush(Qt::white));

	if (_drawGrid)
		drawGrid(painter);

	drawZeroAxis(painter);

	for (auto chart : _charts){
		if (chart->type() == Function){
			drawFunctionChart(painter, chart);
			drawIntervals(painter, chart);
		}
		else if (chart->type() == Series)
			drawSeriesChart(painter, chart);
	}

	if (altPressed)
		drawInfoWindow(painter);
}

void ChartWindow::mouseMoveEvent(QMouseEvent *event){
	mousePos = event->pos();
	bool shouldUpdate = false;
	if (mousePressed && _isMovable){
		tempOffset = event->pos() - mousePressPos;
		shouldUpdate = true;
	}
	if (altPressed)
		shouldUpdate = true;

	if (shouldUpdate)
		update();
}

void ChartWindow::mousePressEvent(QMouseEvent *event){
	if (_isMovable){
		setCursor(QCursor(Qt::ClosedHandCursor));
		mousePressed = true;
		mousePressPos = event->pos();
	}
}

void ChartWindow::mouseReleaseEvent(QMouseEvent *){
	setCursor(QCursor(Qt::ArrowCursor));
	mousePressed = false;
	permOffset += tempOffset;
	tempOffset = QPoint();
}

void ChartWindow::keyPressEvent(QKeyEvent *event){
	switch(event->key()){
	case Qt::Key_Space:
		resetOffset();
		update();
		break;
	case Qt::Key_Plus:
		if (_isScalable)
			scale(1.2, BothAxis, true);
		break;
	case Qt::Key_Minus:
		if (_isScalable)
			scale(1 / 1.2, BothAxis, true);
		break;
	case Qt::Key_Alt:
		if (!event->isAutoRepeat()){
			altPressed = true;
			setCursor(QCursor(Qt::PointingHandCursor));
			update();
		}
		break;
	default:
		QWidget::keyPressEvent(event);
	}
}

void ChartWindow::keyReleaseEvent(QKeyEvent *event){
	switch(event->key()){
	case Qt::Key_Alt:
		if (!event->isAutoRepeat()){
			altPressed = false;
			setCursor(QCursor(Qt::ArrowCursor));
			update();
		}
		break;
	default:
		QWidget::keyReleaseEvent(event);
	}
}

void ChartWindow::resizeEvent(QResizeEvent *event){
	QSize offsetSize = event->size() - event->oldSize();
	switch(courner){
	case TopRight:
		permOffset += QPoint(offsetSize.width(), 0);
		break;
	case BottomLeft:
		permOffset += QPoint(0, offsetSize.height());
		break;
	case BottomRight:
		permOffset += QPoint(offsetSize.width(), offsetSize.height());
		break;
	case TopLeft:
	case NoAttach:
	default:
		break;
	}
	QWidget::resizeEvent(event);
}

void ChartWindow::wheelEvent(QWheelEvent *event){
	if (type() != Series){
		qreal scaleFactor = (event->angleDelta().y() > 0) ? (1.05) : (1/1.05);
		scale(scaleFactor, BothAxis, true);
	}
}
