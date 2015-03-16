#-------------------------------------------------
#
# Project created by QtCreator 2013-05-05T10:34:31
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = options
TEMPLATE = app
#LIBS +=   -lboost_thread-mt -lboost_system -lboost_filesystem
MOC_DIR = ./moc
OBJECTS_DIR = ./obj
QMAKE_CXXFLAGS += -Wunused-parameter -g
include(../qslog/QsLog.pri)
INCLUDEPATH += ..
LIBS += -L $$[QT_INSTALL_LIBS]
DEFINES += STANDALONE
SOURCES += main.cpp\
        mainwindow.cpp \
        optionswidget.cpp \
	optionsdialog.cpp \
	rootsoptions.cpp \
        shortcutoptions.cpp \
        printoptions.cpp \
        diacriticsoptions.cpp \
        entryoptions.cpp \
        findoptions.cpp \
        popupoptions.cpp \
        bookmarkoptions.cpp \
        systemoptions.cpp \
        logoptions.cpp

HEADERS  += mainwindow.h \
        optionswidget.h \
	optionsdialog.h \
	rootsoptions.h \
        shortcutoptions.h \
        printoptions.h \
        diacriticsoptions.h \
        entryoptions.h \
        findoptions.h \
        popupoptions.h \
        bookmarkoptions.h \
        systemoptions.h \
        logoptions.h
