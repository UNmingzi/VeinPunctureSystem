#if _MSC_VER >= 1900
#pragma execution_character_set("utf-8")
#endif
#include "tozerothread.h"
#include "myaxesdriver.h"

ToZeroThread::ToZeroThread(TrioPC *pcMatParent, MyAxesDriver *axesControlParent)                                     //驱动器对象指针
{
    pcMat=pcMatParent;                                                                      //传递对象指针
    axesControl = axesControlParent;
}

void ToZeroThread::closeThread()            //关闭线程
{

}

/*寻零*/
void ToZeroThread::run()                    //线程运行函数
{
    int axesZeroCount = 0;
    AXES_TOZERO_FLAG = false;
    axesControl->axesInMoveRev(6, moveZeroPos); //多轴控制
//    qDebug()<<tr("kaiqi");

    while (safe_flag)
    {
        /*线程循环函数*/
        for(int i = 0; i < AXES_COUNT; i++)
        {
            if(qAbs(axesControl->axisGetTorque(i)) > TORQUE_LIMIT[i])
            {
                if(!AXIS_TOZERO_FLAG[i])//该轴未寻零则触发寻零函数，否则直接进行力矩检测
                {
                    qDebug()<<tr("运动停止，轴：")<<i;                                               //多线程停止轴
                    pcMat->Cancel(0, i);
                    pcMat->Cancel(1, i);

                    axesControl->axisInMoveRev(i, AXES_RETURN[i]);                                  //相对运动
                    AXIS_TOZERO_FLAG[i] = true;
                    axesZeroCount++;
                    if(axesZeroCount == 5)//所有轴都回零位    修改
                    {
                        qDebug()<<tr("注意：所有轴已到零位");
                        while(axesControl->axesIsRuning());                                         //等待所有轴运动停止
                        axesControl->axesGetZeroPos();                                              //获取零点位置
                        AXES_TOZERO_FLAG = true;
                        axesZeroCount = 0;
                       //axesControl->needleMoveAbs(POS_BASE);                                       //走到寻零后的初始位置
                        return;                                                                     //退出寻零线程
                    }
                }
            }
        }
        msleep(50);
    }
}
