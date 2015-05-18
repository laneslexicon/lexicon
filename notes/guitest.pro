#-------------------------------------------------
#
# Project created by QtCreator 2013-05-05T10:34:31
#
#-------------------------------------------------

QT       += core gui printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testgui
TEMPLATE = app
#LIBS +=   -lboost_thread-mt -lboost_system -lboost_filesystem
MOC_DIR = ./moc
OBJECTS_DIR = ./obj
QMAKE_CXXFLAGS += -Wunused-parameter
include(../qslog/QsLog.pri)
include(../imedit/imedit.pri)
include(../keyboard/keyboard.pri)

INCLUDEPATH += ..

libxslt {
! win32 {
  INCLUDEPATH += /usr/include/libxml2 /usr/local/include/libxml2
  INCLUDEPATH += /usr/local/include
  LIBS += -lxml2 -lxslt
}
win32 {
  INCLUDEPATH += c:/Users/andrewsg/xslt/include
  LIBS += -Lc:/Users/andrewsg/xslt/lib -llibxml2 -llibxslt
}
}

INCLUDEPATH += /usr/include/libxml2 /usr/local/include/libxml2
INCLUDEPATH += /usr/local/include
LIBS += -L $$[QT_INSTALL_LIBS]  -lxml2 -lxslt
DEFINES += USE_LIBXSLT
#DEFINES += QT_NO_CAST_FROM_ASCII
DEFINES += TEST_FRAMEWORK
SOURCES += main.cpp\
        eventtype.cpp \
        notes.cpp \
        notebrowser.cpp \
        notedialog.cpp \
        mainwindow.cpp \
        ..\place.cpp \
        ..\xsltsupport.cpp \
        noteview.cpp

HEADERS  += mainwindow.h \
        notes.h \
        notebrowser.h \
        notedialog.h \
        ..\xsltsupport.h \
        noteview.h
