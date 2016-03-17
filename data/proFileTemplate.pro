TEMPLATE  = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
QT -= gui

include(#PRI_FILE#)

SOURCES +=  \
    main.cpp \
    ofApp.cpp

HEADERS += ofApp.h

win32 {
    equals(OF_VERSION, 0.8) {
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
    }
    else {
        equals(AR, "x64") {
            FMODEX_DLL_PATH  = $$OF/export/vs/$$AR/fmodex64.dll
            FMODEXL_DLL_PATH = $$OF/export/vs/$$AR/fmodexL64.dll
        }
        else {
            FMODEX_DLL_PATH  = $$OF/export/vs/$$AR/fmodex.dll
            FMODEXL_DLL_PATH = $$OF/export/vs/$$AR/fmodexL.dll
        }
        EXTRA_BINFILES += \
            $$OF/export/vs/$$AR/assimp.dll \
            $$FMODEX_DLL_PATH \
            $$FMODEXL_DLL_PATH \
            $$OF/export/vs/$$AR/FreeImage.dll \
            $$OF/export/vs/$$AR/freeType.dll \
            $$OF/export/vs/$$AR/glut32.dll \
            $$OF/export/vs/$$AR/libeay32.dll \
            $$OF/export/vs/$$AR/ssleay32.dll \
            $$OF/export/vs/$$AR/Zlib.dll
    }

    EXTRA_BINFILES_WIN = $${EXTRA_BINFILES}
    EXTRA_BINFILES_WIN ~= s,/,\\,g
    win32:CONFIG(release, debug|release): DESTDIR_WIN = $$OUT_PWD/release
    else:win32:CONFIG(debug, debug|release): DESTDIR_WIN = $$OUT_PWD/debug
    DESTDIR_WIN ~= s,/,\\,g
    for(FILE,EXTRA_BINFILES_WIN){
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DESTDIR_WIN) $$escape_expand(\\n\\t)
    }
}
