QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CodecClass.cpp \
    headerview.cpp \
    main.cpp \
    mainwindow.cpp \
    modelrecord.cpp \
    rcnamespace.cpp \
    recorddata.cpp \
    treeitem.cpp \
    treeitemwidget.cpp \
    treemodel.cpp \
    treeview.cpp

HEADERS += \
    CodecClass.h \
    headerview.h \
    mainwindow.h \
    modelrecord.h \
    rcnamespace.h \
    recorddata.h \
    showmodelrecord.h \
    showrecorddata.h \
    stdafx.h \
    treeitem.h \
    treeitemwidget.h \
    treemodel.h \
    treeview.h \
    typedef.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
