/********************************************************************************
** Form generated from reading UI file 'controlcenter.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLCENTER_H
#define UI_CONTROLCENTER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlCenter
{
public:
    QListWidget *listWidget;
    QPushButton *pb_reset;
    QLabel *label_image;
    QPushButton *pb_start;
    QPushButton *pb_stop;
    QPushButton *pb_imgSave;

    void setupUi(QWidget *ControlCenter)
    {
        if (ControlCenter->objectName().isEmpty())
            ControlCenter->setObjectName(QString::fromUtf8("ControlCenter"));
        ControlCenter->setWindowModality(Qt::NonModal);
        ControlCenter->resize(1300, 1000);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ControlCenter->sizePolicy().hasHeightForWidth());
        ControlCenter->setSizePolicy(sizePolicy);
        ControlCenter->setMinimumSize(QSize(1300, 1000));
        ControlCenter->setMaximumSize(QSize(1300, 1000));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu Serif"));
        ControlCenter->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8("cypress.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8("cypress.png"), QSize(), QIcon::Normal, QIcon::On);
        ControlCenter->setWindowIcon(icon);
        listWidget = new QListWidget(ControlCenter);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(10, 30, 301, 21));
        pb_reset = new QPushButton(ControlCenter);
        pb_reset->setObjectName(QString::fromUtf8("pb_reset"));
        pb_reset->setGeometry(QRect(740, 30, 121, 21));
        label_image = new QLabel(ControlCenter);
        label_image->setObjectName(QString::fromUtf8("label_image"));
        label_image->setGeometry(QRect(10, 60, 1281, 931));
        pb_start = new QPushButton(ControlCenter);
        pb_start->setObjectName(QString::fromUtf8("pb_start"));
        pb_start->setGeometry(QRect(320, 30, 131, 21));
        pb_start->setAutoRepeat(true);
        pb_stop = new QPushButton(ControlCenter);
        pb_stop->setObjectName(QString::fromUtf8("pb_stop"));
        pb_stop->setGeometry(QRect(460, 30, 131, 21));
        pb_stop->setAutoRepeat(true);
        pb_imgSave = new QPushButton(ControlCenter);
        pb_imgSave->setObjectName(QString::fromUtf8("pb_imgSave"));
        pb_imgSave->setGeometry(QRect(600, 30, 131, 21));
        pb_imgSave->setAutoRepeat(true);
        QWidget::setTabOrder(listWidget, pb_reset);

        retranslateUi(ControlCenter);

        QMetaObject::connectSlotsByName(ControlCenter);
    } // setupUi

    void retranslateUi(QWidget *ControlCenter)
    {
        ControlCenter->setWindowTitle(QApplication::translate("ControlCenter", "CyUSB Suite for Linux", 0, QApplication::UnicodeUTF8));
        pb_reset->setText(QApplication::translate("ControlCenter", "Reset Device", 0, QApplication::UnicodeUTF8));
        label_image->setText(QString());
        pb_start->setText(QApplication::translate("ControlCenter", "Video Start", 0, QApplication::UnicodeUTF8));
        pb_stop->setText(QApplication::translate("ControlCenter", "Video Stop", 0, QApplication::UnicodeUTF8));
        pb_imgSave->setText(QApplication::translate("ControlCenter", "Image Save", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ControlCenter: public Ui_ControlCenter {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLCENTER_H
