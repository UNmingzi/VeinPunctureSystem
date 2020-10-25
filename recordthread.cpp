#include "recordthread.h"
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <iostream>
#include <mainwindow.h>
#pragma execution_character_set("utf-8")

using namespace std;

RecordThread::RecordThread(TrioPC *pcMatParent, MyAxesDriver *axesControlParent)
{
    pcMat=pcMatParent;                                                                      //传递对象指针
    axesControl = axesControlParent;
    isStop = false;
}

void RecordThread::closeThread()
{
    qDebug()<<tr("close record");                                                  //线程结束后触发函数
    isStop = true;
}

void RecordThread::run()
{
    qDebug()<<tr("start record");                                                  //线程结束后触发函数
    //串口初始化


    //创建文件夹
    QDir tempDir;
    QDateTime dataTime;
    QString timeStr=dataTime.currentDateTime().toString("yyyyMMddHHmmss");
    QString foldPath = "C:/Users/Jier/Desktop/record/" + timeStr;
    if(!tempDir.exists(foldPath))
    {
        qDebug()<<QObject::tr("不存在该路径")<<endl;
        tempDir.mkpath(foldPath);
    }
    //创建文件目录
    QString filePathTorque = foldPath + "/1torque.txt";
    QString filePathPosNow = foldPath + "/2posnow.txt";
    QString filePathPosAim = foldPath + "/3posaim.txt";
    QString filePathPressure = foldPath + "/4pressure.txt";
    QFile fileTorque(filePathTorque);                               //力矩记录
    QFile filePosNow(filePathPosNow);                               //现在位置记录
    QFile filePosAim(filePathPosAim);                               //实际位置记录
    QFile filePressure(filePathPressure);                           //穿刺力记录

    if(!fileTorque.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<tr("Open failed.");
        return;
    }
    if(!filePosNow.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<tr("Open failed.");
        return;
    }
    if(!filePosAim.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<tr("Open failed.");
        return;
    }
    if(!filePressure.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<tr("Open failed.");
        return;
    }
    QTextStream txtoutTorque(&fileTorque);
    QTextStream txtoutPosNow(&filePosNow);
    QTextStream txtoutPosAim(&filePosAim);
    QTextStream txtoutPressure(&filePressure);

//    txtoutTorque << "Torque" << endl;
//    txtoutPosNow << "PosNow" << endl;
//    txtoutPosAim << "PosAim" << endl;
//    txtoutPressure << "pressure" << endl;

    while (1)
    {
        if(isStop)
        {
            //输出到文件
            fileTorque.close();
            filePosNow.close();
            filePosAim.close();
            filePressure.close();
            isStop = false;
            return;
        }
        //记录力矩
        txtoutTorque << axesControl->axisGetTorque(0) << endl;            //double axisGetTorque(int axisNum);

        //目标位置
        //txtoutPosNow << axesControl->axisGetPosAim(0) << endl;
        double* jointArr = new double[6];                                                                    //关节差值
        double* jointArrAim = new double[6];
        double jointError[6];
        jointArr = axesControl->getJointsPos();
        jointArrAim = axesControl->getJointsPosAim();
        for(int i = 0; i < 6; i++)
        {
            jointError[i] = *(jointArrAim+i)-*(jointArr+i);
            txtoutPosNow << jointError[i]<<' ';
        }
        txtoutPosNow << endl;


        //实际位置
        //txtoutPosAim << axesControl->axisGetPosNow(0) << endl;
        double* posArr = new double[6];                                                                    //关节差值
        double* posArrAim = new double[6];
        double posError[6];
        posArr = axesControl->getNeedlePos();
        posArrAim = axesControl->getNeedlePosAim();
        for(int i = 0; i < 6; i++)
        {
            posError[i] = *(posArrAim+i)-*(posArr+i);
            txtoutPosAim << posError[i]<<' ';
        }
        txtoutPosAim << endl;

        //穿刺力
        txtoutPressure << analogReadNum << endl;             //0-1024,0-5V

        msleep(100);
    }
}
