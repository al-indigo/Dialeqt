QT       += core gui sql webkitwidgets webkit multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Dialeqt
TEMPLATE = app


SOURCES += main.cpp\
        dialeqt.cpp \
    dialogcreatenewdictionary.cpp \
    dictglobalattributes.cpp \
    dicttabscontainer.cpp \
    dictdbfactory.cpp \
    tabcontents.cpp \
    dictentry.cpp \
    customquerydiagnostics.cpp \
    legendwindow.cpp \
    phonologywindow.cpp \
    taleswindow.cpp \
    paradigmwindow.cpp \
    etimologywindow.cpp \
    playwindow.cpp \
    editablesqlmodel.cpp

HEADERS  += dialeqt.h \
    dialogcreatenewdictionary.h \
    dictglobalattributes.h \
    dicttabscontainer.h \
    dictdbfactory.h \
    customqhash.h \
    tabcontents.h \
    dictentry.h \
    customquerydiagnostics.h \
    legendwindow.h \
    phonologywindow.h \
    taleswindow.h \
    paradigmwindow.h \
    etimologywindow.h \
    playwindow.h \
    editablesqlmodel.h

FORMS    += dialeqt.ui \
    dialogcreatenewdictionary.ui \
    tabcontents.ui \
    legendwindow.ui \
    phonologywindow.ui \
    taleswindow.ui \
    paradigmwindow.ui \
    etimologywindow.ui \
    playwindow.ui
