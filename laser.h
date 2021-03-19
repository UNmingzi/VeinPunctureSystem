#ifndef LASER_H
#define LASER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QSerialPort>
#include <QDebug>


class Laser : public QWidget
{
    Q_OBJECT
public:
    explicit Laser(QWidget *parent = nullptr);
    void laserPortInit(QString);   //激光串口初始化以及连接
    void laserPortDisconnect(QString);  //激光串口断开连接

signals:

public slots:
private:
    QSerialPort laserPort;  //激光串口变量
};

#endif // LASER_H
