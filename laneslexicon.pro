#-------------------------------------------------
#
#
#-------------------------------------------------
QT       += core gui printsupport
QT       += webkitwidgets network
QT       += xml sql

CONFIG   += debug
QMAKE_CXXFLAGS += -g

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = laneslexicon
TEMPLATE = app

INCLUDEPATH += /usr/include/xalanc/PlatformSupport
INCLUDEPATH += /usr/include/xalanc/XalanTransformer
INCLUDEPATH += ./qslog
DEFINES += USE_XALAN
#LIBS +=   -lboost_thread-mt -lboost_system -lboost_filesystem
LIBS += -lxalan-c -lxalanMsg -lxerces-c -lxerces-depdom
MOC_DIR = ./moc
OBJECTS_DIR = ./obj
QMAKE_CXXFLAGS += -Wunused-parameter
include(qslog/QsLog.pri)
include(imedit/imedit.pri)
SOURCES += main.cpp\
        eventtype.cpp \
        laneslexicon.cpp \
        contentswidget.cpp \
        graphicsentry.cpp \
        xsltsupport.cpp \
        searchwidget.cpp \
        noteswidget.cpp \
        history.cpp \
        place.cpp \
        searchresults.cpp \
        placewidget.cpp


HEADERS  += laneslexicon.h \
            contentswidget.h \
            graphicsentry.h \
            xsltsupport.h \
            searchwidget.h \
            noteswidget.h \
            history.h \
            place.h \
            searchresults.h \
            placewidget.h
