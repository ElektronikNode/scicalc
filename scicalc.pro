#-------------------------------------------------
#
# Project created by QtCreator 2013-03-08T16:53:57
#
#-------------------------------------------------

QT       += core gui

#CONFIG   += console	# for debug output on windows

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scicalc
TEMPLATE = app


SOURCES += main.cpp\
        scicalc.cpp \
    variables.cpp \
    variable.cpp \
    scanner.cpp \
    print.cpp \
    parser.cpp \
    parseException.cpp \
    dialoggeneralsettings.cpp \
    scicalcedit.cpp \
    scicalcblock.cpp

HEADERS  += scicalc.h \
    variables.h \
    variable.h \
    token.h \
    scanner.h \
    print.h \
    parser.h \
    parseException.h \
    dialoggeneralsettings.h \
    scicalcedit.h \
    scicalcblock.h

FORMS    += \
    scicalc.ui \
    dialoggeneralsettings.ui

DISTFILES += \
    CHANGELOG.md \
    Grammatik.txt \
    README.md \
    constants.txt \
    functions.txt \
    test.sc \
    wunschliste.txt
