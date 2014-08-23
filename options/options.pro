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
INCLUDEPATH += ..
SOURCES += main.cpp\
        mainwindow.cpp \
        optionswidget.cpp \
	optionsdialog.cpp \
	rootsoptions.cpp \
        printoptions.cpp

HEADERS  += mainwindow.h \
        optionswidget.h \
	optionsdialog.h \
	rootsoptions.h \
        printoptions.h
