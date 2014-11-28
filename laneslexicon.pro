#-------------------------------------------------
#
#
#-------------------------------------------------
QT       += core gui printsupport
#QT       += webkitwidgets
QT       += sql
QT       += help
CONFIG   += debug
CONFIG   += libxslt
QMAKE_CXXFLAGS += -g

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = laneslexicon
TEMPLATE = app
! exists ("version.cpp") {
   system(cp dummy_version.cpp version.cpp)
}
libxslt {
! win32 {
  INCLUDEPATH += /usr/include/libxml2 /usr/local/include/libxml2 /usr/local/include
  LIBS += -L $$[QT_INSTALL_LIBS] -lxml2 -lxslt
}
win32 {
  INCLUDEPATH += c:/Users/andrewsg/xslt/include
  LIBS += -Lc:/Users/andrewsg/xslt/lib -llibxml2 -llibxslt
}
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
TRANSLATIONS = laneslexicon_de.ts laneslexicon_fr.ts
MOC_DIR = ./moc
OBJECTS_DIR = ./obj
QMAKE_CXXFLAGS += -Wunused-parameter
include(qslog/QsLog.pri)
include(imedit/imedit.pri)
include(keyboard/keyboard.pri)
include(options/options.pri)
include(notes/notes.pri)
SOURCES += main.cpp\
        eventtype.cpp \
        laneslexicon.cpp \
        contentswidget.cpp \
        bookmarkwidget.cpp \
        graphicsentry.cpp \
        entryitem.cpp \
        xsltsupport.cpp \
        headsearch.cpp \
        history.cpp \
        place.cpp \
        fullsearch.cpp \
        focustable.cpp \
        helpviewer.cpp \
        menubar.cpp \
        tabwidget.cpp \
        application.cpp \
        searchoptions.cpp \
        searchoptionswidget.cpp \
        searchdialogs.cpp \
        nodeview.cpp \
        splashscreen.cpp \
        about.cpp \
        entrylayoutwidget.cpp \
        logviewer.cpp \
        version.cpp \
        htmldelegate.cpp \
        historylist.cpp \
        nodeinfo.cpp

HEADERS  += laneslexicon.h \
            contentswidget.h \
            bookmarkwidget.h \
            graphicsentry.h \
            entryitem.h \
            xsltsupport.h \
            headsearch.h \
            history.h \
            place.h \
            fullsearch.h \
            focustable.h \
            helpviewer.h \
            menubar.h \
            tabwidget.h \
            namespace.h \
            application.h \
            searchoptions.h \
            searchoptionswidget.h \
            searchdialogs.h \
            nodeview.h \
            splashscreen.h \
            about.h \
            definedsettings.h \
            externs.h \
            entrylayoutwidget.h \
            logviewer.h \
            version.h \
            htmldelegate.h \
            historylist.h \
            nodeinfo.h
