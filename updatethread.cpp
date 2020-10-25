#include "updatethread.h"

updateThread::updateThread(TrioPC *pcMatParent, MyAxesDriver *axesControlParent)
{
    pcMat=pcMatParent;                                                                      //传递对象指针
    axesControl = axesControlParent;
    Update_isStop=false;
}

void updateThread::closeUpdateThread()
{
   Update_isStop=true;
}

void updateThread::run()
{

    qDebug()<<tr("开启更新线程");

}


double* updateThread::nowPosition()
{
    double* posArr=new double[6];
    double* jointsPos=new double[6];
    jointsPos=axesControl->getJointsPos();
    posArr=MyAlgorithm::forKinematics(jointsPos);
    delete [] jointsPos;
    return posArr;

}
