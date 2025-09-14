QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

QT += network

# Ensure deterministic build output paths to avoid shadow-build confusion
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/build/debug
    OBJECTS_DIR = $$PWD/build/debug/obj
    MOC_DIR = $$PWD/build/debug/moc
    RCC_DIR = $$PWD/build/debug/rcc
    UI_DIR = $$PWD/build/debug/ui
}
CONFIG(release, debug|release) {
    DESTDIR = $$PWD/build/release
    OBJECTS_DIR = $$PWD/build/release/obj
    MOC_DIR = $$PWD/build/release/moc
    RCC_DIR = $$PWD/build/release/rcc
    UI_DIR = $$PWD/build/release/ui
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    widget.cpp \
    components/userbadgewidget.cpp \
    components/authdialog.cpp \
    components/herocardwidget.cpp \
    components/herodetailwidget.cpp \
    components/avatarwidget.cpp \
    components/radarchartwidget.cpp \
    components/typeindicatorwidget.cpp \
    components/parallelogrambar.cpp \
    network/httpclient.cpp

HEADERS += \
    widget.h \
    components/userbadgewidget.h \
    components/authdialog.h \
    components/herocardwidget.h \
    components/herodetailwidget.h \
    components/avatarwidget.h \
    components/radarchartwidget.h \
    components/typeindicatorwidget.h \
    components/parallelogrambar.h \
    network/httpclient.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
