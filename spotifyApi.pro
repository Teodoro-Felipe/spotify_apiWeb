QT += core gui network networkauth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    Auth/authrequests.cpp \
    main.cpp \
    initauth.cpp \
    mainscreen.cpp

HEADERS += \
    Auth/authrequests.h \
    Auth/defines.h \
    initauth.h \
    mainscreen.h

FORMS += \
    initauth.ui \
    mainscreen.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
