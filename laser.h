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
    void laserPortInit(QString);   //���⴮�ڳ�ʼ���Լ�����
    void laserPortDisconnect(QString);  //���⴮�ڶϿ�����

signals:

public slots:
private:
    QSerialPort laserPort;  //���⴮�ڱ���
};

#endif // LASER_H
