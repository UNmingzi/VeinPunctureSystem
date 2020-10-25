#ifndef UPDATEWIDGET_H
#define UPDATEWIDGET_H

#include <QThread>
#include "mainwindow.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>

#include <QChartView>
#include <QLineSeries>
#include <QPieSeries>
#include <QtCharts/QChartView>      //显示图表
#include <QtCharts/QLineSeries>     //线系列
#include <QtCharts/QtCharts>
#include <QMouseEvent>
#include <QMainWindow>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

QT_CHARTS_USE_NAMESPACE//QtChart名空间
using namespace QtCharts;

using namespace std;
using namespace cv;

extern QLineSeries *seriesSource;
extern double decPosArr[6];    //鼠标选点
class UpdatePicThread : public QThread
{
    Q_OBJECT
public:
    UpdatePicThread(Ui::MainWindow *ui);
    ~UpdatePicThread();
    void updateImage();                 //图像获取
    void readParameters();
    void rectify();
    void Cut();
    void matchSGBM();
    Ui::MainWindow *gui;
    Mat frame,frameLeft,frameRight,grayLeft,grayRight,rectLeft,rectRight,RGBLeft,RGBRight,imagePcl,disp8,dispRGB,imageCut;
    Mat disp;


public slots:
    //void slotUpate();

private:
    VideoCapture cap;
    QTimer theTimer;
    //Mat frame,frameLeft,frameRight,grayLeft,grayRight,rectLeft,rectRight,RGBLeft,RGBRight,imagePcl,disp,disp8,dispRGB,imageCut;
    int aimPositionX,aimPositionY;


protected:
    void run();
    void UpdatePicThread::mousePressEvent(QMouseEvent *event);       //选取点
    void paintEvent(QPaintEvent *e);

signals:
    //void updateWidgetSignal();
private slots:


};

#endif // UPDATEWIDGET_H
