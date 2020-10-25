#if _MSC_VER >= 1900
#pragma execution_character_set("utf-8")
#endif
#include "flatui.h"
#include "qpushbutton.h"
#include "qlineedit.h"
#include "qprogressbar.h"
#include "qslider.h"
#include "qradiobutton.h"
#include "qcheckbox.h"
#include "qscrollbar.h"
#include "qdebug.h"
#include "qpainter.h"
#include "qevent.h"

FlatUI *FlatUI::self = 0;
FlatUI::FlatUI(QObject *parent) : QObject(parent)
{

}

void FlatUI::setPushButtonQss(QPushButton *btn, int radius, int padding,
                              const QString &normalColor, const QString &normalTextColor,
                              const QString &hoverColor, const QString &hoverTextColor,
                              const QString &pressedColor, const QString &pressedTextColor)
{
    QStringList qss;
    qss.append(QString("QPushButton{border-style:none;padding:%1px;border-radius:%2px;color:%3;background:%4;}")
               .arg(padding).arg(radius).arg(normalTextColor).arg(normalColor));
    qss.append(QString("QPushButton:hover{color:%1;background:%2;}")
               .arg(hoverTextColor).arg(hoverColor));
    qss.append(QString("QPushButton:pressed{color:%1;background:%2;}")
               .arg(pressedTextColor).arg(pressedColor));
    btn->setStyleSheet(qss.join(""));
}

void FlatUI::setLineEditQss(QLineEdit *txt, int radius, int borderWidth,
                            const QString &normalColor, const QString &focusColor)
{
    QStringList qss;
    qss.append(QString("QLineEdit{border-style:none;padding:3px;border-radius:%1px;border:%2px solid %3;}")
               .arg(radius).arg(borderWidth).arg(normalColor));
    qss.append(QString("QLineEdit:focus{border:%1px solid %2;}")
               .arg(borderWidth).arg(focusColor));
    txt->setStyleSheet(qss.join(""));
}

void FlatUI::setProgressBarQss(QProgressBar *bar, int barHeight, int barRadius, int fontSize,
                               const QString &normalColor, const QString &chunkColor)
{

    QStringList qss;
    qss.append(QString("QProgressBar{font:%1pt;background:%2;max-height:%3px;border-radius:%4px;text-align:center;border:1px solid %2;}")
               .arg(fontSize).arg(normalColor).arg(barHeight).arg(barRadius));
    qss.append(QString("QProgressBar:chunk{border-radius:%2px;background-color:%1;}")
               .arg(chunkColor).arg(barRadius));
    bar->setStyleSheet(qss.join(""));
}

