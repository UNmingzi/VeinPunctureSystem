﻿#if _MSC_VER >= 1900
#pragma execution_character_set("utf-8")
#endif
#include "myaxesdriver.h"

/*各轴物理参数*/
static double UNITS_IN[6] = {8192, 8192, 8192, 24, 24, 24};                     //各轴电机一转脉冲数
static double UNITS_OUT[6] = {0.2, 0.2778, 0.2778, 0.3028, 0.3028, 1/0.7};          //输出轴单位：r/mm, r/°, r/°, r/°, r/°, r/mm(丝杠螺距由1变为0.7)   输入轴转数/输出轴参数
double moveZeroPos[6] = {-80, -60, -70, -90, -35, -21};                         //寻零路程
double AXES_RETURN[6] = {0.5, 0.5, 0.5, 1, 2, 1};                               //碰到零位后的返回值单位：输入轴转数
double POS_BASE[6] = {150, 30, 30, 0, 25, 5};                                   //各轴寻零后机械臂末端位姿
static double AXES_ZERO_POS[6] = {0, 0, 0, 0, 0, 0};                            //各轴零点偏差，归零后赋值
static double AXES_BASE_THEO_ERROR[6] = {0, 20, 60, -48, 0, 0};                 //各轴零点位置相对关节零点位置理论偏差
static double AXES_BASE_REAL_ERROR[6] = {0, 0, 0, 0, 0, 0};                     //各轴零点位置相对关节零点位置实际偏差

/*各轴控制参数*/
static double P_GAIN[6] = {1, 1, 1, 1, 1, 1};                                   //P
static double I_GAIN[6] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};                       //I
static double D_GAIN[6] = {0, 0, 0, 0, 0, 0};                                   //D
static double SPEED_ZERO[6] = {0.8, 0.7, 0.9, 1.8, 1.8, 1.3};                   //回零速度
static double SPEED_MOVE[6] = {0.8, 0.8, 0.8, 1.8, 1.8, 1.3};                   //正常运行速度
static double ACCEL[6] = {500, 500, 500, 1000, 500, 500};                       //加速度
static double DECEL[6] = {800, 800, 800, 1800, 1800, 1300};                     //减速度
static double CREEP[6] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1};                        //最小速度
static double JOGSPEED[6] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2};                     //步进速度
static double FE_LIMIT[6] = {100, 100, 100, 100, 100, 100};                     //偏差限制阈值
static double FE_RANGE[6] = {70, 70, 70, 70, 70, 70};                           //偏差提醒阈值
static double FHOLD_IN[6] = {-1, -1, -1, -1, -1, -1};                           //轴运动暂停,disable
static double DATUM_IN[6] = {-1, -1, -1, -1, -1, -1};                           //原点信号输入，disable

/*各轴限制参数*/
double TORQUE_LIMIT[6] = {1500, 1100, 800, 1200, 1000, 1100};                    //寻零力矩限制
double TORQUE_MAX[6] = {3000, 3000, 3000, 2500, 2500, 3000};                    //最大力矩限制
static double AXES_OUT_LIMIT[6] = {150, 55, 65, 85, 30, 20};                     //各轴绝对运动最大限位
static double JOINTS_LIMIT_MIN[6] = {3, 18, 60, -50, -15, 1};                     //各关节运动最小限位
static double JOINTS_LIMIT_MAX[6] = {75, 75, 125, 40, 20, 20};                  //各关节运动最大限位


/*各轴状态参数*/
static const int AXIS_DIRECTION[6] = {-1, 1, 1, 1, 1, -1};                      //各轴反向标志位,0轴和5轴反向
bool AXIS_TOZERO_FLAG[6] = {false, false, false, false, false, false};          //各轴寻零标志位
bool AXES_TOZERO_FLAG = false;                                                  //所有轴寻零完成



MyAxesDriver::MyAxesDriver(TrioPC *pcMatParent)
{
    pcMat = pcMatParent;
}

