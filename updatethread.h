#ifndef UPDATETHREAD_H
#define UPDATETHREAD_H
#include <triopc.h>
#include <QThread>
#include "myaxesdriver.h"
#include "myalgorithm.h"

class updateThread : public QThread
{
public:
    updateThread(TrioPC *pcMatParent, MyAxesDriver *axesControlParent);
    void closeUpdateThread();
    double* nowPosition();

protected:
    virtual void run();

private:
    TrioPC *pcMat;                                      //驱动器对象指针
    MyAxesDriver *axesControl;
    volatile bool Update_isStop;       //isStop是易失性变量，需要用volatile进行申明
};

#endif // UPDATETHREAD_H
