#ifndef RECORDTHREAD_H
#define RECORDTHREAD_H
#include <QThread>
#include "myaxesdriver.h"
#include <QDebug>

class RecordThread : public QThread
{
public:
    RecordThread(TrioPC *pcMatParent, MyAxesDriver *axesControlParent);
    void closeThread();

protected:
    virtual void run();

private:
    TrioPC *pcMat;                                      //驱动器对象指针
    MyAxesDriver *axesControl;
    volatile bool isStop;       //isStop是易失性变量，需要用volatile进行申明
};

#endif // RECORDTHREAD_H
