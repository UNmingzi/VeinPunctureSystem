﻿#if _MSC_VER >= 1900
#pragma execution_character_set("utf-8")
#endif
#include "myalgorithm.h"
#include <QDebug>

static double L1 = 110;
static double L2 = 110;


MyAlgorithm::MyAlgorithm(QObject *parent) : QObject(parent)
{
}


double* MyAlgorithm::forKinematics(double* jointsArray)            //正运动学        关节是角度
{
    double* pos_Array=new double[6];
    *(jointsArray+1) = *(jointsArray+1)*M_PI/180;                                       //将角度转化为弧度
    *(jointsArray+2) = *(jointsArray+2)*M_PI/180;                                       //将角度转化为弧度
    /*进行计算*/
    double L3 = qSqrt(qPow(L1, 2) + qPow(L2, 2) - 2*L1*L2*qCos(*(jointsArray+2)));
    *(pos_Array) = L3*qCos(*(jointsArray+2)/2 - *(jointsArray+1));                       //x
    *(pos_Array+1) = *jointsArray;                                                       //y
    *(pos_Array+2) = L3*qSin(*(jointsArray+2)/2 - *(jointsArray+1));                     //z
    *(pos_Array+3) = -*(jointsArray+3);                                                  //yaw
    *(pos_Array+4) = *(jointsArray+4);                                                   //pitch
    *(pos_Array+5) = *(jointsArray+5);                                                     //L


    *(jointsArray+1) = *(jointsArray+1)*180/M_PI;                                       //将弧度转化为角度
    *(jointsArray+2) = *(jointsArray+2)*180/M_PI;                                       //将弧度转化为角度
    return pos_Array;

}

double* MyAlgorithm::invKinematics(double* posArray)            //逆运动学
{
    double* joints_Array=new double[6];
    *(joints_Array) = *(posArray+1);
    *(joints_Array+2) = qAcos((qPow(L1, 2)+qPow(L2, 2)-(qPow(*(posArray), 2)+qPow(*(posArray+2), 2)))/(2*L1*L2));
    *(joints_Array+1) = *(joints_Array+2)/2-qAtan(*(posArray+2)/(*(posArray)));
    *(joints_Array+3) = -*(posArray+3);
    *(joints_Array+4) = *(posArray+4);
    *(joints_Array+5) = *(posArray+5);

    *(joints_Array+1) = *(joints_Array+1)*180/M_PI;                                       //将弧度转化为角度
    *(joints_Array+2) = *(joints_Array+2)*180/M_PI;                                       //将弧度转化为角度
    return joints_Array;
}
