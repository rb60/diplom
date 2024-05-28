QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DBTypes.cpp \
    addstudentwidget.cpp \
    addtaskwidget.cpp \
    averagestrategy.cpp \
    calculationparametrinfo.cpp \
    dbmanger.cpp \
    deviationstrategy.cpp \
    generatoreditor.cpp \
    generatorwizzard.cpp \
    importboolselector.cpp \
    importmarkswidget.cpp \
    importnumberselector.cpp \
    importparametr.cpp \
    importparametrlistselector.cpp \
    importpathselector.cpp \
    importstringselector.cpp \
    importstudentselector.cpp \
    main.cpp \
    mainwindow.cpp \
    markmatrix.cpp \
    parametrselector.cpp \
    parametrslistselector.cpp \
    removedialog.cpp \
    simpletransferdata.cpp \
    studentlistform.cpp \
    subjectmodel.cpp \
    swapstudentwidget.cpp

HEADERS += \
    DBTypes.h \
    TransferData.h \
    addstudentwidget.h \
    addtaskwidget.h \
    averagestrategy.h \
    calculationparametrinfo.h \
    calculationstrategy.h \
    dbmanger.h \
    deviationstrategy.h \
    exportTypes.h \
    generatoreditor.h \
    generatorwizzard.h \
    importParametrSelector.h \
    importTypes.h \
    importboolselector.h \
    importmarkswidget.h \
    importnumberselector.h \
    importparametr.h \
    importparametrlistselector.h \
    importpathselector.h \
    importstringselector.h \
    importstudentselector.h \
    mainwindow.h \
    markmatrix.h \
    parametrselector.h \
    parametrslistselector.h \
    removedialog.h \
    simpletransferdata.h \
    studentlistform.h \
    subjectmodel.h \
    swapstudentwidget.h

FORMS += \
    addstudentwidget.ui \
    addtaskwidget.ui \
    generatoreditor.ui \
    generatorwizzard.ui \
    importboolselector.ui \
    importmarkswidget.ui \
    importnumberselector.ui \
    importparametrlistselector.ui \
    importpathselector.ui \
    importstringselector.ui \
    importstudentselector.ui \
    mainwindow.ui \
    parametrselector.ui \
    parametrslistselector.ui \
    removedialog.ui \
    studentlistform.ui \
    swapstudentwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    sqls.qrc
