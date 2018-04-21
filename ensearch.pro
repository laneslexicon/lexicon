#-------------------------------------------------
#
#
#-------------------------------------------------

QT       += core sql xml gui widgets printsupport xmlpatterns
CONFIG   += debug
#CONFIG   += libxslt
CONFIG   += gui
CONFIG   += xquery
TARGET = ensearch

TEMPLATE = app
QMAKE_CXXFLAGS += -pthread -Wunused-parameter -g
libxslt {
  DEFINES += USE_LIBXSLT
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

xquery {
DEFINES += USE_XQUERY
}
OBJECTS_DIR = ./ensearch-obj
MOC_DIR = ./ensearch-moc
INCLUDEPATH += ./qslog
include(qslog/QsLog.pri)

gui {
include(imedit/imedit.pri)
include(keyboard/keyboard.pri)
}
SOURCES += ensearch.cpp \
xsltsupport.cpp \
place.cpp \
textsearch.cpp \
lanesupport.cpp

HEADERS += textsearch.h \
place.h \
xsltsupport.h \
lanesupport.h
#
# following sources are for GUI
#
gui {
SOURCES += columnartablewidget.cpp \
columnselectdialog.cpp \
exportsearchdialog.cpp \
centeredcheckbox.cpp \
textsearchwidget.cpp \
ensearchwidget.cpp  \
nodeview.cpp

HEADERS += columnartablewidget.h \
columnselectdialog.h \
exportsearchdialog.h \
centeredcheckbox.h \
textsearchwidget.h \
ensearchwidget.h \
nodeview.h

}
