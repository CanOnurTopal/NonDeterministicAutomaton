TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        nfacontainer.cpp \
        main.cpp \
        statemap.cpp

HEADERS += \
    nfacontainer.h \
    nfa.h \
    statemap.h \
    nfa_utilities.h \
    statedefinition.h
