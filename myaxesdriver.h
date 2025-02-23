﻿#ifndef MYAXESDRIVER_H
#define MYAXESDRIVER_H

//引用自己写的各种头文件
#include <QDebug>
#include <QString>
#include "triopc.h"
#include <QDebug>
#include <QtCore/qmath.h>
#include "myalgorithm.h"

#define AXES_COUNT 5                                //轴数

enum Axis_Num {AXIS_ZERO = 0, AXIS_ONE, AXIS_TWO, AXIS_THREE, AXIS_FOUR, AXIS_FIVE};            //轴号枚举
enum Axis_In_Out {AXIS_IN = 0, AXIS_OUT};                                                       //各轴控制单位标识符
extern double TORQUE_LIMIT[];
extern double TORQUE_MAX[];
extern double AXES_RETURN[];
extern double moveZeroPos[];
extern double POS_BASE[];
extern bool AXIS_TOZERO_FLAG[];
extern bool AXES_TOZERO_FLAG;

using namespace TrioPc;                             //添加TrioPC的命名空间

class MyAxesDriver : public QObject
{
public:
    MyAxesDriver(TrioPC *pcMatParent);

    void axisInit(int axisNum);                                          //单轴初始化
    void axesInit();                                                     //各轴初始化
    void axesOn();                                                       //伺服使能
    void axesOff();                                                      //伺服失能

    void axisInMoveRev(int axisNum, double uints);                      //相对运动, 只在寻零阶段用
    void axesInMoveRev(int axes, const double posArray[]);              //多轴同步相对运动，只在寻零阶段用
    void axisInMoveAbs(int axisNum, double uints);                      //绝对运动
    void axesInMoveAbs(int axes, double* posArray);                    //绝对运动
    void axisOutMoveAbs(int axisNum, double uints);                     // °(1到4)或mm(0和5)
    void axesOutMoveAbs(int axes, double* posArray);                   //绝对运动,输出轴 mm或 °
    void jointsOutMoveAbs(int joints, double* posArray);               //多关节绝对运动——输出关节
    void needleMoveAbs(double* posArray);                              // 针尖运动

    void jointsTrajectoryMove(int joints, double* posArray);           //多关节按规划轨迹运动
    void jointTraOutMoveAbs(int axisNum, double uints);                //单关节按规划轨迹运动
    void axisTraInMoveAbs(int axisNum, double uints);                   //单关节输入轴绝对运动
    double* traPlaning(unsigned int num);                                          //五次多项式插值

    bool isAxesContorlLink();                                           //多轴控制器连接判断
    double axisGetTorque(int axisNum);
    double axisGetPosNow(int axisNum);
    double axisGetPosAim(int axisNum);
    double analogRead(int channel);                                     //模拟量输入检测
    bool axisIsRuning(int axisNum);//单轴运动检测
    bool axesIsRuning();//多轴运动检测
    void axesGetZeroPos();//设置绝对零点
    double* getJointsPos();//获取当前关节实际位置
    double* getJointsPosAim(); //
    double* getNeedlePos();
    double* getNeedlePosAim();

private:
    TrioPC *pcMat;
};
#endif // MYAXESDRIVER_H
