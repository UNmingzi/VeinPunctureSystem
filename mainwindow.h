#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMutex>
#include <QtWidgets/QLabel>

#include "flatui.h"
#include "triopc.h"
#include "safethread.h"
#include "tozerothread.h"
#include "recordthread.h"
#include "myaxesdriver.h"
#include "myalgorithm.h"

#include <QChartView>
#include <QLineSeries>
#include <QPieSeries>
#include <QtCharts/QChartView>      //显示图表
#include <QtCharts/QLineSeries>     //线系列
#include <QtCharts/QtCharts>

#include <QSerialPort>
#include <QSerialPortInfo>


#include<QTime>
#include <QCoreApplication>

QT_CHARTS_USE_NAMESPACE//QtChart名空间
using namespace QtCharts;

extern bool widget_flag;   //子窗口数据更新标志
extern QByteArray analogReadNum;
extern QString labelpro;


namespace Ui {
class MainWindow;                                   //主界面更新
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    //TrioPC *pcMat;                                      //驱动器对象指针
    //SafeThread *threadSafe;
    //ToZeroThread *threadZero;                                  //线程对象指针
    //MyAxesDriver *axesControl;                                 //轴控制指针

    Painter_widget *axis1_light;
    Painter_widget *axis2_light;
    Painter_widget *axis3_light;
    Painter_widget *axis4_light;
    Painter_widget *axis5_light;
    Painter_widget *axis6_light;

    /*界面更新函数*/
    void display_axes_status();
    void nowPositionDisplay();
    void chartUpdate(double num);

    /*串口刷新*/
    QStringList getPortNameList();//获取所有可用的串口列表
    void openPort();//打开串口





    void Motormove(int destination);
    int Motorreadpos();
    int Motorreadforce();
    static void DelayTime(int msc)
    {
        QTime t=QTime::currentTime().addMSecs(msc);
        while(QTime::currentTime()<t)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        }
    }








public slots:
    void receiveInfo(); //串口接收

private slots:              //信号槽？
    void updateWatch();
    void scrollCaption();
    double* nowPosition();

    void on_buttonHoming_clicked();
    void on_buttonRecordStart_clicked();
    void on_buttonRecordStop_clicked();
    void on_closeButton_clicked();
    void on_buttonConnect_clicked();
    void on_paraAdjustButton_clicked();
    void on_radioButtonEnable_clicked();


     /*微调函数*/
    void on_xPosAdd_clicked();
    void on_xPosMinus_clicked();
    void on_yPosAdd_clicked();
    void on_yPosMinus_clicked();
    void on_zPosAdd_clicked();
    void on_zPosMinus_clicked();
    void on_yawPosAdd_clicked();
    void on_yawPosMinus_clicked();
    void on_pitchPosAdd_clicked();
    void on_pitchPosMinus_clicked();
    void on_disPosAdd_clicked();
    void on_disPosMinus_clicked();

    double* getTargetPos();

    void on_pushButton_clicked();
    void on_punctureButton_clicked();
    void on_run_clicked();
    void on_testButton_clicked();

private:

    QTimer *timer;                                      //定时器
    int m_charWidth;
    int m_curIndex;
    QString m_showText;

    QSerialPort* m_serialPort;                          //串口类
    QStringList m_portNameList;                         //串口列表
    Ui::MainWindow *ui;



    void picInit();                                     //图像显示初始化
protected:
    void MainWindow::mousePressEvent(QMouseEvent *event);
    void MainWindow::paintEvent(QPaintEvent *e);

};



#endif // MAINWINDOW_H
