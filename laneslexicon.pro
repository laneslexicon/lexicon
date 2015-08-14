#-------------------------------------------------
#
#
#-------------------------------------------------
QT       += core gui printsupport
QT       += webkitwidgets
QT       += sql
#QT       += help
QT       += svg
CONFIG   += release
CONFIG   += libxslt
QMAKE_CXXFLAGS += -g

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = laneslexicon
TEMPLATE = app
! exists ("version.cpp") {
  ! win32 {
      system(cp dummy_version.cpp version.cpp)
   }
    win32 {
    system(copy dummy_version.cpp version.cpp)
   }
}
libxslt {
! win32 {
  INCLUDEPATH += /usr/include/libxml2 /usr/local/include/libxml2 /usr/local/include
  LIBS += -L $$[QT_INSTALL_LIBS] -lxml2 -lxslt
}
win32 {
INCLUDEPATH += "C:\Users\andrewsg\My Documents\libxml2-2.7.8.win32\include"
INCLUDEPATH += "C:\Users\andrewsg\My Documents\libxslt-1.1.26.win32\include"
LIBS += -L"C:\Users\andrewsg\My Documents\libxml2-2.7.8.win32\lib" -llibxml2
LIBS += -L"C:\Users\andrewsg\My Documents\libxslt-1.1.26.win32\lib" -llibxslt
  INCLUDEPATH += c:/Users/andrewsg/xslt/include
  LIBS += -Lc:/Users/andrewsg/xslt/lib -llibxml2 -llibxslt
}
DEFINES += USE_LIBXSLT
DEFINES += LANE
}
xalan {
INCLUDEPATH += /usr/include/xalanc/PlatformSupport
INCLUDEPATH += /usr/include/xalanc/XalanTransformer
LIBS += -lxalan-c -lxalanMsg -lxerces-c -lxerces-depdom
DEFINES += USE_XALAN
}
INCLUDEPATH += ./qslog
RESOURCES += laneslexicon.qrc
#LIBS +=   -lboost_thread-mt -lboost_system -lboost_filesystem
TRANSLATIONS = laneslexicon_de.ts laneslexicon_fr.ts
MOC_DIR = ./moc
OBJECTS_DIR = ./obj
macx:ICON=$${PWD}/laneslexicon.icns
win32: RC_FILE = $${PWD}/laneslexicon.rc
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
 #       helpviewer.cpp \
        menubar.cpp \
        tabwidget.cpp \
        application.cpp \
        searchoptions.cpp \
        searchoptionswidget.cpp \
        searchdialogs.cpp \
        nodeview.cpp \
        splashscreen.cpp \
        about.cpp \
        logviewer.cpp \
        version.cpp \
        htmldelegate.cpp \
        historylist.cpp \
        nodeinfo.cpp \
        fontchangedialog.cpp \
#        arabicfontdialog.cpp \
        zoomdialog.cpp \
        editview.cpp \
        deletethemedialog.cpp \
        createthemedialog.cpp \
        themedialog.cpp \
        exportsearchdialog.cpp \
        helpview.cpp


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
#            helpviewer.h \
            menubar.h \
            tabwidget.h \
            application.h \
            searchoptions.h \
            searchoptionswidget.h \
            searchdialogs.h \
            nodeview.h \
            splashscreen.h \
            about.h \
            definedsettings.h \
            externs.h \
            logviewer.h \
            version.h \
            htmldelegate.h \
            historylist.h \
            nodeinfo.h \
            fontchangedialog.h \
#            arabicfontdialog.h \
            zoomdialog.h \
            editview.h \
            deletethemedialog.h \
            createthemedialog.h \
            themedialog.h \
            definedsql.h \
            exportsearchdialog.h \
            helpview.h \
            acknowledgements.h