/*多轴控制器连接判断*/
bool MyAxesDriver::isAxesContorlLink()                                           //多轴控制器连接判断
{
    //通讯判断
    if(!pcMat->IsOpen(3240))
    {
        //建立通讯链接
        pcMat->SetHostAddress(tr("192.168.0.250"));
        pcMat->Open(2,3240);                                                //Port Type:2 Ethernet;PortMode:0/3240 Synchronous on specified port number
        if(pcMat->IsOpen(3240))
        {
            qDebug()<< "Date:" << QDate::currentDate()<<tr("建立通讯链接");
            return true;
        }
        else
        {
            qDebug()<< "Date:" << QDate::currentDate()<<tr("error:axisInit 无法建立通讯链接");
            return false;
        }
    }
    else
    {
        //qDebug()<< "Date:" << QDate::currentDate()<<tr("建立通讯链接");
        return true;
    }
}

/*单轴初始化*/
void MyAxesDriver::axisInit(int axisNum)
{
    if(!isAxesContorlLink())                                                                   //通讯判断
        return;
    pcMat->SetAxisVariable(tr("UNITS"), axisNum, UNITS_IN[axisNum]);
    pcMat->SetAxisVariable(tr("P_GAIN"), axisNum, P_GAIN[axisNum]);
    pcMat->SetAxisVariable(tr("I_GAIN"), axisNum, I_GAIN[axisNum]);
    pcMat->SetAxisVariable(tr("D_GAIN"), axisNum, D_GAIN[axisNum]);
    pcMat->SetAxisVariable(tr("SPEED"),axisNum, SPEED_ZERO[axisNum]);
    pcMat->SetAxisVariable(tr("ACCEL"),axisNum, ACCEL[axisNum]);
    pcMat->SetAxisVariable(tr("DECEL"),axisNum, DECEL[axisNum]);
    pcMat->SetAxisVariable(tr("CREEP"),axisNum, CREEP[axisNum]);
    pcMat->SetAxisVariable(tr("JOGSPEED"),axisNum, JOGSPEED[axisNum]);
    pcMat->SetAxisVariable(tr("FE_LIMIT"),axisNum, FE_LIMIT[axisNum]);
    pcMat->SetAxisVariable(tr("FE_RANGE"),axisNum, FE_RANGE[axisNum]);
    pcMat->SetAxisVariable(tr("FHOLD_IN"),axisNum, FHOLD_IN[axisNum]);
    pcMat->SetAxisVariable(tr("DATUM_IN"),axisNum, DATUM_IN[axisNum]);
    pcMat->SetAxisVariable(tr("SERVO"),axisNum, true);                      //伺服模式
}

/*轴系初始化*/
void MyAxesDriver::axesInit()
{
    for(int i = 0; i < AXES_COUNT; i++)
    {
        AXES_BASE_REAL_ERROR[i] = AXES_BASE_THEO_ERROR[i] + AXES_RETURN[i]/UNITS_OUT[i];           //根据寻零后返回值给关节偏差重新赋值
        axisInit(i);
    }
}

/*伺服使能*/
void MyAxesDriver::axesOn()
{
    if(!isAxesContorlLink())                                         //通讯判断
        return;
    for(int i = 0; i < AXES_COUNT; i++)
        pcMat->SetAxisVariable(tr("WDOG"),i, true);                  //驱动器使能
}

/*伺服失能*/
void MyAxesDriver::axesOff()
{
    if(!isAxesContorlLink())                                                                   //通讯判断
        return;
    for(int i = 0; i < AXES_COUNT; i++)
        pcMat->SetAxisVariable(tr("WDOG"),i, false);                  //驱动器使能
}

/*单轴相对运动—输入轴*/
void MyAxesDriver::axisInMoveRev(int axisNum, double uints)                       //输入轴，单位：圈
{
    pcMat->MoveRel(1, AXIS_DIRECTION[axisNum]*uints, axisNum);
    qDebug()<<axisNum<<tr("轴动");
    //注意：第一个参数代表控制几个轴
}

/*多轴相对运动—输入轴*/
void MyAxesDriver::axesInMoveRev(int axes, const double posArray[])
{
    for(int i = 0; i < axes; i++)
        axisInMoveRev(i, posArray[i]);
}

