#include "optionsdialog.h"
#include "ui_optionsdialog.h"

#include "chartwindow.h"
#include "graph.h"

OptionsDialog::OptionsDialog(ChartWindow* chartWgt, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::OptionsDialog)
	, chartWidget(chartWgt)
	, chart(chartWgt->charts().first())
{
	ui->setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint)
				   | Qt::MSWindowsFixedSizeDialogHint);

	syncMethodGroup(chart->integralMethod());
	syncModeGroup(chartWidget->type());

	ui->leftRange->setValue(chart->range().first);
	ui->rightRange->setValue(chart->range().second);

	ui->intervalQuantity->setValue(chart->intervalsQuantity());

	syncAreaLabels();
	syncFuncComboBox();
	if (chartWidget->type() == ChartWindow::Series){
		ui->intervalQuantity->setEnabled(false);
		ui->numericalValue->setEnabled(false);
	}

	connect(ui->leftRange, SIGNAL(valueChanged(double)),
			this, SLOT(leftRangeChanged(double)));
	connect(ui->rightRange, SIGNAL(valueChanged(double)),
			this, SLOT(rightRangeChanged(double)));

	connect(ui->intervalQuantity, SIGNAL(valueChanged(int)),
			this, SLOT(intervalQuantityChanged(int)));

	connect(ui->leftMethod, SIGNAL(toggled(bool)),
			this, SLOT(methodChanged(bool)));
	connect(ui->middleMethod, SIGNAL(toggled(bool)),
			this, SLOT(methodChanged(bool)));
	connect(ui->rightMethod, SIGNAL(toggled(bool)),
			this, SLOT(methodChanged(bool)));

	connect(ui->functionMode, SIGNAL(toggled(bool)),
			this, SLOT(chartModeChanged(bool)));

	connect(ui->seriesMode, SIGNAL(toggled(bool)),
			this, SLOT(chartModeChanged(bool)));

	connect(ui->funcComboBox, SIGNAL(currentTextChanged(QString)),
			this, SLOT(functionChanged(QString)));
}

OptionsDialog::~OptionsDialog(){
	delete ui;
}

void OptionsDialog::updateRange(QPair<qreal, qreal> rng){
	chart->setRange(rng);
	chart->calculateNumericArea();
	chart->calculateAnalyticArea();
}

void OptionsDialog::syncAreaLabels(){
	ui->numericalValue->setText((chartWidget->type() == ChartWindow::Function)?
									(QString::number(chart->numericArea())):
									(""));
	ui->calculatedValue->setText(QString::number(chart->analyticArea()));
}

void OptionsDialog::updateIntervalQuantity(int value){
	chart->setIntervalsQuantity(value);
	chart->calculateNumericArea();
}

void OptionsDialog::syncMethodGroup(Graph::IntegralMethod method){
	switch(method){
	case Graph::Left:
		ui->leftMethod->setChecked(true);
		break;
	case Graph::Middle:
		ui->middleMethod->setChecked(true);
		break;
	case Graph::Right:
		ui->rightMethod->setChecked(true);
		break;
	}
}

void OptionsDialog::syncModeGroup(ChartWindow::GraphType type){
	if (type == ChartWindow::Function)
		ui->functionMode->setChecked(true);
	else
		ui->seriesMode->setChecked(true);
}

void OptionsDialog::updateFunction(QString funcText){
	if (funcText == "sin(x)"){
		chart->_funcPtr = ChartWindow::func1;
		chart->_diffFuncPtr = ChartWindow::difFunc1;
	}
	else if (funcText == "sinh(x)"){
		chart->_funcPtr = ChartWindow::func2;
		chart->_diffFuncPtr = ChartWindow::difFunc2;
	}
	else if (funcText == "x * x"){
		chart->_funcPtr = ChartWindow::func3;
		chart->_diffFuncPtr = ChartWindow::difFunc3;
	}
	else if (funcText == "e^-|x| * cos(2*pi*x)"){
		chart->_funcPtr = ChartWindow::func4;
		chart->_diffFuncPtr = ChartWindow::difFunc4;
	}
}

