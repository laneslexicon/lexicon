#-------------------------------------------------
#
#
#-------------------------------------------------
QT       += core gui printsupport
#QT       += webkitwidgets network
QT       += sql

CONFIG   += debug
CONFIG   += libxslt
QMAKE_CXXFLAGS += -g

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = laneslexicon
TEMPLATE = app
libxslt {
INCLUDEPATH += /opt/include/libxml2
LIBS += -L /opt/lib -lxml2 -lxslt
DEFINES += USE_LIBXSLT
}
xalan {
INCLUDEPATH += /usr/include/xalanc/PlatformSupport
INCLUDEPATH += /usr/include/xalanc/XalanTransformer
LIBS += -lxalan-c -lxalanMsg -lxerces-c -lxerces-depdom
DEFINES += USE_XALAN
}
INCLUDEPATH += ./qslog

#LIBS +=   -lboost_thread-mt -lboost_system -lboost_filesystem

MOC_DIR = ./moc
OBJECTS_DIR = ./obj
QMAKE_CXXFLAGS += -Wunused-parameter
include(qslog/QsLog.pri)
include(imedit/imedit.pri)
SOURCES += main.cpp\
        eventtype.cpp \
        laneslexicon.cpp \
        contentswidget.cpp \
        bookmarkwidget.cpp \
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
            bookmarkwidget.h \
            graphicsentry.h \
            xsltsupport.h \
            searchwidget.h \
            noteswidget.h \
            history.h \
            place.h \
            searchresults.h \
            placewidget.h