/*单轴绝对运动—输入轴*/
void MyAxesDriver::axisInMoveAbs(int axisNum, double uints)                        //绝对运动
{
    if(AXES_TOZERO_FLAG)
        pcMat->MoveAbs(1, AXIS_DIRECTION[axisNum]*(uints+AXES_ZERO_POS[axisNum]+AXIS_DIRECTION[axisNum]*AXES_RETURN[axisNum]), axisNum);         //加上零点位置的绝对运动(没有加上return)
    else
        qDebug()<<tr("警告：无法进行绝对运动，未归零");
}

/*多轴绝对运动—输入轴*/
void MyAxesDriver::axesInMoveAbs(int axes, double* posArray)                      //绝对运动
{
    for(int i = 0; i < axes; i++)
        axisInMoveAbs(i, *(posArray+i));
}

/*单轴绝对运动——输出轴*/
void MyAxesDriver::axisOutMoveAbs(int axisNum, double uints)                    // °(1到4)或mm(0和5)
{
    if(uints > AXES_OUT_LIMIT[axisNum])
        qDebug()<<tr("警告:超出软件限位")<<tr("轴")<<axisNum;
    else
        axisInMoveAbs(axisNum, uints*UNITS_OUT[axisNum]);
}

/*多轴绝对运动——输出轴*/
void MyAxesDriver::axesOutMoveAbs(int axes, double* posArray)                    // °(1到4)或mm(0和5)
{
    for(int i = 0; i < axes; i++)
        axisOutMoveAbs(i, *(posArray+i));
}

/*多关节绝对运动——输出关节*/
void MyAxesDriver::jointsOutMoveAbs(int joints, double* posArray)                    // °(1到4)或mm(0和5)
{
    for(int i = 0; i < joints; i++)
    {
        if(*(posArray+i) < JOINTS_LIMIT_MIN[i])
        {
            qDebug()<<"关节"<<i<<"超最小限位";
            return;
        }
        else if(*(posArray+i) > JOINTS_LIMIT_MAX[i])
        {
            qDebug()<<"关节"<<i<<"超最大限位";
            return;
        }
        else
            axisOutMoveAbs(i, *(posArray+i) - AXES_BASE_REAL_ERROR[i]);
    }
}

/*多关节绝对运动——输出关节*/
void MyAxesDriver::needleMoveAbs(double* posArray)                          // 针尖运动
{
    double* jointsPos=new double[6];
    jointsPos=MyAlgorithm::invKinematics(posArray);
    if (!axesIsRuning())
        jointsOutMoveAbs(AXES_COUNT,jointsPos);
     qDebug() <<"进入needle";

}

/*轴力矩检测*/
double MyAxesDriver::axisGetTorque(int axisNum)//力矩检测
{
    double axisTorque;
    pcMat->GetAxisVariable(tr("DRIVE_TORQUE"), axisNum, axisTorque);
    return axisTorque;
}

/*单轴实际位置检测*/
double MyAxesDriver::axisGetPosNow(int axisNum)
{
    double axisPosNow;
    pcMat->GetAxisVariable(tr("MPOS"), axisNum, axisPosNow);
    return axisPosNow;
}

/*模拟量输入检测*/
double MyAxesDriver::analogRead(int channel)
{
    double analogReadPara;
    pcMat->Ain(channel, analogReadPara);
    return analogReadPara;
}

/*单轴目标位置检测*/
double MyAxesDriver::axisGetPosAim(int axisNum)
{
    double axisPosAim;
    pcMat->GetAxisVariable(tr("DPOS"), axisNum, axisPosAim);
    return axisPosAim;
}


/*轴状态检测*/
bool MyAxesDriver::axisIsRuning(int axisNum)//单轴运动检测
{
    double axisRuningState;
    pcMat->GetAxisVariable(tr("IDLE"), axisNum, axisRuningState);
    return !bool(axisRuningState);
}

bool MyAxesDriver::axesIsRuning()//多轴运动检测
{
    bool axesRunningState = false;
    for(int i = 0; i < AXES_COUNT; i++)
        axesRunningState |= axisIsRuning(i);
    return axesRunningState;
}

void MyAxesDriver::axesGetZeroPos()//获取零点位置
{
    double axisPos;
    for(int i = 0; i < AXES_COUNT; i++)
    {
        pcMat->GetAxisVariable(tr("MPOS"), i, axisPos);
        AXES_ZERO_POS[i] = AXIS_DIRECTION[i]*axisPos;
    }
}


