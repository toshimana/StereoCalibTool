#ifndef STEREOCALIBTOOLGUI_H
#define STEREOCALIBTOOLGUI_H

#include <QtWidgets/QMainWindow>
#include <memory>

class StereoCalibToolGUI : public QMainWindow
{
	Q_OBJECT

public:
	StereoCalibToolGUI(QWidget *parent = 0);
	~StereoCalibToolGUI();

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;

private slots:
    void getMessage( void )const;
};

#endif // STEREOCALIBTOOLGUI_H
