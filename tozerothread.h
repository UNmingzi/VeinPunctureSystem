#ifndef TOZEROTHREAD_H
#define TOZEROTHREAD_H

#include <triopc.h>
#include <QThread>
#include "myaxesdriver.h"
extern bool safe_flag;
using namespace TrioPc;     //添加TrioPC的命名空间

namespace Ui {
class ToZeroThread;                                   //线程
}

class ToZeroThread : public QThread
{
public: 
    ToZeroThread(TrioPC *pcMatParent, MyAxesDriver *axesControlParent);
    void closeThread();

protected:
    virtual void run();

private:
    TrioPC *pcMat;                                      //驱动器对象指针
    MyAxesDriver *axesControl;
    volatile bool isStop;       //isStop是易失性变量，需要用volatile进行申明
};

#endif // TOZEROTHREAD_H
