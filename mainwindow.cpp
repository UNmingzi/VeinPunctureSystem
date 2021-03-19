


#if _MSC_VER >= 1900
#pragma execution_character_set("utf-8")
#endif
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDate>
#include <QString>
#include <QDebug>
#include <QtAlgorithms>
#include <QPainter>
#include <QRect>
#include <QBrush>
#include <QFont>
#include <QPainter>
#include <QChar>
#include "updatepicthread.h"

bool widget_flag=0;
QString labelpro = "血管千万条，只能扎一条，血管扎不对，病人两行泪";

using namespace std;
using namespace cv;

static UpdatePicThread *threadUpdatePic;
static TrioPC *pcMat;                                       //驱动器对象指针
static SafeThread *threadSafe;
static ToZeroThread *threadZero;                                  //线程对象指针
static RecordThread *threadRecord;
static MyAxesDriver *axesControl;                                 //轴控制指针
static Laser *laser;                                              //laser对象指针

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /*界面初始化*/





    ui->setupUi(this);
    //this->setFixedSize(1264,645);                                           //固定界面大小
    this->setWindowIcon(QIcon(":/picture/Image/icon.png"));
    ui->radioButtonMode2->setChecked(1);
    ui->enable_light->setRed();
    ui->zero_light->setRed();

    //界面背景图像设置
    setAutoFillBackground(true);
    QPalette pal = this->palette();
    pal.setBrush(backgroundRole(), QPixmap(":/picture/Image/background2.jpg"));
    setPalette(pal);
//    picInit();                                                              //界面显示初始化

    /*实例化对象*/
    pcMat = new TrioPC;
    axesControl = new MyAxesDriver(pcMat);

    /*实例化线程*/
    //电机保护线程
    threadSafe = new SafeThread(pcMat,axesControl);
    //寻零线程
    threadZero = new ToZeroThread(pcMat,axesControl);

    //图像处理和更新线程
    threadUpdatePic = new UpdatePicThread(ui);
    threadUpdatePic->start();
    //threadUpdatePic->updateImage();
    //数据记录线程
    threadRecord = new RecordThread(pcMat, axesControl);

    //窗口定时器-数据刷新
    timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateWatch()));                       //关联定时器计满信号和响应的槽函数
    timer->start(50);                                                                  //主线程定时器开始，并设置定时器触发槽函数的时间

    //激光串口连接
    connect(&laserPort,&QSerialPort::readyRead,this,&MainWindow::laser_receive_data);

    //串口初始化
//    m_serialPort = new QSerialPort();
//    m_portNameList = getPortNameList();
//    QString tmp0 =  m_portNameList.at(0);
//    qDebug()<<"tmp0 ="<< tmp0;
//    QString tmp1 =  m_portNameList.at(1);
//    qDebug()<<"tmp1 ="<< tmp1;

//    openPort();
}

MainWindow::~MainWindow()
{
    axesControl->axesOff();                                                             //轴失能
    threadSafe->closeSafeThread();

    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
    }
    delete m_serialPort;

    delete ui;
}

/*主线程定时器：数据更新*/
void MainWindow::updateWatch()
{
    display_axes_status();                                 //更新轴状态
    nowPositionDisplay();                                  //更新实时位置

    static double i;
    i+=0.2;
    if(i >= 500)
        i = 0;
//    if(ui->tabWidget->currentIndex() == 5)
//    {
//        //chartUpdate(sin(i));
//        //chartUpdate(axesControl->analogRead(0));
////        receiveInfo();
//        int  intVar =(Motorreadforce()-2000);

//        qDebug() <<"yuan="<<intVar;
////        int  outIntVar;
////        memcpy(&outIntVar, analogReadNum.data(), 4);
////        qDebug() <<"now="<< outIntVar;
//        if(intVar !=200)
//        {
//                chartUpdate(intVar);

//        }

//    }
        //chartUpdate(12);

}


