#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QWidget>

class Graph;

class ChartWindow : public QWidget
{
	Q_OBJECT
public:
	static qreal func1(qreal x);
	static qreal func2(qreal x);
	static qreal func3(qreal x);
	static qreal func4(qreal x);

	static qreal difFunc1(qreal x);
	static qreal difFunc2(qreal x);
	static qreal difFunc3(qreal x);
	static qreal difFunc4(qreal x);

	static qreal func1Step;
	static qreal func2Step;
	static qreal func4Step;


	enum GridLabels { NoLabels = 0,
					  LeftLabels = 0x01,
					  RightLabels = 0x02,
					  TopLabels = 0x04,
					  BottomLabels = 0x08,
					  AllLabels = LeftLabels | RightLabels | TopLabels | BottomLabels
	};
	enum GraphType { Function, Series };
	enum AttachToCourner { NoAttach,
						   TopLeft,
						   TopRight,
						   BottomLeft,
						   BottomRight
	};
	enum ZoomAxis { AxisX = 0x01,
					AxisY = 0x02,
					BothAxis = AxisX | AxisY
	};

private:
	QPoint mousePos;
    QPoint mousePressPos;

    bool mousePressed;
	bool altPressed;

	// chart options
	bool _drawGrid;
	GridLabels labels;
	bool _isMovable;
	bool _isScalable;
	AttachToCourner courner;
	GraphType _type;

	qreal step = 0.01;
	qreal yScale = 0.01;

	qreal tempDeltaX = 1;
	qreal deltaX = 1;
	qreal tempDeltaY = 1;
	qreal deltaY = 1;

	QPoint tempOffset;
	QPoint permOffset;

	QList<Graph*> _charts;

	inline QPointF totalOffset() const;

	// coord mapping funcs
	// chart coord (y - up, x - right) --> screen coord (y - down, x - right)
	inline QPointF mapFromChart(const QPointF&) const;
	inline QPointF mapFromChart(qreal x, qreal y) const;
	inline qreal mapFromChartX(qreal) const;
	inline qreal mapFromChartY(qreal) const;
	// screen coord --> chart coord
	inline QPointF mapToChart(const QPoint&) const;
	inline QPointF mapToChart(int x, int y) const;
	inline qreal mapToChartX(int) const;
	inline qreal mapToChartY(int) const;

	// drawing funcs
	inline void drawGrid(QPainter&);
	inline void drawZeroAxis(QPainter&);
	inline void drawInfoWindow(QPainter&);
	inline void drawFunctionChart(QPainter&, Graph*);
	inline void drawSeriesChart(QPainter&, Graph*);
	inline void drawIntervals(QPainter&, Graph*);

public:
	ChartWindow(QWidget *parent = nullptr);

	// setters
	void setMode(GraphType);
	void setMovable(bool enable);
	void setScalable(bool enable);
	void setAttachment(int cor);
	void setGridLabels(int lab);

	// getters
	bool isMovable() const;
	bool isScalable() const;
	QList<Graph*> charts() const;
	GraphType type() const;

	// actions
	void scale(qreal value, ZoomAxis axis = BothAxis, bool centerOnPointer = false);
	void resetOffset();
	void resetScale();
	void fitSeries();
	void addChart(Graph*);

protected:
	void paintEvent(QPaintEvent*) override;
	void mouseMoveEvent(QMouseEvent*) override;
	void mousePressEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;
	void keyPressEvent(QKeyEvent*) override;
	void keyReleaseEvent(QKeyEvent *) override;
	void resizeEvent(QResizeEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
};

#endif // CHARTWINDOW_H
