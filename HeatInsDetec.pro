QT       += core gui serialport serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
include(./AIBUSProtocol/aibusProtocol.pri)
CONFIG += c++11
TARGET =grjc
TEMPLATE =app
RC_ICONS = ClOilClear.ico
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
TRANSLATIONS += Jap.ts \
                Chn.ts \
                Arab.ts
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    axistag.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    setting.cpp

HEADERS += \
    axistag.h \
    dataType.h \
    mainwindow.h \
    qcustomplot.h \
    setting.h

FORMS += \
    mainwindow.ui \
    setting.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc

DISTFILES +=

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../GiteeGithub/QtRptProject/bin/release/lib/ -lQtRPT
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../GiteeGithub/QtRptProject/bin/debug/lib/ -lQtRPT
else:unix: LIBS += -L$$PWD/../../../../GiteeGithub/QtRptProject/bin/debug/lib/ -lQtRPT

INCLUDEPATH += $$PWD/../../../../GiteeGithub/QtRptProject/QtRPT
DEPENDPATH += $$PWD/../../../../GiteeGithub/QtRptProject/QtRPT
