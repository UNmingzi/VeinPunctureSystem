﻿#ifndef FLATUI_H
#define FLATUI_H

/**
 * FlatUI辅助类 作者:feiyangqingyun(QQ:517216493) 2016-12-16
 * 1:按钮样式设置
 * 2:文本框样式设置
 * 3:进度条样式
 * 4:滑块条样式
 * 5:单选框样式
 * 6:滚动条样式
 * 7:可自由设置对象的高度宽度大小等
 * 8:自带默认参数值
 */

#include <QObject>
#include <QMutex>
#include <QWidget>

class QPushButton;
class QLineEdit;
class QProgressBar;
class QSlider;
class QRadioButton;
class QCheckBox;
class QScrollBar;


class FlatUI : public QObject
{
    Q_OBJECT
public:
    explicit FlatUI(QObject *parent = 0);
    static FlatUI *Instance()
    {
        static QMutex mutex;

        if (!self) {
            QMutexLocker locker(&mutex);

            if (!self) {
                self = new FlatUI;
            }
        }

        return self;
    }

private:
    static FlatUI *self;

public:
    //设置按钮样式
    void setPushButtonQss(QPushButton *btn,                             //按钮对象
                          int radius = 5,                               //圆角半径
                          int padding = 8,                              //间距
                          const QString &normalColor = "#34495E",       //正常颜色
                          const QString &normalTextColor = "#FFFFFF",   //文字颜色
                          const QString &hoverColor = "#4E6D8C",        //悬停颜色
                          const QString &hoverTextColor = "#F0F0F0",    //悬停文字颜色
                          const QString &pressedColor = "#2D3E50",      //按下颜色
                          const QString &pressedTextColor = "#B8C6D1"); //按下文字颜色

    //设置文本框样式
    void setLineEditQss(QLineEdit *txt,                                 //文本框对象
                        int radius = 3,                                 //圆角半径
                        int borderWidth = 2,                            //边框大小
                        const QString &normalColor = "#DCE4EC",         //正常颜色
                        const QString &focusColor = "#34495E");         //选中颜色

    //设置进度条样式
    void setProgressBarQss(QProgressBar *bar,
                           int barHeight = 8,                           //进度条高度
                           int barRadius = 5,                           //进度条半径
                           int fontSize = 9,                            //文字字号
                           const QString &normalColor = "#E8EDF2",      //正常颜色
                           const QString &chunkColor = "#E74C3C");      //进度颜色

    //设置滑块条样式
    void setSliderQss(QSlider *slider,                                  //滑动条对象
                      int sliderHeight = 8,                             //滑动条高度
                      const QString &normalColor = "#E8EDF2",           //正常颜色
                      const QString &grooveColor = "#1ABC9C",           //滑块颜色
                      const QString &handleColor = "#1ABC9C");          //指示器颜色

    //设置单选框样式
    void setRadioButtonQss(QRadioButton *rbtn,                          //单选框对象
                           int indicatorRadius = 8,                     //指示器圆角角度
                           const QString &normalColor = "#D7DBDE",      //正常颜色
                           const QString &checkColor = "#34495E");      //选中颜色

    //设置滚动条样式
    void setScrollBarQss(QScrollBar *scroll,                            //滚动条对象
                         int radius = 6,                                //圆角角度
                         int min = 120,                                 //指示器最小长度
                         int max = 12,                                  //滚动条最大长度
                         const QString &bgColor = "#606060",            //背景色
                         const QString &handleNormalColor = "#34495E",  //指示器正常颜色
                         const QString &handleHoverColor = "#1ABC9C",   //指示器悬停颜色
                         const QString &handlePressedColor = "#E74C3C");//指示器按下颜色


};

class Painter_widget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QColor overlayColor READ getOverlayColor WRITE setOverlayColor)
public:
    explicit Painter_widget(QWidget *parent = 0);
    QColor getOverlayColor()        const;


protected:
 void paintEvent(QPaintEvent *);
 void drawBorderOut(QPainter *painter);
 void drawBorderIn(QPainter *painter);
 void drawBg(QPainter *painter);

 void drawOverlay(QPainter *painter);

private:

    QColor borderOutColorStart;     //外边框渐变开始颜色
    QColor borderOutColorEnd;       //外边框渐变结束颜色
    QColor borderInColorStart;      //里边框渐变开始颜色
    QColor borderInColorEnd;        //里边框渐变结束颜色
    QColor bgColor;                 //背景颜色
    QColor overlayColor;            //遮罩层颜色

   bool showOverlay;               //是否显示遮罩层


public Q_SLOTS:



    //设置是否可移动
   // void setCanMove(bool canMove);
    //设置是否显示遮罩层
    void setShowOverlay(bool showOverlay);
    //设置遮罩层颜色
    void setOverlayColor(const QColor &overlayColor);

    //设置为绿色
    void setGreen();
    //设置为红色
    void setRed();

};

#endif // FLATUI_H