//图表更新程序
void MainWindow::chartUpdate(double num)
{
    //两种写法：MOde1、不断赋值并压缩，到非常大的数后清除（穿刺动作中显示）  Mode2、先排满0-100，再往后平移（不断读数时）
    int FLAG_SHOW_MODE=0;
    if(ui->radioButtonMode1->isChecked())
        FLAG_SHOW_MODE = 1;
    else if(ui->radioButtonMode2->isChecked())
        FLAG_SHOW_MODE = 2;
    QChart *chart = new QChart();
    static QLineSeries *series = new QLineSeries();
    series->setPen(QPen(Qt::blue,1,Qt::SolidLine)); //设置画笔


    static int count = 0;
    if(count > 200)                                //QLineSeries设定最大数据数
    {
        count = 0;
        series->clear();
    }
    //给lineseries赋值和chart配置
    if(FLAG_SHOW_MODE == 1)                         //模式1
    {
        series->append(count, num);
        chart->legend()->hide();                                        //去除边缘-蓝色点
        chart->addSeries(series);
        chart->createDefaultAxes();                                     //设置默认的坐标系，如果不设置程序是运行不了的
        chart->setTitle("Insert Stress");
        chart->axisY()->setRange(350, 400);
    }
    else if(FLAG_SHOW_MODE == 2)        //模式2
    {
        series->append(count, num);
        if(count > 100)
            series->remove(count-100);
        //chart配置
        chart->legend()->hide();                                        //去除边缘-蓝色点
        chart->addSeries(series);
        chart->createDefaultAxes();                                     //设置默认的坐标系，如果不设置程序是运行不了的
        chart->setTitle("Insert Stress");
        if(count <= 100)
            chart->axisX()->setRange(0, 100);                           //设置图表坐标轴的范围，可以不设置，自动调节的
        else
            chart->axisX()->setRange(count-100, count);                 //设置图表坐标轴的范围，可以不设置，自动调节的
        chart->axisY()->setRange(350, 400);
    }
    //chart显示
    ui->pressureView->setChart(chart);
    ui->pressureView->setRenderHint(QPainter::Antialiasing);        //抗锯齿渲染

    count++;
}
//获取串口号
QStringList MainWindow::getPortNameList()
{
    QStringList m_serialPortName;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        m_serialPortName << info.portName();
        qDebug()<<"serialPortName:"<<info.portName();
    }
    return m_serialPortName;
}
//打开串口
void MainWindow::openPort()
{
    if(m_serialPort->isOpen())//如果串口已经打开了 先给他关闭了
    {
        m_serialPort->clear();
        m_serialPort->close();
    }

    //m_serialPort->setPortName(m_PortNameComboBox->currentText());//当前选择的串口名字
    m_serialPort->setPortName(m_portNameList[0]);                   //当前指定串口

    if(!m_serialPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
    {
        qDebug()<<"打开失败!";
        return;
    }
    qDebug()<<"串口打开成功!";
    m_serialPort->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//设置波特率和读写方向
    m_serialPort->setDataBits(QSerialPort::Data8);      //数据位为8位
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
    m_serialPort->setParity(QSerialPort::NoParity); //无校验位
    m_serialPort->setStopBits(QSerialPort::OneStop); //一位停止位
    //connect(m_serialPort,SIGNAL(readyRead()),this,SLOT(receiveInfo()));
}

//接收到单片机发送的数据进行解析
QByteArray analogReadNum;

void MainWindow::receiveInfo()
{
//    QByteArray temp = m_serialPort->readAll();
//    static QByteArray byteArray;
//    if(!temp.isEmpty())
//    {
//        byteArray.append(temp);
//        if(byteArray.contains("#"))
//        {
//            analogReadNum = byteArray.left(byteArray.indexOf('#'));
//            qDebug()<<"serial port get:"<<analogReadNum;
//            byteArray = byteArray.right(byteArray.length()-byteArray.indexOf('#')-1);
//        }
//    }


//   int  intVar =(Motorreadforce()-2000);

//   qDebug() <<"yuan="<<intVar;


////   int len_intVar = sizeof(intVar);
//   analogReadNum.resize(4);
//   memcpy(analogReadNum.data(), &intVar, 4);
//    qDebug() <<"len_intVar="<<len_intVar;




}









/**********************************************************************************************************************************/


int  MainWindow::Motorreadforce()
{
    QByteArray ba;
     ba.resize(8);
     ba[0] = 0x55;
     ba[1] = 0xaa;
     ba[2] = 0x03;
     ba[3] = 0x01;
     ba[4] = 0x04;
     ba[5] = 0x00;
     ba[6] = 0x22;
     ba[7] = 0x2a;

     m_serialPort->write(ba);
     DelayTime(2);



     QByteArray temp=m_serialPort->readAll();
     QString str=temp;
//     ui->textEdit->append(str);
     quint8 buffer[22] = {0};
     int forcevalue=2200;
     if (!temp.isEmpty())
         {
          QDataStream out(&temp,QIODevice::ReadWrite);
          int i=0;
          int k=1;
          bool isGotFrameHeader = false;
           while (!out.atEnd())
               {
                 quint8 outChar=0;


                 out>>outChar;

                 buffer[i]=static_cast<quint8>(outChar&0xFF);
                 i++;
                 if(i==2)
                 {
                     i=0;
                     if(buffer[0]==static_cast<quint8>(0xaa)&&buffer[1]==0x55)
                     {
                         isGotFrameHeader = true;
                     }
                 }


                 if (isGotFrameHeader)
                 {
                     buffer[k]=static_cast<quint8>(outChar&0xFF);
                     k++;
                 }
                 if (k==22)
                 {
                     k=1;
                     isGotFrameHeader = false;

                 }

                }

           //qDebug()<<temp;
           buffer[0]=0xaa;
           buffer[1]=0x55;
           forcevalue=buffer[18]*256+buffer[17];


          }



//     dataReceived(forcevalue);

//     QString str=temp;

//     ui->textEdit->append(str);
     return  forcevalue;


}



int  MainWindow::Motorreadpos()
{
    QByteArray ba;
     ba.resize(8);
     ba[0] = 0x55;
     ba[1] = 0xaa;
     ba[2] = 0x03;
     ba[3] = 0x01;
     ba[4] = 0x04;
     ba[5] = 0x00;
     ba[6] = 0x22;
     ba[7] = 0x2a;

     m_serialPort->write(ba);
     DelayTime(2);



     QByteArray temp=m_serialPort->readAll();
     quint8 buffer[22] = {0};
     int forcevalue=0;
     if (!temp.isEmpty())
         {
          QDataStream out(&temp,QIODevice::ReadWrite);
          int i=0;
          int k=1;
          bool isGotFrameHeader = false;
           while (!out.atEnd())
               {
                 quint8 outChar=0;


                 out>>outChar;

                 buffer[i]=static_cast<quint8>(outChar&0xFF);
                 i++;
                 if(i==2)
                 {
                     i=0;
                     if(buffer[0]==static_cast<quint8>(0xaa)&&buffer[1]==0x55)
                     {
                         isGotFrameHeader = true;
                     }
                 }


                 if (isGotFrameHeader)
                 {
                     buffer[k]=static_cast<quint8>(outChar&0xFF);
                     k++;
                 }
                 if (k==22)
                 {
                     k=1;
                     isGotFrameHeader = false;

                 }

                }

           //qDebug()<<temp;
           buffer[0]=0xaa;
           buffer[1]=0x55;


          }

     forcevalue=buffer[10]*256+buffer[9];

//     dataReceived(forcevalue);

//     QString str=temp;

//     ui->textEdit->append(str);
     return  forcevalue;


}


void  MainWindow::Motormove(int destination)
{
    int pos= destination;
    int dec=pos/256;
    int middec=(93+pos-dec*255)/256;
    QByteArray ba;
     ba.resize(9);
     ba[0] = 0x55;
     ba[1] = 0xaa;
     ba[2] = 0x04;
     ba[3] = 0x01;
     ba[4] = 0x21;
     ba[5] = 0x37;
     ba[6] = pos-dec*256;
     ba[7] = dec;
     ba[8] = (93+pos-dec*255)-middec*256;
      m_serialPort->write(ba);

       qDebug()<<"test ="<<pos;
}




















































































/**********************************************************************************************************************************/


/*开始记录*/
void MainWindow::on_buttonRecordStart_clicked()
{
    threadRecord->start();
}

/*停止记录*/
void MainWindow::on_buttonRecordStop_clicked()
{
    /*关闭多线程*/
    threadRecord->closeThread();                                                                //thread1中自定义函数，给标识符置位
    threadRecord->wait();                                                                       //等待线程结束
}


/*点击寻零按键*/
void MainWindow::on_buttonHoming_clicked()
{
    qDebug()<<tr("tozero!!!!!!!!!!!!");                                                  //线程结束后触发函数
    axesControl->axesInit();                                                                //轴系初始化
    axesControl->axesOn();                                                                  //伺服On
    threadSafe->start();                                                                    //打开保护线程

    /*寻零标志位初始化*/
    for(int i=0; i < AXES_COUNT; i++)
        AXIS_TOZERO_FLAG[i] = false;

    Motormove(0);
    /*开始寻零*/
    threadZero->start();                                                                      //自动运行run()
   //axesControl->axesInMoveRev(6, moveZeroPos);                                              //多轴控制
    ui->enable_light->setGreen();
    ui->zero_light->setGreen();
}


void MainWindow::scrollCaption()
{
    static int nPos = 0; //当截取的位置比字符串长时，从头开始
                               if (nPos > labelpro .length())
                                   nPos = 0; //设置控件文本
                               ui->label->setText(labelpro.mid(nPos));
                                                          nPos++;
}


double* MainWindow::nowPosition()
{
    double* posArr=new double[6];
    double* jointsPos=new double[6];
    jointsPos=axesControl->getJointsPos();
    posArr=MyAlgorithm::forKinematics(jointsPos);
    delete [] jointsPos;
    return posArr;

}

void MainWindow::nowPositionDisplay()
{
  double* nowPositionArr=new double[6];
  nowPositionArr=nowPosition();
  ui->xActPos->setText(QString::number(*(nowPositionArr),10,2));
  ui->yActPos->setText(QString::number(*(nowPositionArr+1),10,2));
  ui->zActPos->setText(QString::number(*(nowPositionArr+2),10,2));
  ui->yawActPos->setText(QString::number(*(nowPositionArr+3),10,2));
  ui->pitchActPos->setText(QString::number(*(nowPositionArr+4),10,2));
  ui->disActPos->setText(QString::number(*(nowPositionArr+5),10,2));
  delete [] nowPositionArr;
}

double* MainWindow::getTargetPos()
{
    double* targetPosArr=new double[6];
    *(targetPosArr)=ui->xTargetPosition->text().toDouble();
    *(targetPosArr+1)=ui->yTargetPosition->text().toDouble();
    *(targetPosArr+2)=ui->zTargetPosition->text().toDouble();
    *(targetPosArr+3)=ui->yawTargetPosition->text().toDouble();
    *(targetPosArr+4)=ui->pitchTargetPosition->text().toDouble();
    *(targetPosArr+5)=ui->disTargetPosition->text().toDouble();
    return targetPosArr;

}

/*运动到给定值*/
void MainWindow::on_pushButton_clicked()
{
    double* targetPosArr=new double[6];
    double* jointsPos=new double[6];
    targetPosArr=getTargetPos();

    jointsPos=MyAlgorithm::invKinematics(targetPosArr);
    bool isrun=(!axesControl->axesIsRuning());
    qDebug() << isrun;

    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);

    delete[] targetPosArr;
    delete[] jointsPos;

//    QString str=ui->disTargetPosition->text();
    double posfive =ui->disTargetPosition->text().toDouble();
    int num=static_cast<int>(posfive/0.008);
//    int num = str.toInt();
    qDebug() << num;
    Motormove(num);
}

