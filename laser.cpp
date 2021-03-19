#include "laser.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

Laser::Laser(QWidget *parent) : QWidget(parent)
{

}
void Laser::laserPortInit(QString laserPortNumber)
{
//    //port config
//    laserPort.setBaudRate(QSerialPort::Baud9600);
//    laserPort.setParity(QSerialPort::NoParity);
//    laserPort.setDataBits(QSerialPort::Data8);
//    laserPort.setStopBits(QSerialPort::OneStop);
//    laserPort.setPortName(laserPortNumber);

//    laserPort.open(QIODevice::ReadWrite);

//    if(laserPort.isOpen())
//    {
//        qDebug()<<laserPortNumber<<"is open.";
//    }
//    else
//    {
//        qDebug()<<laserPortNumber<<"connect error.";
//    }

}

void Laser::laserPortDisconnect(QString laserPortNumber)
{
//    laserPort.close();

//    if(!laserPort.isOpen())
//    {
//        qDebug()<<laserPortNumber<<"is closed.";

//    }
//    else
//    {
//        qDebug()<<laserPortNumber<<"disconnect error.";
//    }
}
