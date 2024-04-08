/********************************************************************************
** Form generated from reading UI file 'trimmingwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRIMMINGWINDOW_H
#define UI_TRIMMINGWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrimmingWindow
{
public:
    QLabel *labelImageToBeCropped;
    QPushButton *pushButtonApplyTrimming;
    QLabel *label;

    void setupUi(QWidget *TrimmingWindow)
    {
        if (TrimmingWindow->objectName().isEmpty())
            TrimmingWindow->setObjectName("TrimmingWindow");
        TrimmingWindow->resize(608, 406);
        labelImageToBeCropped = new QLabel(TrimmingWindow);
        labelImageToBeCropped->setObjectName("labelImageToBeCropped");
        labelImageToBeCropped->setGeometry(QRect(30, 20, 561, 301));
        labelImageToBeCropped->setAlignment(Qt::AlignCenter);
        pushButtonApplyTrimming = new QPushButton(TrimmingWindow);
        pushButtonApplyTrimming->setObjectName("pushButtonApplyTrimming");
        pushButtonApplyTrimming->setGeometry(QRect(410, 350, 161, 41));
        QFont font;
        font.setPointSize(11);
        pushButtonApplyTrimming->setFont(font);
        label = new QLabel(TrimmingWindow);
        label->setObjectName("label");
        label->setGeometry(QRect(70, 340, 281, 51));
        QFont font1;
        font1.setPointSize(10);
        label->setFont(font1);
        label->setFrameShape(QFrame::Panel);
        label->setFrameShadow(QFrame::Plain);
        label->setAlignment(Qt::AlignCenter);

        retranslateUi(TrimmingWindow);

        QMetaObject::connectSlotsByName(TrimmingWindow);
    } // setupUi

    void retranslateUi(QWidget *TrimmingWindow)
    {
        TrimmingWindow->setWindowTitle(QCoreApplication::translate("TrimmingWindow", "Form", nullptr));
        labelImageToBeCropped->setText(QString());
        pushButtonApplyTrimming->setText(QCoreApplication::translate("TrimmingWindow", "Apply Trimming", nullptr));
        label->setText(QCoreApplication::translate("TrimmingWindow", "<html><head/><body><p align=\"center\">\342\206\221Select the trimming area by drag-and-drop.</p><p align=\"center\" >(Note that the yellow border is a guide only.)</p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TrimmingWindow: public Ui_TrimmingWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRIMMINGWINDOW_H
