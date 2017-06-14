#-------------------------------------------------
#
#
#-------------------------------------------------

QT       += core sql xml gui widgets
CONFIG   += debug
CONFIG   += libxslt
TARGET = ensearch
TEMPLATE = app
QMAKE_CXXFLAGS += -pthread -Wunused-parameter -g
DEFINES += USE_LIBXSLT
libxslt {
! win32 {
  INCLUDEPATH += /usr/include/libxml2 /usr/local/include/libxml2 /usr/local/include
  LIBS += -L $$[QT_INSTALL_LIBS] -lxml2 -lxslt
}
win32 {
INCLUDEPATH += "libxml2-2.7.8.win32\include"
INCLUDEPATH += "libxslt-1.1.26.win32\include"
LIBS += -L"$$PWD\libxml2-2.7.8.win32\lib" -llibxml2
LIBS += -L"$$PWD\libxslt-1.1.26.win32\lib" -llibxslt
}
}
OBJECTS_DIR = ./obj
MOC_DIR = ./moc
INCLUDEPATH += ./qslog
include(qslog/QsLog.pri)
#include(imedit/imedit.pri)
#include(keyboard/keyboard.pri)

SOURCES += ensearch.cpp \
xsltsupport.cpp \
place.cpp \
           textsearch.cpp

#textsearchwidget.cpp \
#searchoptionswidget.cpp \
#searchoptions.cpp \
#columnselectdialog.cpp \
#columnartablewidget.cpp \
#centeredcheckbox.cpp \
#nodeview.cpp \

HEADERS += textsearch.h \
place.h \
xsltsupport.h
