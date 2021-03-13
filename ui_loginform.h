/********************************************************************************
** Form generated from reading UI file 'loginform.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINFORM_H
#define UI_LOGINFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_LoginForm
{
public:
    QLabel *label;
    QLabel *label_2;
    QLineEdit *userNameLEd;
    QLineEdit *pwdLEd;
    QPushButton *loginBtn;
    QPushButton *exitBtn;

    void setupUi(QDialog *LoginForm)
    {
        if (LoginForm->objectName().isEmpty())
            LoginForm->setObjectName(QStringLiteral("LoginForm"));
        LoginForm->resize(400, 300);
        label = new QLabel(LoginForm);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(110, 90, 54, 12));
        label_2 = new QLabel(LoginForm);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(110, 120, 54, 12));
        userNameLEd = new QLineEdit(LoginForm);
        userNameLEd->setObjectName(QStringLiteral("userNameLEd"));
        userNameLEd->setGeometry(QRect(160, 90, 113, 20));
        pwdLEd = new QLineEdit(LoginForm);
        pwdLEd->setObjectName(QStringLiteral("pwdLEd"));
        pwdLEd->setGeometry(QRect(160, 120, 113, 20));
        loginBtn = new QPushButton(LoginForm);
        loginBtn->setObjectName(QStringLiteral("loginBtn"));
        loginBtn->setGeometry(QRect(100, 180, 75, 23));
        exitBtn = new QPushButton(LoginForm);
        exitBtn->setObjectName(QStringLiteral("exitBtn"));
        exitBtn->setGeometry(QRect(200, 180, 75, 23));

        retranslateUi(LoginForm);

        QMetaObject::connectSlotsByName(LoginForm);
    } // setupUi

    void retranslateUi(QDialog *LoginForm)
    {
        LoginForm->setWindowTitle(QApplication::translate("LoginForm", "Dialog", nullptr));
        label->setText(QApplication::translate("LoginForm", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        label_2->setText(QApplication::translate("LoginForm", "\345\257\206  \347\240\201\357\274\232", nullptr));
        loginBtn->setText(QApplication::translate("LoginForm", "\347\231\273  \345\275\225", nullptr));
        exitBtn->setText(QApplication::translate("LoginForm", "\351\200\200  \345\207\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginForm: public Ui_LoginForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINFORM_H
