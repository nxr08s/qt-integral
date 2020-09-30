#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QPushButton;
class OptionsDialog;
class ChartWindow;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	QPushButton *optButton;
	OptionsDialog *optDialog;
	ChartWindow *chartWidget;

public:
	explicit MainWindow(QWidget *parent = nullptr);

public slots:
	void optButtonClicked();
	void optDialogDestroyed();

protected:
	void resizeEvent(QResizeEvent*) override;
};

#endif // MAINWINDOW_H