/*程序测试*/
void MainWindow::on_testButton_clicked()
{
    double* targetPosArr=new double[6];
    double* jointsPos=new double[6];
    targetPosArr=getTargetPos();

    jointsPos=MyAlgorithm::invKinematics(targetPosArr);
    if (!axesControl->axesIsRuning())
     axesControl->jointsTrajectoryMove(AXES_COUNT,jointsPos);
//    axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);

    delete[] targetPosArr;
    delete[] jointsPos;


}

void MainWindow::on_closeButton_clicked()
{
    QApplication::exit();
}

/*显示轴系状态*/
void MainWindow::display_axes_status()
{
    double axis_status;
    pcMat->GetAxisVariable(tr("AXISSTATUS"),0,axis_status);
    if(axis_status==0)
        ui->axis1_light->setGreen();
    else
        ui->axis1_light->setRed();

    pcMat->GetAxisVariable(tr("AXISSTATUS"),1,axis_status);
    if(axis_status==0)
        ui->axis2_light->setGreen();
    else
        ui->axis2_light->setRed();

    pcMat->GetAxisVariable(tr("AXISSTATUS"),2,axis_status);
    if(axis_status==0)
        ui->axis3_light->setGreen();
    else
        ui->axis3_light->setRed();

    pcMat->GetAxisVariable(tr("AXISSTATUS"),3,axis_status);
    if(axis_status==0)
        ui->axis4_light->setGreen();
    else
        ui->axis4_light->setRed();

    pcMat->GetAxisVariable(tr("AXISSTATUS"),4,axis_status);
    if(axis_status==0)
        ui->axis5_light->setGreen();
    else
        ui->axis5_light->setRed();

    pcMat->GetAxisVariable(tr("AXISSTATUS"),5,axis_status);
    if(axis_status==0)
        ui->axis6_light->setGreen();
    else
        ui->axis6_light->setRed();

}


