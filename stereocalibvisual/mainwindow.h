#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../utils/stereocapture.h"
#include "stereocalibrator.h"
#include "../utils/persistence.h"
#include "../utils/stereoprocess.h"
#include <QTimer>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow(); 

private:  
    Ui::MainWindow *m_ui;
    StereoCapture *m_stereoCapture;
    StereoCalibrator *m_stereoCalibrator;
    StereoProcess *m_stereoProcess;

    QTimer m_timer;
    double m_timeToPicture;

public slots:
    void sl_timeout();
    void sl_captureMessage(QString message);
    void sl_displayImages(int type);
    void sl_changeTimeToPicture(int time);

private slots:
    void on_buttonCamera_clicked();

    void on_buttonCalibration_clicked();

    void on_buttonSave_clicked();

    void on_buttonLoad_clicked();

    void setCalibration();

    void endedCalibration();

signals:
    void sg_message(QString error);
};

#endif // MAINWINDOW_H
