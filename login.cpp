﻿#include "login.h"
#include "ui_login.h"

Login_form::Login_form(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login_form)
{
    ui->setupUi(this);
}

Login_form::~Login_form()
{
    delete ui;
}