/*图像显示初始化*/
void MainWindow::picInit()
{
    QImage* image=new QImage;
    QImage resultImg;
    /*左图*/
    image->load(":/picture/Image/start.jpg");
    resultImg = image->scaled(ui->picLeft->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->picLeft->setPixmap(QPixmap::fromImage(resultImg));
    ui->picLeft->setScaledContents(true);

    resultImg = image->scaled(ui->picLeft2->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->picLeft2->setPixmap(QPixmap::fromImage(resultImg));
    ui->picLeft2->setScaledContents(true);

    /*右图*/
    //image->load(":/picture/Image/start.jpg");
    resultImg = image->scaled(ui->picRight->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->picRight->setPixmap(QPixmap::fromImage(resultImg));
    ui->picRight->setScaledContents(true);

    resultImg = image->scaled(ui->picRight2->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->picRight2->setPixmap(QPixmap::fromImage(resultImg));
    ui->picRight2->setScaledContents(true);

    /*分割图*/
    //image->load(":/picture/Image/start.jpg");
    resultImg = image->scaled(ui->picCut->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->picCut->setPixmap(QPixmap::fromImage(resultImg));                                                 //小图
    ui->picCut->setScaledContents(true);

    resultImg = image->scaled(ui->picCut2->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->picCut2->setPixmap(QPixmap::fromImage(resultImg));                                                //大图
    ui->picCut2->setScaledContents(true);

    /*三维图*/
    //image->load(":/picture/Image/start.jpg");
    resultImg = image->scaled(ui->picPcl->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->picPcl->setPixmap(QPixmap::fromImage(resultImg));                                          //小图
    ui->picPcl->setScaledContents(true);

    resultImg = image->scaled(ui->picPcl2->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->picPcl2->setPixmap(QPixmap::fromImage(resultImg));                                      //大图
    ui->picPcl2->setScaledContents(true);

    /*右下角针头图*/
//    image->load(":/picture/Image/start.jpg");
//    resultImg = image->scaled(ui->picNeedle->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
//    ui->picNeedle->setPixmap(QPixmap::fromImage(resultImg));                                          //小图
//    ui->picNeedle->setScaledContents(true);
}

/*连接控制器*/
void MainWindow::on_buttonConnect_clicked()
{
   //updateWidget *myUpdateThread=new updateWidget(ui);
    if(!pcMat->IsOpen(3240))
        {
            //建立通讯链接
            pcMat->SetHostAddress(tr("192.168.0.250"));
            pcMat->Open(2,3240);                                                            //Port Type:2 Ethernet;PortMode:0/3240 Synchronous on specified port number

            if(pcMat->IsOpen(3240))
            {
                axesControl->axesInit();                                                    //轴初始化
                qDebug()<< "Date:" << QDate::currentDate()<<tr("建立通讯链接");
                ui->sysOutput->append(tr("建立通讯链接"));
            }

            else
           { qDebug()<< "Date:" << QDate::currentDate()<<tr("无法建立通讯链接");
             ui->sysOutput->append(tr("无法建立通讯链接"));
            }

        }
    //myUpdateThread->start();
}

/*设置参数*/
void MainWindow::on_paraAdjustButton_clicked()   //运动参数调整
{
    pcMat->SetAxisVariable(tr("P_GAIN"), 0, ui->axis0KP->value());
    pcMat->SetAxisVariable(tr("I_GAIN"), 0, ui->axis0KI->value());
    pcMat->SetAxisVariable(tr("D_GAIN"), 0, ui->axis0KD->value());
    pcMat->SetAxisVariable(tr("SPEED"), 0, ui->axis0V->value());
    pcMat->SetAxisVariable(tr("ACCEL"), 0, ui->axis0A->value());
    pcMat->SetAxisVariable(tr("DECEL"), 0, ui->axis0A->value());

    pcMat->SetAxisVariable(tr("P_GAIN"), 1, ui->axis1KP->value());
    pcMat->SetAxisVariable(tr("I_GAIN"), 1, ui->axis1KI->value());
    pcMat->SetAxisVariable(tr("D_GAIN"), 1, ui->axis1KD->value());
    pcMat->SetAxisVariable(tr("SPEED"), 1, ui->axis1V->value());
    pcMat->SetAxisVariable(tr("ACCEL"), 1, ui->axis1A->value());
    pcMat->SetAxisVariable(tr("DECEL"), 1, ui->axis1A->value());


    pcMat->SetAxisVariable(tr("P_GAIN"), 2, ui->axis2KP->value());
    pcMat->SetAxisVariable(tr("I_GAIN"), 2, ui->axis2KI->value());
    pcMat->SetAxisVariable(tr("D_GAIN"), 2, ui->axis2KD->value());
    pcMat->SetAxisVariable(tr("SPEED"), 2, ui->axis2V->value());
    pcMat->SetAxisVariable(tr("ACCEL"), 2, ui->axis2A->value());
    pcMat->SetAxisVariable(tr("DECEL"), 2, ui->axis2A->value());

    pcMat->SetAxisVariable(tr("P_GAIN"), 3, ui->axis3KP->value());
    pcMat->SetAxisVariable(tr("I_GAIN"), 3, ui->axis3KI->value());
    pcMat->SetAxisVariable(tr("D_GAIN"), 3, ui->axis3KD->value());
    pcMat->SetAxisVariable(tr("SPEED"), 3, ui->axis3V->value());
    pcMat->SetAxisVariable(tr("ACCEL"), 3, ui->axis3A->value());
    pcMat->SetAxisVariable(tr("DECEL"), 3, ui->axis3A->value());

    pcMat->SetAxisVariable(tr("P_GAIN"), 4, ui->axis4KP->value());
    pcMat->SetAxisVariable(tr("I_GAIN"), 4, ui->axis4KI->value());
    pcMat->SetAxisVariable(tr("D_GAIN"), 4, ui->axis4KD->value());
    pcMat->SetAxisVariable(tr("SPEED"), 4, ui->axis4V->value());
    pcMat->SetAxisVariable(tr("ACCEL"), 4, ui->axis4A->value());
    pcMat->SetAxisVariable(tr("DECEL"), 4, ui->axis4A->value());

    pcMat->SetAxisVariable(tr("P_GAIN"), 5, ui->axis5KP->value());
    pcMat->SetAxisVariable(tr("I_GAIN"), 5, ui->axis5KI->value());
    pcMat->SetAxisVariable(tr("D_GAIN"), 5, ui->axis5KD->value());
    pcMat->SetAxisVariable(tr("SPEED"), 5, ui->axis5V->value());
    pcMat->SetAxisVariable(tr("ACCEL"), 5, ui->axis5A->value());
    pcMat->SetAxisVariable(tr("DECEL"), 5, ui->axis5A->value());
}

void MainWindow::on_radioButtonEnable_clicked()
{
    if(ui->radioButtonEnable->isChecked())
    {
        //axesControl->axesOn();
        qDebug()<< tr("使能");
        ui->sysOutput->append(tr("使能"));

    }
    else
    {
        //axesControl->axesOn();
        qDebug()<< tr("失能");
        ui->sysOutput->append(tr("失能"));
    }
}


void MainWindow::on_xPosAdd_clicked()
{
    double* nowPositionArr=new double[6];
    double* jointsPos=new double[6];
    nowPositionArr=nowPosition();
    *nowPositionArr=*nowPositionArr+1;
    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);

    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);

    for(int i = 0; i < 6; i++)
    {

        qDebug()<<"关节"<<i<<"位置"<<jointsPos[i];

    }

    delete[] nowPositionArr;
    delete[] jointsPos;
}

void MainWindow::on_xPosMinus_clicked()
{
        double* nowPositionArr=new double[6];
        double* jointsPos=new double[6];
        nowPositionArr=nowPosition();
        *nowPositionArr=*nowPositionArr-1;
        jointsPos=MyAlgorithm::invKinematics(nowPositionArr);

        if (!axesControl->axesIsRuning())
         axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);
        for(int i = 0; i < 6; i++)
        {

            qDebug()<<"关节"<<i<<"位置"<<jointsPos[i];

        }

        delete[] nowPositionArr;
        delete[] jointsPos;
}

void MainWindow::on_yPosAdd_clicked()
{
    double* nowPositionArr=new double[6];
    double* jointsPos=new double[6];
    nowPositionArr=nowPosition();
    *(nowPositionArr+1)=*(nowPositionArr+1)+1;
    qDebug()<<*(nowPositionArr+1);
    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);

    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);
    for(int i = 0; i < 6; i++)
    {

        qDebug()<<"关节"<<i<<"位置"<<jointsPos[i];

    }

    delete[] nowPositionArr;
    delete[] jointsPos;
}

void MainWindow::on_yPosMinus_clicked()
{
    double* nowPositionArr=new double[6];
    double* jointsPos=new double[6];
    nowPositionArr=nowPosition();
    *(nowPositionArr+1)=*(nowPositionArr+1)-1;
    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);
    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);
    for(int i = 0; i < 6; i++)
    {

        qDebug()<<"关节"<<i<<"位置"<<jointsPos[i];

    }

    delete[] nowPositionArr;
    delete[] jointsPos;
}

void MainWindow::on_zPosAdd_clicked()
{
    double* nowPositionArr=new double[6];
    double* jointsPos=new double[6];
    nowPositionArr=nowPosition();
    *(nowPositionArr+2)=*(nowPositionArr+2)+1;
    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);
    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);
    for(int i = 0; i < 6; i++)
    {

        qDebug()<<"关节"<<i<<"位置"<<jointsPos[i];

    }

    delete[] nowPositionArr;
    delete[] jointsPos;
}

