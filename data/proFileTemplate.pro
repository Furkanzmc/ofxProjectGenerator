TEMPLATE  = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
QT -= gui

include(openFrameworks-0.8.4.pri)

SOURCES +=  \
    main.cpp \
    ofApp.cpp

HEADERS += ofApp.h

win32 {
    EXTRA_BINFILES += \
        $$OF/export/vs/Assimp32.dll \
        $$OF/export/vs/fmodex.dll \
        $$OF/export/vs/fmodexL.dll \
        $$OF/export/vs/FreeImage.dll \
        $$OF/export/vs/FreeType-6.dll \
        $$OF/export/vs/glut32.dll \
        $$OF/export/vs/libeay32.dll \
        $$OF/export/vs/ssleay32.dll \
        $$OF/export/vs/Zlib.dll

    EXTRA_BINFILES_WIN = $${EXTRA_BINFILES}
    EXTRA_BINFILES_WIN ~= s,/,\\,g
    win32:CONFIG(release, debug|release): DESTDIR_WIN = $$OUT_PWD/release
    else:win32:CONFIG(debug, debug|release): DESTDIR_WIN = $$OUT_PWD/debug
    DESTDIR_WIN ~= s,/,\\,g
    for(FILE,EXTRA_BINFILES_WIN){
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DESTDIR_WIN) $$escape_expand(\\n\\t)
    }
}