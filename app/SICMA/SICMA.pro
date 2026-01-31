QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    otsubinarizer.cpp \
    overlaywidget.cpp \
    tensioncalculator.cpp \
    trimmingwindow.cpp

HEADERS += \
    mainwindow.h \
    otsubinarizer.h \
    overlaywidget.h \
    tensioncalculator.h \
    trimmingwindow.h

FORMS += \
    mainwindow.ui \
    trimmingwindow.ui

# TRANSLATIONS += \
#     SICMA_ja_JP.ts
# CONFIG += lrelease
# CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    appicon.rc

RESOURCES += \
    resources.qrc

RC_FILE += appicon.rc

