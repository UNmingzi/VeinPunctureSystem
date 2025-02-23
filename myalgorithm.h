﻿#ifndef MYALGORITHM_H
#define MYALGORITHM_H

#include <QObject>
#include <qmath.h>


class MyAlgorithm : public QObject
{
    Q_OBJECT
public:
    explicit MyAlgorithm(QObject *parent = nullptr);

    static double* forKinematics(double* jointsArray);            //正运动学
    static double* invKinematics(double* posArray);            //逆运动学
signals:

public slots:
};

#endif // MYALGORITHM_H
