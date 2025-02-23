﻿#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

namespace Ui {
class Login_form;
}

class Login_form : public QDialog
{
    Q_OBJECT

public:
    explicit Login_form(QWidget *parent = nullptr);
    ~Login_form();
public slots:
    void login();
private:
    Ui::Login_form *ui;
};

#endif // LOGIN_H
