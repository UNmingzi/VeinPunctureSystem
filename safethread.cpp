#if _MSC_VER >= 1900
#pragma execution_character_set("utf-8")
#endif
#include "safethread.h"
bool safe_flag=true;

SafeThread::SafeThread(TrioPC *pcMatParent, MyAxesDriver *axesControlParent)
{
    pcMat=pcMatParent;                                                                      //传递对象指针
    axesControl = axesControlParent;
    Safe_isStop=false;
}

void SafeThread::closeSafeThread()
{
   Safe_isStop=true;
}

/*电机力矩保护*/
void SafeThread::run()
{
    qDebug()<<tr("开启保护线程");
    while(1)
    {
//         qDebug()<<Safe_isStop;
        if(Safe_isStop)
            return;
        //qDebug()<<tr("力矩保护中！！！");
        for(int i = 0; i < AXES_COUNT; i++)
        {
            if(qAbs(axesControl->axisGetTorque(i)) > TORQUE_MAX[i])                             //超过保护力矩
            {
                qDebug()<<tr("触发电流保护，轴：")<<i;
                pcMat->SetAxisVariable(tr("WDOG"),0, false);                                    //各轴失能
                safe_flag=false;
            }
        }
        msleep(50);
    }


}