/*计算当前关节角*/
double* MyAxesDriver::getJointsPos()
{
  double* motorsPosArr = new double[6];
  double* jointsPos = new double[6];
  int i=0;
  for (i=0;i<5;i++)
  {
      pcMat->GetAxisVariable(tr("MPOS"), i, *(motorsPosArr+i));
      *(jointsPos+i)=(AXIS_DIRECTION[i]*(*(motorsPosArr+i))-AXES_ZERO_POS[i]-AXIS_DIRECTION[i]*AXES_RETURN[i])/UNITS_OUT[i]+AXES_BASE_REAL_ERROR[i];
  }

  *(jointsPos+5)=0;

  delete [] motorsPosArr;
  return jointsPos;
}

double* MyAxesDriver::getJointsPosAim()         //理论关节位置
{
  double* motorsPosArr=new double[6];
  double* jointsPos=new double[6];
  int i=0;
  for (i=0;i<6;i++)
  {
      pcMat->GetAxisVariable(tr("MPOS"), i, *(motorsPosArr+i));
      *(jointsPos+i)=(AXIS_DIRECTION[i]*(*(motorsPosArr+i))-AXES_ZERO_POS[i]-AXIS_DIRECTION[i]*AXES_RETURN[i])/UNITS_OUT[i]+AXES_BASE_REAL_ERROR[i];
  }
  delete [] motorsPosArr;
  return jointsPos;
}

double* MyAxesDriver::getNeedlePos()         //实际执行器位置
{
double* posArr=new double[6];
double* jointsPos=new double[6];
jointsPos=getJointsPos();
posArr=MyAlgorithm::forKinematics(jointsPos);
delete [] jointsPos;
return posArr;
}

double* MyAxesDriver::getNeedlePosAim()         //理论执行器位置
{
double* posArr=new double[6];
double* jointsPos=new double[6];
jointsPos=getJointsPosAim();
posArr=MyAlgorithm::forKinematics(jointsPos);
delete [] jointsPos;
return posArr;
}

 void  MyAxesDriver::jointsTrajectoryMove(int joints, double* posArray)
 {
     for(int i = 0; i < joints; i++)
     {
         if(*(posArray+i) < JOINTS_LIMIT_MIN[i])
         {
             qDebug()<<"关节"<<i<<"超最小限位";
             return;
         }
         else if(*(posArray+i) > JOINTS_LIMIT_MAX[i])
         {
             qDebug()<<"关节"<<i<<"超最大限位";
             return;
         }
         else
             jointTraOutMoveAbs(i, *(posArray+i) - AXES_BASE_REAL_ERROR[i]);
     }
 }

void  MyAxesDriver::jointTraOutMoveAbs(int axisNum, double uints)
{
    if(uints > AXES_OUT_LIMIT[axisNum])
        qDebug()<<tr("警告:超出软件限位")<<tr("轴")<<axisNum;
    else
        axisTraInMoveAbs(axisNum, uints*UNITS_OUT[axisNum]);
}

/*关节空间轨迹规划*/
void MyAxesDriver::axisTraInMoveAbs(int axisNum, double uints)     //规划点保存在控制器的table中
{
    double nowAxisPos, relDis;
    pcMat->GetAxisVariable(tr("MPOS"), axisNum, nowAxisPos);    //获取当前轴位置
    relDis = (AXIS_DIRECTION[axisNum]*(uints+AXES_ZERO_POS[axisNum]+AXIS_DIRECTION[axisNum]*AXES_RETURN[axisNum]) - nowAxisPos)*UNITS_IN[axisNum];    //相对位移

    if(AXES_TOZERO_FLAG)
        {  
        pcMat->Cam(0, 100, relDis, relDis/UNITS_IN[axisNum], axisNum);
        }
    else
        qDebug()<<tr("警告：无法进行绝对运动，未归零");
}

/*五次多项式插值（实际未使用）*/
double* MyAxesDriver::traPlaning(unsigned int num)
{
   double* points=new double[num];
   unsigned int i;
   for(i=0;i<num;i++)
   {
       *(points+i) = 6*qPow(i,5)/qPow((num-1),5) - 15*qPow(i,4)/qPow((num-1),4) + 10*qPow(i,3)/qPow((num-1),3);

   }
   return points;
}