void FlatUI::setSliderQss(QSlider *slider, int sliderHeight,
                          const QString &normalColor, const QString &grooveColor,
                          const QString &handleColor)
{
    int sliderRadius = sliderHeight / 2;
    int handleWidth = (sliderHeight * 3) / 2 + (sliderHeight / 5);
    int handleRadius = handleWidth / 2;
    int handleOffset = handleRadius / 2;

    QStringList qss;
    qss.append(QString("QSlider::groove:horizontal{background:%1;height:%2px;border-radius:%3px;}")
               .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    qss.append(QString("QSlider::add-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
               .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    qss.append(QString("QSlider::sub-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
               .arg(grooveColor).arg(sliderHeight).arg(sliderRadius));
    qss.append(QString("QSlider::handle:horizontal{width:%2px;margin-top:-%3px;margin-bottom:-%3px;border-radius:%4px;"
                       "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 #FFFFFF,stop:0.8 %1);}")
               .arg(handleColor).arg(handleWidth).arg(handleOffset).arg(handleRadius));

    //偏移一个像素
    handleWidth = handleWidth + 1;
    qss.append(QString("QSlider::groove:vertical{width:%2px;border-radius:%3px;background:%1;}")
               .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    qss.append(QString("QSlider::add-page:vertical{width:%2px;border-radius:%3px;background:%1;}")
               .arg(grooveColor).arg(sliderHeight).arg(sliderRadius));
    qss.append(QString("QSlider::sub-page:vertical{width:%2px;border-radius:%3px;background:%1;}")
               .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    qss.append(QString("QSlider::handle:vertical{height:%2px;margin-left:-%3px;margin-right:-%3px;border-radius:%4px;"
                       "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 #FFFFFF,stop:0.8 %1);}")
               .arg(handleColor).arg(handleWidth).arg(handleOffset).arg(handleRadius));

    slider->setStyleSheet(qss.join(""));
}

void FlatUI::setRadioButtonQss(QRadioButton *rbtn, int indicatorRadius,
                               const QString &normalColor, const QString &checkColor)
{
    int indicatorWidth = indicatorRadius * 2;

    QStringList qss;
    qss.append(QString("QRadioButton::indicator{border-radius:%1px;width:%2px;height:%2px;}")
               .arg(indicatorRadius).arg(indicatorWidth));
    qss.append(QString("QRadioButton::indicator::unchecked{background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,"
                       "stop:0.6 #FFFFFF,stop:0.7 %1);}").arg(normalColor));
    qss.append(QString("QRadioButton::indicator::checked{background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,"
                       "stop:0 %1,stop:0.3 %1,stop:0.4 #FFFFFF,stop:0.6 #FFFFFF,stop:0.7 %1);}").arg(checkColor));

    rbtn->setStyleSheet(qss.join(""));
}

void FlatUI::setScrollBarQss(QScrollBar *scroll, int radius, int min, int max,
                             const QString &bgColor, const QString &handleNormalColor,
                             const QString &handleHoverColor, const QString &handlePressedColor)
{
    //滚动条离背景间隔
    int padding = 0;

    QStringList qss;

    //handle:指示器,滚动条拉动部分 add-page:滚动条拉动时增加的部分 sub-page:滚动条拉动时减少的部分 add-line:递增按钮 sub-line:递减按钮

    //横向滚动条部分
    qss.append(QString("QScrollBar:horizontal{background:%1;padding:%2px;border-radius:%3px;max-height:%4px;}")
               .arg(bgColor).arg(padding).arg(radius).arg(max));
    qss.append(QString("QScrollBar::handle:horizontal{background:%1;min-width:%2px;border-radius:%3px;}")
               .arg(handleNormalColor).arg(min).arg(radius));
    qss.append(QString("QScrollBar::handle:horizontal:hover{background:%1;}")
               .arg(handleHoverColor));
    qss.append(QString("QScrollBar::handle:horizontal:pressed{background:%1;}")
               .arg(handlePressedColor));
    qss.append(QString("QScrollBar::add-page:horizontal{background:none;}"));
    qss.append(QString("QScrollBar::sub-page:horizontal{background:none;}"));
    qss.append(QString("QScrollBar::add-line:horizontal{background:none;}"));
    qss.append(QString("QScrollBar::sub-line:horizontal{background:none;}"));

    //纵向滚动条部分
    qss.append(QString("QScrollBar:vertical{background:%1;padding:%2px;border-radius:%3px;max-width:%4px;}")
               .arg(bgColor).arg(padding).arg(radius).arg(max));
    qss.append(QString("QScrollBar::handle:vertical{background:%1;min-height:%2px;border-radius:%3px;}")
               .arg(handleNormalColor).arg(min).arg(radius));
    qss.append(QString("QScrollBar::handle:vertical:hover{background:%1;}")
               .arg(handleHoverColor));
    qss.append(QString("QScrollBar::handle:vertical:pressed{background:%1;}")
               .arg(handlePressedColor));
    qss.append(QString("QScrollBar::add-page:vertical{background:none;}"));
    qss.append(QString("QScrollBar::sub-page:vertical{background:none;}"));
    qss.append(QString("QScrollBar::add-line:vertical{background:none;}"));
    qss.append(QString("QScrollBar::sub-line:vertical{background:none;}"));

    scroll->setStyleSheet(qss.join(""));
}

Painter_widget::Painter_widget(QWidget *parent) : QWidget(parent)
{
    borderOutColorStart = QColor(255, 255, 255);
    borderOutColorEnd = QColor(166, 166, 166);

    borderInColorStart = QColor(166, 166, 166);
    borderInColorEnd = QColor(255, 255, 255);

    bgColor = QColor(100, 184, 255);

    bgColor = QColor(100, 184, 255);
    overlayColor = QColor(255, 255, 255);
    showOverlay = true;

}


void Painter_widget::paintEvent(QPaintEvent *)
{
    int width = this->width();
    int height = this->height();
    int side = qMin(width, height);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.translate(width / 2, height / 2);
    painter.scale(side / 200.0, side / 200.0);
    //绘制外边框
    drawBorderOut(&painter);
    //绘制内边框
    drawBorderIn(&painter);
    //绘制内部指示颜色
    drawBg(&painter);
    //绘制遮罩层
    drawOverlay(&painter);
}
void Painter_widget::drawBorderOut(QPainter *painter)
{
    int radius = 99;
    painter->save();
    painter->setPen(Qt::NoPen);
    QLinearGradient borderGradient(0, -radius, 0, radius);
    borderGradient.setColorAt(0, borderOutColorStart);
    borderGradient.setColorAt(1, borderOutColorEnd);
    painter->setBrush(borderGradient);
    painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
    painter->restore();
}

void Painter_widget::drawBorderIn(QPainter *painter)
{
    int radius = 90;
    painter->save();
    painter->setPen(Qt::NoPen);
    QLinearGradient borderGradient(0, -radius, 0, radius);
    borderGradient.setColorAt(0, borderInColorStart);
    borderGradient.setColorAt(1, borderInColorEnd);
    painter->setBrush(borderGradient);
    painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
    painter->restore();
}

void Painter_widget::drawBg(QPainter *painter)
{
    int radius = 80;
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);
    painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
    painter->restore();
}

void Painter_widget::drawOverlay(QPainter *painter)
{

    int radius = 80;
    painter->save();
    painter->setPen(Qt::NoPen);

    QPainterPath smallCircle;
    QPainterPath bigCircle;
    radius -= 1;
    smallCircle.addEllipse(-radius, -radius, radius * 2, radius * 2);
    radius *= 2;
    bigCircle.addEllipse(-radius, -radius + 140, radius * 2, radius * 2);

    //高光的形状为小圆扣掉大圆的部分
    QPainterPath highlight = smallCircle - bigCircle;

    QLinearGradient linearGradient(0, -radius / 2, 0, 0);
    overlayColor.setAlpha(100);
    linearGradient.setColorAt(0.0, overlayColor);
    overlayColor.setAlpha(30);
    linearGradient.setColorAt(1.0, overlayColor);
    painter->setBrush(linearGradient);
    painter->rotate(-20);
    painter->drawPath(highlight);

    painter->restore();
}

QColor Painter_widget::getOverlayColor() const
{
    return this->overlayColor;
}

void Painter_widget::setShowOverlay(bool showOverlay)
{

        this->showOverlay = showOverlay;
        update();

}

void Painter_widget::setOverlayColor(const QColor &overlayColor)
{
    if (this->overlayColor != overlayColor) {
        this->overlayColor = overlayColor;
        update();
    }
}
void Painter_widget::setGreen()
{

    this->bgColor = QColor(0, 166, 0);
    update();
}

void Painter_widget::setRed()
{
    this->bgColor = QColor(166, 0, 0);
    update();
}
