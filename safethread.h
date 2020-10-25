#ifndef SAFETHREAD_H
#define SAFETHREAD_H

#include <triopc.h>
#include <QThread>
#include "myaxesdriver.h"

using namespace TrioPc;     //添加TrioPC的命名空间
extern bool safe_flag;

namespace Ui {
class SafeThread;                                   //线程
}

class SafeThread : public QThread
{
public:
    SafeThread(TrioPC *pcMatParent, MyAxesDriver *axesControlParent);
    void closeSafeThread();

protected:
    virtual void run();

private:
    TrioPC *pcMat;                                      //驱动器对象指针
    MyAxesDriver *axesControl;
    volatile bool Safe_isStop;       //isStop是易失性变量，需要用volatile进行申明
};
#endif // SAFETHREAD_H
