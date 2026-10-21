QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ArbolBClientes.cpp \
    ArbolFunciones.cpp \
    ArbolMerkle.cpp \
    Blockchain.cpp \
    GrafoSedes.cpp \
    LZW.cpp \
    TablaHashReservas.cpp \
    arbolpeliculas.cpp \
    listasolicitudes.cpp \
    main.cpp \
    mainwindow.cpp \
    matrizdispersa.cpp \
    promociones.cpp

HEADERS += \
    ArbolBClientes.h \
    ArbolFunciones.h \
    ArbolMerkle.h \
    ArbolPeliculas.h \
    Blockchain.h \
    LZW.h \
    ListaSolicitudes.h \
    TablaHashReservas.h \
    mainwindow.h \
    matrizdispersa.h \
    promociones.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