void OptionsDialog::syncFuncComboBox()
{
	if (chart->_funcPtr == ChartWindow::func1){
		ui->funcComboBox->setCurrentIndex(0);
	} else if (chart->_funcPtr == ChartWindow::func2){
		ui->funcComboBox->setCurrentIndex(1);
	} else if (chart->_funcPtr == ChartWindow::func3){
		ui->funcComboBox->setCurrentIndex(2);
	} else if (chart->_funcPtr == ChartWindow::func4){
		ui->funcComboBox->setCurrentIndex(3);
	}
}

void OptionsDialog::updateSeriesChart(){
	chart->setData(calculatePoints());
	chartWidget->resetScale();
	chartWidget->fitSeries();
}

QList<QPointF> OptionsDialog::calculatePoints()
{
	QList<QPointF> points;
	int currentIntervalValue = chart->intervalsQuantity();
	for (int i = 1; i < 200; i += 2){
		chart->setIntervalsQuantity(i);
		chart->calculateNumericArea();
		qreal error = qAbs(chart->analyticArea() - chart->numericArea())
					  / chart->analyticArea() * 100;
		points << QPointF(i, error);
	}
	chart->setIntervalsQuantity(currentIntervalValue);
	chart->calculateNumericArea();
	return points;
}

void OptionsDialog::leftRangeChanged(double d){
	if (d < ui->rightRange->value()){
		updateRange(QPair<qreal,qreal>(d, chart->range().second));
		if (chartWidget->type() == ChartWindow::Series)
			updateSeriesChart();
		syncAreaLabels();
		chartWidget->update();
	}else
		ui->leftRange->setValue(ui->rightRange->value() - 0.5);
}

void OptionsDialog::rightRangeChanged(double d)
{
	if (d > ui->leftRange->value()){
		updateRange(QPair<qreal,qreal>(chart->range().first, d));
		if (chartWidget->type() == ChartWindow::Series)
			updateSeriesChart();
		syncAreaLabels();
		chartWidget->update();
	}else
		ui->rightRange->setValue(ui->leftRange->value() + 0.5);
}

void OptionsDialog::intervalQuantityChanged(int newValue){
	updateIntervalQuantity(newValue);
	chartWidget->update();
	syncAreaLabels();
}

void OptionsDialog::methodChanged(bool toggled){
	if (!toggled)
		return;

	QRadioButton* button = static_cast<QRadioButton*>(sender());
	if (button == ui->leftMethod)
		chart->setIntegralMethod(Graph::Left);
	else if (button == ui->middleMethod)
		chart->setIntegralMethod(Graph::Middle);
	else if (button == ui->rightMethod)
		chart->setIntegralMethod(Graph::Right);

	chart->calculateNumericArea();
	syncAreaLabels();

	if (chartWidget->type() == ChartWindow::Series)
		updateSeriesChart();

	chartWidget->update();
}

void OptionsDialog::chartModeChanged(bool toggled){
	if (!toggled)
		return;

	QRadioButton *button = static_cast<QRadioButton*>(sender());
	if (button == ui->seriesMode){
		chart->setData(calculatePoints());
		chart->setType(ChartWindow::Series);
		chartWidget->setMode(ChartWindow::Series);
		ui->intervalQuantity->setEnabled(false);
		ui->numericalValue->setEnabled(false);
	} else {
		chart->setType(ChartWindow::Function);
		chartWidget->setMode(ChartWindow::Function);
		ui->intervalQuantity->setEnabled(true);
		ui->numericalValue->setEnabled(true);
	}
	syncAreaLabels();
}

void OptionsDialog::functionChanged(QString text)
{
	updateFunction(text);
	chart->calculateNumericArea();
	chart->calculateAnalyticArea();
	syncAreaLabels();
	if (chartWidget->type() == ChartWindow::Series)
		updateSeriesChart();
	chartWidget->update();
}
