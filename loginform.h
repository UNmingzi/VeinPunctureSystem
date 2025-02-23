﻿#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
class LoginForm;
}
class LoginForm : public QDialog
{
    Q_OBJECT //使用信号与槽需要的宏
public:
    explicit LoginForm(QDialog *parent = 0);  //explicit 防止歧义
    ~LoginForm();
signals:

public slots:
    void login();//点击登录按钮是执行的槽函数
    void close();
private:
    Ui::LoginForm *login_ui;
};


#endif // LOGINFORM_H
