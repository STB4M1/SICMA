/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *pushButtonSelectImage;
    QLabel *labelImage;
    QLabel *labelImageSize;
    QPushButton *pushButtonGrayscale;
    QLabel *labelGrayscaleImage;
    QLabel *labelBinaryImage;
    QPushButton *pushButtonBinary;
    QPushButton *pushButtonTrim;
    QLabel *labelCroppedImage;
    QSlider *sliderThreshold;
    QFrame *line;
    QPushButton *pushButtonCalculateTension;
    QLabel *labelTensionValue;
    QLabel *labelTensionUnit;
    QLabel *labelGrayScaledImageSize;
    QLabel *labelBinarizedImageSize;
    QLabel *labelCroppedImageSize;
    QLineEdit *lineEditG;
    QLineEdit *lineEditDRho;
    QLineEdit *lineEditRhoDrop;
    QLineEdit *lineEditRhoAround;
    QPushButton *pushButtonOtsuBinarize;
    QLineEdit *lineEditPixelSize;
    QLabel *labelDe;
    QLabel *labelDs;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *labelDeUnit;
    QLabel *labelDsUnit;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *labelPixelSizeUnit;
    QLabel *labelDAroundUnit;
    QLabel *labelDDropUnit;
    QLabel *labelDRhoUnit;
    QLabel *labelGUnit;
    QLabel *labelG;
    QLabel *labelPixelSize;
    QLabel *labelDAround;
    QLabel *labelDDrop;
    QLabel *labelDRho;
    QLabel *labelDeDsDiagram;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1004, 680);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        pushButtonSelectImage = new QPushButton(centralwidget);
        pushButtonSelectImage->setObjectName("pushButtonSelectImage");
        pushButtonSelectImage->setGeometry(QRect(470, 20, 80, 31));
        labelImage = new QLabel(centralwidget);
        labelImage->setObjectName("labelImage");
        labelImage->setGeometry(QRect(20, 10, 301, 151));
        labelImage->setFrameShape(QFrame::Box);
        labelImage->setFrameShadow(QFrame::Raised);
        labelImage->setAlignment(Qt::AlignCenter);
        labelImageSize = new QLabel(centralwidget);
        labelImageSize->setObjectName("labelImageSize");
        labelImageSize->setGeometry(QRect(350, 30, 91, 20));
        labelImageSize->setFrameShape(QFrame::NoFrame);
        labelImageSize->setAlignment(Qt::AlignCenter);
        pushButtonGrayscale = new QPushButton(centralwidget);
        pushButtonGrayscale->setObjectName("pushButtonGrayscale");
        pushButtonGrayscale->setGeometry(QRect(470, 190, 80, 31));
        labelGrayscaleImage = new QLabel(centralwidget);
        labelGrayscaleImage->setObjectName("labelGrayscaleImage");
        labelGrayscaleImage->setGeometry(QRect(20, 180, 301, 141));
        labelGrayscaleImage->setFrameShape(QFrame::Box);
        labelGrayscaleImage->setFrameShadow(QFrame::Raised);
        labelGrayscaleImage->setAlignment(Qt::AlignCenter);
        labelBinaryImage = new QLabel(centralwidget);
        labelBinaryImage->setObjectName("labelBinaryImage");
        labelBinaryImage->setGeometry(QRect(20, 340, 301, 141));
        labelBinaryImage->setFrameShape(QFrame::Box);
        labelBinaryImage->setFrameShadow(QFrame::Raised);
        labelBinaryImage->setAlignment(Qt::AlignCenter);
        pushButtonBinary = new QPushButton(centralwidget);
        pushButtonBinary->setObjectName("pushButtonBinary");
        pushButtonBinary->setGeometry(QRect(530, 430, 80, 31));
        pushButtonTrim = new QPushButton(centralwidget);
        pushButtonTrim->setObjectName("pushButtonTrim");
        pushButtonTrim->setGeometry(QRect(430, 510, 141, 101));
        labelCroppedImage = new QLabel(centralwidget);
        labelCroppedImage->setObjectName("labelCroppedImage");
        labelCroppedImage->setGeometry(QRect(20, 500, 301, 131));
        labelCroppedImage->setFrameShape(QFrame::Box);
        labelCroppedImage->setFrameShadow(QFrame::Raised);
        labelCroppedImage->setAlignment(Qt::AlignCenter);
        sliderThreshold = new QSlider(centralwidget);
        sliderThreshold->setObjectName("sliderThreshold");
        sliderThreshold->setGeometry(QRect(329, 425, 191, 21));
        sliderThreshold->setMaximum(255);
        sliderThreshold->setSliderPosition(128);
        sliderThreshold->setOrientation(Qt::Horizontal);
        sliderThreshold->setInvertedAppearance(false);
        sliderThreshold->setInvertedControls(false);
        sliderThreshold->setTickPosition(QSlider::TicksBothSides);
        sliderThreshold->setTickInterval(16);
        line = new QFrame(centralwidget);
        line->setObjectName("line");
        line->setGeometry(QRect(610, 10, 20, 621));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        pushButtonCalculateTension = new QPushButton(centralwidget);
        pushButtonCalculateTension->setObjectName("pushButtonCalculateTension");
        pushButtonCalculateTension->setGeometry(QRect(890, 370, 91, 41));
        labelTensionValue = new QLabel(centralwidget);
        labelTensionValue->setObjectName("labelTensionValue");
        labelTensionValue->setGeometry(QRect(640, 360, 181, 51));
        QFont font;
        font.setPointSize(18);
        labelTensionValue->setFont(font);
        labelTensionValue->setFrameShape(QFrame::Box);
        labelTensionValue->setFrameShadow(QFrame::Raised);
        labelTensionValue->setLineWidth(3);
        labelTensionValue->setMidLineWidth(0);
        labelTensionValue->setAlignment(Qt::AlignCenter);
        labelTensionUnit = new QLabel(centralwidget);
        labelTensionUnit->setObjectName("labelTensionUnit");
        labelTensionUnit->setGeometry(QRect(830, 390, 49, 16));
        QFont font1;
        font1.setPointSize(12);
        labelTensionUnit->setFont(font1);
        labelTensionUnit->setScaledContents(false);
        labelTensionUnit->setAlignment(Qt::AlignCenter);
        labelGrayScaledImageSize = new QLabel(centralwidget);
        labelGrayScaledImageSize->setObjectName("labelGrayScaledImageSize");
        labelGrayScaledImageSize->setGeometry(QRect(350, 200, 91, 20));
        labelGrayScaledImageSize->setAlignment(Qt::AlignCenter);
        labelBinarizedImageSize = new QLabel(centralwidget);
        labelBinarizedImageSize->setObjectName("labelBinarizedImageSize");
        labelBinarizedImageSize->setGeometry(QRect(360, 350, 81, 20));
        labelBinarizedImageSize->setAlignment(Qt::AlignCenter);
        labelCroppedImageSize = new QLabel(centralwidget);
        labelCroppedImageSize->setObjectName("labelCroppedImageSize");
        labelCroppedImageSize->setGeometry(QRect(330, 530, 81, 20));
        labelCroppedImageSize->setAlignment(Qt::AlignCenter);
        lineEditG = new QLineEdit(centralwidget);
        lineEditG->setObjectName("lineEditG");
        lineEditG->setGeometry(QRect(760, 60, 121, 24));
        lineEditDRho = new QLineEdit(centralwidget);
        lineEditDRho->setObjectName("lineEditDRho");
        lineEditDRho->setGeometry(QRect(760, 180, 121, 24));
        lineEditRhoDrop = new QLineEdit(centralwidget);
        lineEditRhoDrop->setObjectName("lineEditRhoDrop");
        lineEditRhoDrop->setGeometry(QRect(760, 100, 121, 24));
        lineEditRhoAround = new QLineEdit(centralwidget);
        lineEditRhoAround->setObjectName("lineEditRhoAround");
        lineEditRhoAround->setGeometry(QRect(760, 140, 121, 24));
        pushButtonOtsuBinarize = new QPushButton(centralwidget);
        pushButtonOtsuBinarize->setObjectName("pushButtonOtsuBinarize");
        pushButtonOtsuBinarize->setGeometry(QRect(470, 350, 80, 31));
        lineEditPixelSize = new QLineEdit(centralwidget);
        lineEditPixelSize->setObjectName("lineEditPixelSize");
        lineEditPixelSize->setGeometry(QRect(760, 20, 121, 24));
        QFont font2;
        font2.setPointSize(9);
        lineEditPixelSize->setFont(font2);
        labelDe = new QLabel(centralwidget);
        labelDe->setObjectName("labelDe");
        labelDe->setGeometry(QRect(680, 270, 91, 31));
        labelDe->setFont(font1);
        labelDe->setFrameShape(QFrame::Box);
        labelDe->setFrameShadow(QFrame::Raised);
        labelDe->setLineWidth(2);
        labelDe->setAlignment(Qt::AlignCenter);
        labelDs = new QLabel(centralwidget);
        labelDs->setObjectName("labelDs");
        labelDs->setGeometry(QRect(840, 270, 91, 31));
        labelDs->setFont(font1);
        labelDs->setFrameShape(QFrame::Box);
        labelDs->setFrameShadow(QFrame::Raised);
        labelDs->setLineWidth(2);
        labelDs->setAlignment(Qt::AlignCenter);
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(700, 250, 49, 16));
        QFont font3;
        font3.setPointSize(11);
        label_4->setFont(font3);
        label_4->setAlignment(Qt::AlignCenter);
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(860, 250, 49, 16));
        label_5->setFont(font3);
        label_5->setAlignment(Qt::AlignCenter);
        labelDeUnit = new QLabel(centralwidget);
        labelDeUnit->setObjectName("labelDeUnit");
        labelDeUnit->setGeometry(QRect(780, 280, 49, 16));
        labelDeUnit->setFont(font3);
        labelDeUnit->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        labelDsUnit = new QLabel(centralwidget);
        labelDsUnit->setObjectName("labelDsUnit");
        labelDsUnit->setGeometry(QRect(940, 280, 31, 16));
        labelDsUnit->setFont(font3);
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(330, 400, 291, 21));
        QFont font4;
        font4.setFamilies({QString::fromUtf8("\346\270\270\346\230\216\346\234\235")});
        font4.setPointSize(10);
        label_6->setFont(font4);
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(640, 340, 181, 20));
        QFont font5;
        font5.setPointSize(10);
        label_7->setFont(font5);
        label_7->setAlignment(Qt::AlignCenter);
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(330, 450, 16, 16));
        label->setFont(font2);
        label->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(410, 450, 31, 16));
        label_2->setFont(font2);
        label_2->setAlignment(Qt::AlignCenter);
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(500, 450, 20, 16));
        label_3->setFont(font2);
        label_3->setAlignment(Qt::AlignCenter);
        labelPixelSizeUnit = new QLabel(centralwidget);
        labelPixelSizeUnit->setObjectName("labelPixelSizeUnit");
        labelPixelSizeUnit->setGeometry(QRect(890, 30, 18, 16));
        labelPixelSizeUnit->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        labelDAroundUnit = new QLabel(centralwidget);
        labelDAroundUnit->setObjectName("labelDAroundUnit");
        labelDAroundUnit->setGeometry(QRect(890, 150, 43, 16));
        labelDDropUnit = new QLabel(centralwidget);
        labelDDropUnit->setObjectName("labelDDropUnit");
        labelDDropUnit->setGeometry(QRect(890, 110, 43, 16));
        labelDRhoUnit = new QLabel(centralwidget);
        labelDRhoUnit->setObjectName("labelDRhoUnit");
        labelDRhoUnit->setGeometry(QRect(890, 190, 43, 16));
        labelDRhoUnit->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        labelGUnit = new QLabel(centralwidget);
        labelGUnit->setObjectName("labelGUnit");
        labelGUnit->setGeometry(QRect(890, 70, 35, 16));
        labelGUnit->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        labelG = new QLabel(centralwidget);
        labelG->setObjectName("labelG");
        labelG->setGeometry(QRect(720, 60, 22, 21));
        labelG->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelPixelSize = new QLabel(centralwidget);
        labelPixelSize->setObjectName("labelPixelSize");
        labelPixelSize->setGeometry(QRect(681, 21, 64, 21));
        labelPixelSize->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelDAround = new QLabel(centralwidget);
        labelDAround->setObjectName("labelDAround");
        labelDAround->setGeometry(QRect(670, 140, 71, 21));
        labelDAround->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelDDrop = new QLabel(centralwidget);
        labelDDrop->setObjectName("labelDDrop");
        labelDDrop->setGeometry(QRect(690, 100, 53, 21));
        labelDDrop->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelDRho = new QLabel(centralwidget);
        labelDRho->setObjectName("labelDRho");
        labelDRho->setGeometry(QRect(690, 180, 51, 21));
        labelDRho->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelDeDsDiagram = new QLabel(centralwidget);
        labelDeDsDiagram->setObjectName("labelDeDsDiagram");
        labelDeDsDiagram->setGeometry(QRect(650, 450, 321, 181));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1004, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButtonSelectImage->setText(QCoreApplication::translate("MainWindow", "Select Image", nullptr));
        labelImage->setText(QCoreApplication::translate("MainWindow", "Color Image", nullptr));
        labelImageSize->setText(QCoreApplication::translate("MainWindow", "Image size", nullptr));
        pushButtonGrayscale->setText(QCoreApplication::translate("MainWindow", "Glay Scale", nullptr));
        labelGrayscaleImage->setText(QCoreApplication::translate("MainWindow", "Glay Scale Image", nullptr));
        labelBinaryImage->setText(QCoreApplication::translate("MainWindow", "Binarized Image", nullptr));
        pushButtonBinary->setText(QCoreApplication::translate("MainWindow", "Binarize", nullptr));
        pushButtonTrim->setText(QCoreApplication::translate("MainWindow", "Trimming Start", nullptr));
        labelCroppedImage->setText(QCoreApplication::translate("MainWindow", "Cropped_Image", nullptr));
        pushButtonCalculateTension->setText(QCoreApplication::translate("MainWindow", "Cal. Tension", nullptr));
        labelTensionValue->setText(QCoreApplication::translate("MainWindow", "Value", nullptr));
        labelTensionUnit->setText(QCoreApplication::translate("MainWindow", "N/m", nullptr));
        labelGrayScaledImageSize->setText(QCoreApplication::translate("MainWindow", "Image size", nullptr));
        labelBinarizedImageSize->setText(QCoreApplication::translate("MainWindow", "Image size", nullptr));
        labelCroppedImageSize->setText(QCoreApplication::translate("MainWindow", "Image size", nullptr));
        pushButtonOtsuBinarize->setText(QCoreApplication::translate("MainWindow", "Otsu Binarize", nullptr));
        labelDe->setText(QString());
        labelDs->setText(QString());
        label_4->setText(QCoreApplication::translate("MainWindow", "de", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "ds", nullptr));
        labelDeUnit->setText(QCoreApplication::translate("MainWindow", "m", nullptr));
        labelDsUnit->setText(QCoreApplication::translate("MainWindow", "m", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "\342\200\273 You can also decide a threshold with the slider.", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Surface / Interfacial Tension", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "128", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "255", nullptr));
        labelPixelSizeUnit->setText(QCoreApplication::translate("MainWindow", "\316\274m", nullptr));
        labelDAroundUnit->setText(QCoreApplication::translate("MainWindow", "kg/m^3", nullptr));
        labelDDropUnit->setText(QCoreApplication::translate("MainWindow", "kg/m^3", nullptr));
        labelDRhoUnit->setText(QCoreApplication::translate("MainWindow", "kg/m^3", nullptr));
        labelGUnit->setText(QCoreApplication::translate("MainWindow", "m/s^2", nullptr));
        labelG->setText(QCoreApplication::translate("MainWindow", "g = ", nullptr));
        labelPixelSize->setText(QCoreApplication::translate("MainWindow", "Pixel Size = ", nullptr));
        labelDAround->setText(QCoreApplication::translate("MainWindow", "\317\201_around = ", nullptr));
        labelDDrop->setText(QCoreApplication::translate("MainWindow", "\317\201_drop = ", nullptr));
        labelDRho->setText(QCoreApplication::translate("MainWindow", "\316\224\317\201 = ", nullptr));
        labelDeDsDiagram->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