void MainWindow::on_zPosMinus_clicked()
{
    double* nowPositionArr=new double[6];
    double* jointsPos=new double[6];
    nowPositionArr=nowPosition();
    *(nowPositionArr+2)=*(nowPositionArr+2)-1;
    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);
    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);
    for(int i = 0; i < 6; i++)
    {

        qDebug()<<"关节"<<i<<"位置"<<jointsPos[i];

    }

    delete[] nowPositionArr;
    delete[] jointsPos;
}

void MainWindow::on_yawPosAdd_clicked()
{
    double* nowPositionArr=new double[6];
    double* jointsPos=new double[6];
    nowPositionArr=nowPosition();
    *(nowPositionArr+3)=*(nowPositionArr+3)+1;
    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);
    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);
    for(int i = 0; i < 6; i++)
    {

        qDebug()<<"关节"<<i<<"位置"<<jointsPos[i];

    }

    delete[] nowPositionArr;
    delete[] jointsPos;
}

void MainWindow::on_yawPosMinus_clicked()
{
    double* nowPositionArr=new double[6];
    double* jointsPos=new double[6];
    nowPositionArr=nowPosition();
    *(nowPositionArr+3)=*(nowPositionArr+3)-1;
    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);
    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);
    for(int i = 0; i < 6; i++)
    {

        qDebug()<<"关节"<<i<<"位置"<<jointsPos[i];

    }

    delete[] nowPositionArr;
    delete[] jointsPos;
}

