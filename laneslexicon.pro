#-------------------------------------------------
#
# This version requires webkitwidgets which is
# deprecated as of 5.6 (?). When its replacement
# is available on freeBSD the help system can be
# rewritten to use it.
#-------------------------------------------------
lessThan(QT_MAJOR_VERSION,5) {
error("This software requires Qt 5.0 or greater")
}
QT       += widgets
QT       += xml
QT       += sql
QT       += svg
QT       += core gui printsupport
QT       += xmlpatterns
#
#  Webkit for < 5.7
#  WebEngine > 5.7
#  Nothing = 5.7   for FreeBSD 5.7 webengine was not ported (I think)
#
! linux {
equals(QT_MAJOR_VERSION,5) {
lessThan(QT_MINOR_VERSION,7) {
QT    += webkitwidgets
DEFINES += HELP_WEBKIT
}
equals(QT_MINOR_VERSION,7) {
freebsd {
DEFINES += HELP_NONE
}
! freebsd {
DEFINES += HELP_WEBKIT
}
}
greaterThan(QT_MINOR_VERSION,7) {
QT += webenginewidgets
DEFINES += HELP_WEBENGINE
}
}
greaterThan(QT_MAJOR_VERSION,5): {
QT += webenginewidgets
DEFINES += HELP_WEBENGINE
}
}
linux {
DEFINES += HELP_NONE
}
CONFIG   += release
CONFIG   += libxslt
QMAKE_CXXFLAGS += -g

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
#
# Library and include paths LibXslt
#
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
#
#
#
#DEFINES += USE_LIBXSLT
DEFINES += USE_XQUERY
DEFINES += LANE
}
#
# Never completed using Xalan. LibXslt seemed a lot easier to incorporate
#
#
xalan {
INCLUDEPATH += /usr/include/xalanc/PlatformSupport
INCLUDEPATH += /usr/include/xalanc/XalanTransformer
LIBS += -lxalan-c -lxalanMsg -lxerces-c -lxerces-depdom
DEFINES += USE_XALAN
}
INCLUDEPATH += ./qslog
RESOURCES += laneslexicon.qrc
#TRANSLATIONS = laneslexicon_de.ts laneslexicon_fr.ts
MOC_DIR = ./moc
OBJECTS_DIR = ./obj
macx:ICON=$${PWD}/laneslexicon.icns
win32: RC_ICONS += $${PWD}/frontis.ico
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
        menubar.cpp \
        tabwidget.cpp \
        application.cpp \
        searchoptions.cpp \
#        searchoptionswidget.cpp \
        searchdialogs.cpp \
        nodeview.cpp \
        splashscreen.cpp \
        about.cpp \
        logviewer.cpp \
        version.cpp \
#       htmldelegate.cpp \
        historylist.cpp \
        nodeinfo.cpp \
        fontchangedialog.cpp \
        zoomdialog.cpp \
        editview.cpp \
        deletethemedialog.cpp \
        createthemedialog.cpp \
        themedialog.cpp \
        exportsearchdialog.cpp \
        helpview.cpp \
        linkcheckdialog.cpp \
        showxmldialog.cpp  \
        centeredcheckbox.cpp \
        savepagesetdialog.cpp \
        loadpagesetdialog.cpp \
        editpagesetdialog.cpp \
        pageentrydialog.cpp \
        columnselectdialog.cpp \
        columnartablewidget.cpp \
        tablistdialog.cpp \
        textwidthdialog.cpp \
        contentspanel.cpp \
        textsearchdialog.cpp \
        textsearch.cpp \
        ensearchwidget.cpp \
        textsearchwidget.cpp \
        lanesupport.cpp


HEADERS  += laneslexicon.h \
            contentswidget.h \
            bookmarkwidget.h \
            graphicsentry.h \
            entryitem.h \
            xsltsupport.h \
            headsearch.h \
            history.h \
            place.h \
            menubar.h \
            tabwidget.h \
            application.h \
            searchoptions.h \
#            searchoptionswidget.h \
            searchdialogs.h \
            nodeview.h \
            splashscreen.h \
            about.h \
            definedsettings.h \
            externs.h \
            logviewer.h \
            version.h \
#            htmldelegate.h \
            historylist.h \
            nodeinfo.h \
            fontchangedialog.h \
            zoomdialog.h \
            editview.h \
            deletethemedialog.h \
            createthemedialog.h \
            themedialog.h \
            definedsql.h \
            exportsearchdialog.h \
            helpview.h \
            acknowledgements.h  \
            linkcheckdialog.h \
            showxmldialog.h \
            centeredcheckbox.h \
            savepagesetdialog.h \
            loadpagesetdialog.h \
            editpagesetdialog.h \
            pageentrydialog.h \
            columnselectdialog.h \
            columnartablewidget.h \
            tablistdialog.h \
            textwidthdialog.h \
            contentspanel.h \
            textsearchdialog.h \
            textsearch.h \
            ensearchwidget.h \
            textsearchwidget.h \
            lanesupport.h