void MainWindow::on_pitchPosAdd_clicked()
{
    double* nowPositionArr=new double[6];
    double* jointsPos=new double[6];
    nowPositionArr=nowPosition();
    *(nowPositionArr+4)=*(nowPositionArr+4)+1;
    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);
    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);
    for(int i = 0; i < 6; i++)
    {

        qDebug()<<"关节"<<i<<"位置"<<jointsPos[i];

    }

    delete[] nowPositionArr;
    delete[] jointsPos;
}

void MainWindow::on_pitchPosMinus_clicked()
{
    double* nowPositionArr=new double[6];
    double* jointsPos=new double[6];
    nowPositionArr=nowPosition();
    *(nowPositionArr+4)=*(nowPositionArr+4)-1;
    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);
    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);
    for(int i = 0; i < 6; i++)
    {

        qDebug()<<"关节"<<i<<"位置"<<jointsPos[i];

    }

    delete[] nowPositionArr;
    delete[] jointsPos;
}

void MainWindow::on_disPosAdd_clicked()
{
    double* nowPositionArr=new double[6];
    double* jointsPos=new double[6];
    nowPositionArr=nowPosition();
    *(nowPositionArr+5)=*(nowPositionArr+5)+1;
    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);
    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);
    for(int i = 0; i < 6; i++)
    {

        qDebug()<<"关节"<<i<<"位置"<<jointsPos[i];

    }

    delete[] nowPositionArr;
    delete[] jointsPos;
}

void MainWindow::on_disPosMinus_clicked()
{
    double* nowPositionArr=new double[6];
    double* jointsPos=new double[6];
    nowPositionArr=nowPosition();
    *(nowPositionArr+5)=*(nowPositionArr+5)-1;
    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);
    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);
    for(int i = 0; i < 6; i++)
    {

        qDebug()<<"关节"<<i<<"位置"<<jointsPos[i];

    }

    delete[] nowPositionArr;
    delete[] jointsPos;
}


/*穿刺按钮*/
void MainWindow::on_punctureButton_clicked()
{
    double* nowPositionArr=new double[6];
    double* jointsPos=new double[6];
    nowPositionArr=nowPosition();
    *(nowPositionArr+5)=15;
    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);
    if (!axesControl->axesIsRuning())
     axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);
//    while(axesControl->axesIsRuning())
//    *(nowPositionArr+5)=3;
//    jointsPos=MyAlgorithm::invKinematics(nowPositionArr);
//    axesControl->jointsOutMoveAbs(AXES_COUNT,jointsPos);

    delete[] nowPositionArr;
    delete[] jointsPos;
}

/*运行按钮*/
void MainWindow::on_run_clicked()
{
    double* nowPositionArr=new double[6];
    nowPositionArr=nowPosition();



    double dx, dy, dz, ds;
    dz = 6*qSin(decPosArr[4]*M_PI/180);
    ds = 6*qCos(decPosArr[4]*M_PI/180);
    dx = ds*qCos(decPosArr[3]*M_PI/180);
    dy = ds*qSin(decPosArr[3]*M_PI/180);
    //double tarPos[6] = {decPosArr[0] - dx, decPosArr[1] - dy, decPosArr[2] - dz, decPosArr[3], 5, 0};    //预穿刺点
    double tarPos[6] ={164,17,-2,33,2,0};
    double stepPos[6][6] = {{tarPos[0], tarPos[1], tarPos[2], tarPos[3], tarPos[4], tarPos[5]},           //运动到预穿刺点
                            {tarPos[0], tarPos[1], tarPos[2], tarPos[3], tarPos[4], tarPos[5] + 10},       //执行穿刺
                            {tarPos[0], tarPos[1], tarPos[2], tarPos[3], 16, tarPos[5] + 10},              //挑针
                            {tarPos[0], tarPos[1], tarPos[2], tarPos[3], 16, tarPos[5] + 16},             //平推
                            {tarPos[0], tarPos[1], tarPos[2], tarPos[3], 16, 0},                          //拔出
                            {130, 10, 21, 30, 10, 0}}; //返回
//     double midPos[6];
//     int internum=2;
//     double step[6];
//     double* jointsPos=new double[6];


//      for(int j = 0; j <(internum+1) ; j++)
//      {


//           qDebug() <<"进入j="<< j;
//          for(int i = 0; i < 6; i++)
//          {
//               step[i] =(tarPos[i]-(*(nowPositionArr+i)))/internum;

//              midPos[i]= (*(nowPositionArr+i))+step[i]*j;
//              qDebug() <<"进入reachi="<< i;


//          }
////          midPos[5]= 0;

//          jointsPos=MyAlgorithm::invKinematics(midPos);

//          axesControl->  jointsOutMoveAbs(AXES_COUNT,jointsPos);


////
////           axesControl->needleMoveAbs(midPos);
////           qDebug() <<"进入j="<< j;
//           bool isRun=true;
//           while (isRun)
//           {


//              double* nowinterpos=new double[6];
//              nowinterpos=nowPosition();

//              bool  reach[3]={false,false,false};
//              for(int i = 0; i < 3; i++)
//              {

//                   axesControl->  jointsOutMoveAbs(AXES_COUNT,jointsPos);

//                  if(qAbs((*(nowinterpos+i))-midPos[i])<0.1)
//                 {
//                     reach[i]=true;
//                     qDebug() <<"进入reachi="<< i;
//                 }


//              }


//              if(reach[2])
//              {
//                 isRun=false;


//              }

//           }

//           qDebug() <<"进入j="<< j;


//      }

//      while(axesControl->axesIsRuning());





    for(int i = 0; i < 6; i++)
    {

         qDebug() <<"进入循环"<< i;
        while(axesControl->axesIsRuning());//等待所有轴运动停止
         axesControl->needleMoveAbs(stepPos[i]);
//             qDebug() <<"进入while";
         while(axesControl->axesIsRuning());


        double posfive =stepPos[i][5];
        int num=static_cast<int>(posfive/0.008);
    //    int num = str.toInt();
        qDebug() <<"末端位置"<< num;
//        DelayTime(500);
        Motormove(num);

        DelayTime(1000);
        bool isRun=true;
        while (isRun)
        {
           int nowpos=Motorreadpos();
           if((qAbs(nowpos-num)<5)&&(axesControl->axesIsRuning()==false))
           {
              isRun=false;
              qDebug() <<"进入run"<< i;

           }

        }


    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int mainX,mainY;
    this->pos();
    mainX=this->pos().x();
    mainY=this->pos().y();
    //cout<<mainX<<"   "<<mainY<<endl;
    if(ui->pickPoint1->isChecked()&&!ui->pickPoint2->isChecked())
    {
        if((QCursor::pos().x()-mainX)>=71&&(QCursor::pos().x()-mainX)<=391
                &&(QCursor::pos().y()-mainY)>=372&&(QCursor::pos().y()-mainY)<=612)
        {
            setCursor(Qt::CrossCursor);
            int mouse_x = QCursor::pos().x();//鼠标点击处横坐标
            int mouse_y = QCursor::pos().y();//鼠标点击处纵坐标
            QWidget *action = QApplication::widgetAt(mouse_x, mouse_y);//获取鼠标点击处的控件
            QPoint GlobalPoint(action->mapToParent(QPoint(0, 0)));//获取该控件在窗体中的坐标
            int labelX = GlobalPoint.x();//该控件在窗体中左上角横坐标
            int labelY = GlobalPoint.y();//该控件在窗体中左上角纵坐标
            int aimPositionX=event->pos().x()-labelX-20;
            int aimPositionY=event->pos().y()-labelY-40;
            double positionFactX=88.305+aimPositionY*0.855;
            double positionFactY=-134.63+aimPositionX*0.826;
            double depth = threadUpdatePic->disp.at<short>(aimPositionX,aimPositionY);
            QString msg1=QString("press:%1,%2").arg(labelX).arg(labelY);
            QString msg2=QString("press:%1,%2").arg(event->pos().x()-labelX-20).arg(event->pos().y()-labelY-40);
            QString msg=QString("Point Position and Depth:(%1,%2),%3").arg(positionFactX).arg(positionFactY).arg(depth);
            //cout<<labelX<<","<<labelY<<endl;

            ui->pointPosition->setText(msg);

            decPosArr[0] = positionFactX;
            decPosArr[1] = positionFactY;
            decPosArr[2] = 5;
            decPosArr[3] = 30;
            decPosArr[4] = 2.5;
            decPosArr[5] = 3;

        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    }
    else if(!ui->pickPoint1->isChecked()&&ui->pickPoint2->isChecked())
    {
        if((QCursor::pos().x()-mainX)>=40&&(QCursor::pos().x()-mainX)<=680
                &&(QCursor::pos().y()-mainY)>=80&&(QCursor::pos().y()-mainY)<=560)
        {
            setCursor(Qt::CrossCursor);
            int mouse_x = QCursor::pos().x();//鼠标点击处横坐标
            int mouse_y = QCursor::pos().y();//鼠标点击处纵坐标
            QWidget *action = QApplication::widgetAt(mouse_x, mouse_y);//获取鼠标点击处的控件
            QPoint GlobalPoint(action->mapToParent(QPoint(0, 0)));//获取该控件在窗体中的坐标
            //int labelX = GlobalPoint.x();//该控件在窗体中左上角横坐标
            //int labelY = GlobalPoint.y();//该控件在窗体中左上角纵坐标
            int aimPositionX=event->pos().x();    //待修改
            int aimPositionY=event->pos().y();
             double depth = threadUpdatePic->disp.at<short>(aimPositionX/2,aimPositionY/2);
            //QString msg1=QString("press:%1,%2").arg(labelX).arg(labelY);
            //QString msg2=QString("press:%1,%2").arg(event->pos().x()).arg(event->pos().y());
             QString msg=QString("Point Position and Depth:(%1,%2),%3").arg(aimPositionX).arg(aimPositionY).arg(depth);

            ui->pointPosition->setText(msg);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    }
    else if(ui->pickPoint1->isChecked()&&ui->pickPoint2->isChecked())
    {
        setCursor(Qt::ArrowCursor);
        QMessageBox::warning(this,"Error","Only one checkbox can be clicked!",QMessageBox::Yes);
    }

}

void  MainWindow::paintEvent(QPaintEvent *e)
{
//    Mat RGBLeft=threadUpdatePic->RGBLeft;
//    Mat RGBRight=threadUpdatePic->RGBRight;
//    Mat imageCut=threadUpdatePic->imageCut;
//    Mat dispRGB=threadUpdatePic->dispRGB;

//    QImage imageLeft(RGBLeft.data,RGBLeft.cols,RGBLeft.rows,QImage::Format_RGB888);
//    ui->picLeft2->setPixmap(QPixmap::fromImage(imageLeft));
//    ui->picLeft2->resize(imageLeft.size());
//    ui->picLeft2->show();
//    imageLeft=imageLeft.scaled(320,240);
//    ui->picLeft->setPixmap(QPixmap::fromImage(imageLeft));
//    ui->picLeft->resize(imageLeft.size());
//    ui->picLeft->show();

//    QImage imageRight(RGBRight.data,RGBRight.cols,RGBRight.rows,QImage::Format_RGB888);
//    ui->picRight2->setPixmap(QPixmap::fromImage(imageRight));
//    ui->picRight2->resize(imageRight.size());
//    ui->picRight2->show();
//    imageRight=imageRight.scaled(320,240);
//    ui->picRight->setPixmap(QPixmap::fromImage(imageRight));
//    ui->picRight->resize(imageRight.size());
//    ui->picRight->show();

//    QImage imageCutQ(imageCut.data,imageCut.cols,imageCut.rows,QImage::Format_RGB888);
//    ui->picCut2->setPixmap(QPixmap::fromImage(imageCutQ));
//    ui->picCut2->resize(imageCutQ.size());
//    ui->picCut2->show();
//    imageCutQ=imageCutQ.scaled(320,240);
//    ui->picCut->setPixmap(QPixmap::fromImage(imageCutQ));
//    ui->picCut->resize(imageCutQ.size());
//    ui->picCut->show();

//    QImage imageDisp(dispRGB.data,dispRGB.cols,dispRGB.rows,QImage::Format_RGB888);
//    ui->picPcl2->setPixmap(QPixmap::fromImage(imageDisp));
//    ui->picPcl2->resize(imageDisp.size());
//    ui->picPcl2->show();
//    imageDisp=imageDisp.scaled(320,240);
//    ui->picPcl->setPixmap(QPixmap::fromImage(imageDisp));
//    ui->picPcl->resize(imageDisp.size());
//    ui->picPcl->show();

}

void MainWindow::on_btn_open_clicked()
{
    QString selectedLaserPort=ui->cmb_port_name->currentText();

    //fix me com端口参数合法性检验

    qDebug()<<selectedLaserPort;

    //port config
    laserPort.setBaudRate(QSerialPort::Baud9600);
    laserPort.setParity(QSerialPort::NoParity);
    laserPort.setDataBits(QSerialPort::Data8);
    laserPort.setStopBits(QSerialPort::OneStop);
    laserPort.setPortName(selectedLaserPort);

    laserPort.open(QIODevice::ReadWrite);

    if(laserPort.isOpen())
    {
        qDebug()<<selectedLaserPort<<"is open.";
        ui->lab_connect_staus->setText("连接");
    }
    else
    {
        qDebug()<<selectedLaserPort<<"connect error.";
    }


}

void MainWindow::on_btn_close_clicked()
{
    QString selectedLaserPort=ui->cmb_port_name->currentText();

    //fix me com端口参数合法性检验

    qDebug()<<selectedLaserPort;

    laserPort.close();

    if(!laserPort.isOpen())
    {
        qDebug()<<selectedLaserPort<<"is closed.";
        ui->lab_connect_staus->setText("断开");
    }
    else
    {
        qDebug()<<selectedLaserPort<<"disconnect error.";
    }
}

void MainWindow::on_btn_searchport_clicked()
{
    ui->cmb_port_name->addItems(getPortNameList()); //激光下拉菜单列出串口号列表
    qDebug()<<getPortNameList();
}

void MainWindow::laser_receive_data()
{
    QByteArray receivedData = laserPort.readAll();
    float receivedData_float;

    //检查返回值error
    if((receivedData.toHex().at(3)=='E')||(receivedData.toHex().at(4)=='R')||(receivedData.toHex().at(5)=='R'))
    {
        qDebug()<<"Received laser data error.";
    }
    else
    {
        qDebug()<<"Received laser data:"<<receivedData;

        receivedData_float = laserDataProcess(receivedData).toFloat();
        qDebug()<<"Actual laser data:"<<receivedData_float;
    }

    //qDebug()<<"Received data:"<<receivedData.at(3)<<receivedData.at(2)<<receivedData.at(1);
    //ui->plainTextEdit->insertPlainText(QString(laserDataProcess(receivedData).toHex(' ')).append(' '));
    ui->plainTextEdit->insertPlainText(QString(laserDataProcess(receivedData)).append(' '));
}

void MainWindow::on_btn_measure_clicked()
{
    //发送16进制数"80060278" 单次测量
    QByteArray sendData;
     sendData.resize(4);
     sendData[0] = (char)0x80;
     sendData[1] = (char)0x06;
     sendData[2] = (char)0x02;
     sendData[3] = (char)0x78;

    //qDebug()<<"Send data:"<<sendData.toHex();

    laserPort.write(sendData);
}

QByteArray MainWindow::laserDataProcess(QByteArray data)
{
    data = data.right(9);
    data = data.left(8);

    return data;
}
