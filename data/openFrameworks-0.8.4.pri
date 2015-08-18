#openFrameworks path.
CONFIG += USE_PRECOMPILED_LIB
OF = #OF_PATH#
win32:CONFIG(release, debug|release): {
    QMAKE_LFLAGS += /NODEFAULTLIB:PocoFoundationmd.lib
    QMAKE_LFLAGS += /NODEFAULTLIB:LIBCMT
    QMAKE_LFLAGS += /NODEFAULTLIB:LIBC.LIBS
    QMAKE_LFLAGS += /NODEFAULTLIB:atlthunk.lib

    DEFINES += "WIN32"
    DEFINES += "NDEBUG"
    DEFINES += "_CONSOLE"
    DEFINES += "POCO_STATIC"
    DEFINES += "CAIRO_WIN32_STATIC_BUILD"
    DEFINES += "DISABLE_SOME_FLOATING_POINT"
    DEFINES += "_UNICODE"
    DEFINES += "UNICODE"
}
else:win32:CONFIG(debug, debug|release): {
    QMAKE_LFLAGS += /NODEFAULTLIB:PocoFoundationmdd.lib
    QMAKE_LFLAGS += /NODEFAULTLIB:atlthunk.lib
    QMAKE_LFLAGS += /NODEFAULTLIB:msvcrt
    QMAKE_LFLAGS += /NODEFAULTLIB:libcmt
    QMAKE_LFLAGS += /NODEFAULTLIB:LIBC
    QMAKE_LFLAGS += /NODEFAULTLIB:LIBCMTD

    DEFINES += "WIN32"
    DEFINES += "_DEBUG"
    DEFINES += "_CONSOLE"
    DEFINES += "POCO_STATIC"
    DEFINES += "CAIRO_WIN32_STATIC_BUILD"
    DEFINES += "DISABLE_SOME_FLOATING_POINT"
    DEFINES += "_UNICODE"
    DEFINES += "UNICODE"
}

#GLUT
LIBS += $$OF/libs/glut/lib/vs/glut32.lib
INCLUDEPATH += $$OF/libs/glut/include

#GLFW
LIBS += $$OF/libs/glfw/lib/vs/glfw3.lib
INCLUDEPATH += $$OF/libs/glfw/include
INCLUDEPATH += $$OF/libs/glfw/include/GLFW

#RTAUDIO
win32:CONFIG(debug, debug|release):LIBS += $$OF/libs/rtAudio/lib/vs/rtAudioD.lib
else:win32:CONFIG(release, debug|release):LIBS += $$OF/libs/rtAudio/lib/vs/rtAudio.lib
INCLUDEPATH += $$OF/libs/rtAudio/include

#FREEIMAGE
LIBS += $$OF/libs/FreeImage/lib/vs/FreeImage.lib
INCLUDEPATH += $$OF/libs/FreeImage/include

#FREETYPE
LIBS += $$OF/libs/freetype/lib/vs/libfreetype.lib
INCLUDEPATH += $$OF/libs/freetype/include
INCLUDEPATH += $$OF/libs/freetype/include/freetype2

#QUICKTIME
LIBS += $$OF/libs/quicktime/lib/vs/qtmlClient.lib
LIBS += $$OF/libs/quicktime/lib/vs/QTSClient.lib
LIBS += $$OF/libs/quicktime/lib/vs/Rave.lib
INCLUDEPATH += $$OF/libs/quicktime/include

#FMODEX
LIBS += $$OF/libs/fmodex/lib/vs/fmodex_vc.lib
INCLUDEPATH += $$OF/libs/fmodex/include

#VIDEOINPUT
win32:CONFIG(debug, debug|release):LIBS += $$OF/libs/videoInput/lib/vs/videoInputD.lib
else:win32:CONFIG(release, debug|release):LIBS += $$OF/libs/videoInput/lib/vs/videoInput.lib
INCLUDEPATH += $$OF/libs/videoInput/include

#CAIRO
LIBS += $$OF/libs/cairo/lib/vs/cairo-static.lib
LIBS += $$OF/libs/cairo/lib/vs/pixman-1.lib
INCLUDEPATH += $$OF/libs/cairo/include
INCLUDEPATH += $$OF/libs/cairo/include/libpng15
INCLUDEPATH += $$OF/libs/cairo/include/pixman-1
INCLUDEPATH += $$OF/libs/cairo/include/cairo

#GLEW
LIBS += $$OF/libs/glew/lib/vs/glew32s.lib
INCLUDEPATH += $$OF/libs/glew/include

#GLU
LIBS += $$OF/libs/glu/lib/vs/glu32.lib
INCLUDEPATH += $$OF/libs/glu/include

#OPENSSL
LIBS += $$OF/libs/openssl/lib/vs/libeay32md.lib
LIBS += $$OF/libs/openssl/lib/vs/ssleay32md.lib
INCLUDEPATH += $$OF/libs/openssl/include
INCLUDEPATH += $$OF/libs/openssl/include/openssl

#POCO
win32:CONFIG(debug, debug|release): LIBS += $$OF/libs/poco/lib/vs/PocoCryptomdd.lib
else:win32:CONFIG(release, debug|release): LIBS += $$OF/libs/poco/lib/vs/PocoCryptomd.lib
win32:CONFIG(debug, debug|release): LIBS += $$OF/libs/poco/lib/vs/PocoUtilmdd.lib
else:win32:CONFIG(release, debug|release): LIBS += $$OF/libs/poco/lib/vs/PocoUtilmd.lib
win32:CONFIG(debug, debug|release): LIBS += $$OF/libs/poco/lib/vs/PocoXMLmdd.lib
else:win32:CONFIG(release, debug|release): LIBS += $$OF/libs/poco/lib/vs/PocoXMLmd.lib
win32:CONFIG(debug, debug|release): LIBS += $$OF/libs/poco/lib/vs/PocoNetmdd.lib
else:win32:CONFIG(release, debug|release): LIBS += $$OF/libs/poco/lib/vs/PocoNetmd.lib
win32:CONFIG(debug, debug|release): LIBS += $$OF/libs/poco/lib/vs/PocoNetSSLmdd.lib
else:win32:CONFIG(release, debug|release): LIBS += $$OF/libs/poco/lib/vs/PocoNetSSLmd.lib
win32:CONFIG(debug, debug|release): LIBS += $$OF/libs/poco/lib/vs/PocoFoundationmdd.lib
win32:CONFIG(release, debug|release): LIBS += $$OF/libs/poco/lib/vs/PocoFoundationmd.lib
INCLUDEPATH += $$OF/libs/poco/include

#TESS2
LIBS += $$OF/libs/tess2/lib/vs/tess2.lib
INCLUDEPATH += $$OF/libs/tess2/include

#SYSTEM LIBS
LIBS += -lmsimg32
LIBS += -lOpenGL32
LIBS += -lkernel32
LIBS += -lsetupapi
LIBS += -lVfw32
LIBS += -lcomctl32
LIBS += -ldsound
LIBS += -luser32
LIBS += -lgdi32
LIBS += -lwinspool
LIBS += -lcomdlg32
LIBS += -ladvapi32
LIBS += -lshell32
LIBS += -lole32
LIBS += -loleaut32
LIBS += -luuid
LIBS += -lcrypt32
LIBS += -lWs2_32
LIBS += -lodbc32
LIBS += -lodbccp32
LIBS += -lGLu32

#OPENFRAMEWORKS
INCLUDEPATH += $$OF/libs/openFrameworks
INCLUDEPATH += $$OF/libs/openFrameworks/types
INCLUDEPATH += $$OF/libs/openFrameworks/sound
INCLUDEPATH += $$OF/libs/openFrameworks/video
INCLUDEPATH += $$OF/libs/openFrameworks/3d
INCLUDEPATH += $$OF/libs/openFrameworks/math
INCLUDEPATH += $$OF/libs/openFrameworks/events
INCLUDEPATH += $$OF/libs/openFrameworks/utils
INCLUDEPATH += $$OF/libs/openFrameworks/gl
INCLUDEPATH += $$OF/libs/openFrameworks/app
INCLUDEPATH += $$OF/libs/openFrameworks/graphics
INCLUDEPATH += $$OF/libs/openFrameworks/communication

USE_PRECOMPILED_LIB {
    win32:CONFIG(debug, debug|release): OF_LIB_PATH = $$OF/libs/openFrameworksCompiled/lib/vs/openframeworksLib_debug.lib
    else:win32:CONFIG(release, debug|release): OF_LIB_PATH = $$OF/libs/openFrameworksCompiled/lib/vs/openframeworksLib_release.lib
}

win32:CONFIG(debug, debug|release): OF_LIB_PATH = $$OF/libs/openFrameworksCompiled/lib/vs/openframeworksLib_debug.lib
else:win32:CONFIG(release, debug|release): OF_LIB_PATH = $$OF/libs/openFrameworksCompiled/lib/vs/openframeworksLib_release.lib

if(exists($$OF_LIB_PATH)) {
    LIBS += $$OF_LIB_PATH
    message("Using precompiled library for OF")
}
else {
message("Compiling OF")
#3d
SOURCES += $$OF/libs/openFrameworks/3d/of3dPrimitives.cpp
SOURCES += $$OF/libs/openFrameworks/3d/ofMesh.cpp
SOURCES += $$OF/libs/openFrameworks/3d/ofNode.cpp
SOURCES += $$OF/libs/openFrameworks/3d/ofCamera.cpp
SOURCES += $$OF/libs/openFrameworks/3d/of3dUtils.cpp
SOURCES += $$OF/libs/openFrameworks/3d/ofEasyCam.cpp

HEADERS += $$OF/libs/openFrameworks/3d/of3dPrimitives.h
HEADERS += $$OF/libs/openFrameworks/3d/ofMesh.h
HEADERS += $$OF/libs/openFrameworks/3d/ofNode.h
HEADERS += $$OF/libs/openFrameworks/3d/ofCamera.h
HEADERS += $$OF/libs/openFrameworks/3d/of3dUtils.h
HEADERS += $$OF/libs/openFrameworks/3d/ofEasyCam.h

#app
SOURCES += $$OF/libs/openFrameworks/app/ofAppGLFWWindow.cpp
SOURCES += $$OF/libs/openFrameworks/app/ofAppGlutWindow.cpp
SOURCES += $$OF/libs/openFrameworks/app/ofAppNoWindow.cpp
SOURCES += $$OF/libs/openFrameworks/app/ofAppRunner.cpp

HEADERS += $$OF/libs/openFrameworks/app/ofAppBaseWindow.h
HEADERS += $$OF/libs/openFrameworks/app/ofAppGLFWWindow.h
HEADERS += $$OF/libs/openFrameworks/app/ofAppGlutWindow.h
HEADERS += $$OF/libs/openFrameworks/app/ofAppNoWindow.h
HEADERS += $$OF/libs/openFrameworks/app/ofAppRunner.h
HEADERS += $$OF/libs/openFrameworks/app/ofBaseApp.h

#communication
SOURCES += $$OF/libs/openFrameworks/communication/ofSerial.cpp
SOURCES += $$OF/libs/openFrameworks/communication/ofArduino.cpp

HEADERS += $$OF/libs/openFrameworks/communication/ofSerial.h
HEADERS += $$OF/libs/openFrameworks/communication/ofArduino.h

#events
SOURCES += $$OF/libs/openFrameworks/events/ofEvents.cpp

HEADERS += $$OF/libs/openFrameworks/events/ofEvents.h
HEADERS += $$OF/libs/openFrameworks/events/ofEventUtils.h

#gl
SOURCES += $$OF/libs/openFrameworks/gl/ofFbo.cpp
SOURCES += $$OF/libs/openFrameworks/gl/ofGLProgrammableRenderer.cpp
SOURCES += $$OF/libs/openFrameworks/gl/ofGLRenderer.cpp
SOURCES += $$OF/libs/openFrameworks/gl/ofGLUtils.cpp
SOURCES += $$OF/libs/openFrameworks/gl/ofLight.cpp
SOURCES += $$OF/libs/openFrameworks/gl/ofMaterial.cpp
SOURCES += $$OF/libs/openFrameworks/gl/ofShader.cpp
SOURCES += $$OF/libs/openFrameworks/gl/ofTexture.cpp
SOURCES += $$OF/libs/openFrameworks/gl/ofVbo.cpp
SOURCES += $$OF/libs/openFrameworks/gl/ofVboMesh.cpp

HEADERS += $$OF/libs/openFrameworks/gl/ofFbo.h
HEADERS += $$OF/libs/openFrameworks/gl/ofGLProgrammableRenderer.h
HEADERS += $$OF/libs/openFrameworks/gl/ofGLRenderer.h
HEADERS += $$OF/libs/openFrameworks/gl/ofGLUtils.h
HEADERS += $$OF/libs/openFrameworks/gl/ofLight.h
HEADERS += $$OF/libs/openFrameworks/gl/ofMaterial.h
HEADERS += $$OF/libs/openFrameworks/gl/ofShader.h
HEADERS += $$OF/libs/openFrameworks/gl/ofTexture.h
HEADERS += $$OF/libs/openFrameworks/gl/ofVbo.h
HEADERS += $$OF/libs/openFrameworks/gl/ofVboMesh.h

#graphics
SOURCES += $$OF/libs/openFrameworks/graphics/of3dGraphics.cpp
SOURCES += $$OF/libs/openFrameworks/graphics/ofBitmapFont.cpp
SOURCES += $$OF/libs/openFrameworks/graphics/ofCairoRenderer.cpp
SOURCES += $$OF/libs/openFrameworks/graphics/ofGraphics.cpp
SOURCES += $$OF/libs/openFrameworks/graphics/ofImage.cpp
SOURCES += $$OF/libs/openFrameworks/graphics/ofPath.cpp
SOURCES += $$OF/libs/openFrameworks/graphics/ofPixels.cpp
SOURCES += $$OF/libs/openFrameworks/graphics/ofPolyline.cpp
SOURCES += $$OF/libs/openFrameworks/graphics/ofRendererCollection.cpp
SOURCES += $$OF/libs/openFrameworks/graphics/ofTessellator.cpp
SOURCES += $$OF/libs/openFrameworks/graphics/ofTrueTypeFont.cpp

HEADERS += $$OF/libs/openFrameworks/graphics/of3dGraphics.h
HEADERS += $$OF/libs/openFrameworks/graphics/ofBitmapFont.h
HEADERS += $$OF/libs/openFrameworks/graphics/ofCairoRenderer.h
HEADERS += $$OF/libs/openFrameworks/graphics/ofGraphics.h
HEADERS += $$OF/libs/openFrameworks/graphics/ofImage.h
HEADERS += $$OF/libs/openFrameworks/graphics/ofPath.h
HEADERS += $$OF/libs/openFrameworks/graphics/ofPixels.h
HEADERS += $$OF/libs/openFrameworks/graphics/ofPolyline.h
HEADERS += $$OF/libs/openFrameworks/graphics/ofRendererCollection.h
HEADERS += $$OF/libs/openFrameworks/graphics/ofTessellator.h
HEADERS += $$OF/libs/openFrameworks/graphics/ofTrueTypeFont.h

#math
SOURCES += $$OF/libs/openFrameworks/math/ofMath.cpp
SOURCES += $$OF/libs/openFrameworks/math/ofMatrix3x3.cpp
SOURCES += $$OF/libs/openFrameworks/math/ofMatrix4x4.cpp
SOURCES += $$OF/libs/openFrameworks/math/ofQuaternion.cpp
SOURCES += $$OF/libs/openFrameworks/math/ofVec2f.cpp
SOURCES += $$OF/libs/openFrameworks/math/ofVec4f.cpp

HEADERS += $$OF/libs/openFrameworks/math/ofMath.h
HEADERS += $$OF/libs/openFrameworks/math/ofMatrix3x3.h
HEADERS += $$OF/libs/openFrameworks/math/ofMatrix4x4.h
HEADERS += $$OF/libs/openFrameworks/math/ofQuaternion.h
HEADERS += $$OF/libs/openFrameworks/math/ofVec2f.h
HEADERS += $$OF/libs/openFrameworks/math/ofVec3f.h
HEADERS += $$OF/libs/openFrameworks/math/ofVec4f.h
HEADERS += $$OF/libs/openFrameworks/math/ofVectorMath.h

#sound
SOURCES += $$OF/libs/openFrameworks/sound/ofFmodSoundPlayer.cpp
SOURCES += $$OF/libs/openFrameworks/sound/ofRtAudioSoundStream.cpp
SOURCES += $$OF/libs/openFrameworks/sound/ofSoundPlayer.cpp
SOURCES += $$OF/libs/openFrameworks/sound/ofSoundStream.cpp

HEADERS += $$OF/libs/openFrameworks/sound/ofBaseSoundPlayer.h
HEADERS += $$OF/libs/openFrameworks/sound/ofBaseSoundStream.h
HEADERS += $$OF/libs/openFrameworks/sound/ofFmodSoundPlayer.h
HEADERS += $$OF/libs/openFrameworks/sound/ofRtAudioSoundStream.h
HEADERS += $$OF/libs/openFrameworks/sound/ofSoundPlayer.h
HEADERS += $$OF/libs/openFrameworks/sound/ofSoundStream.h

#types
SOURCES += $$OF/libs/openFrameworks/types/ofBaseTypes.cpp
SOURCES += $$OF/libs/openFrameworks/types/ofColor.cpp
SOURCES += $$OF/libs/openFrameworks/types/ofParameter.cpp
SOURCES += $$OF/libs/openFrameworks/types/ofParameterGroup.cpp
SOURCES += $$OF/libs/openFrameworks/types/ofRectangle.cpp

HEADERS += $$OF/libs/openFrameworks/types/ofBaseTypes.h
HEADERS += $$OF/libs/openFrameworks/types/ofColor.h
HEADERS += $$OF/libs/openFrameworks/types/ofParameter.h
HEADERS += $$OF/libs/openFrameworks/types/ofParameterGroup.h
HEADERS += $$OF/libs/openFrameworks/types/ofPoint.h
HEADERS += $$OF/libs/openFrameworks/types/ofRectangle.h
HEADERS += $$OF/libs/openFrameworks/types/ofTypes.h

#utils
SOURCES += $$OF/libs/openFrameworks/utils/ofFileUtils.cpp
SOURCES += $$OF/libs/openFrameworks/utils/ofLog.cpp
SOURCES += $$OF/libs/openFrameworks/utils/ofMatrixStack.cpp
SOURCES += $$OF/libs/openFrameworks/utils/ofSystemUtils.cpp
SOURCES += $$OF/libs/openFrameworks/utils/ofThread.cpp
SOURCES += $$OF/libs/openFrameworks/utils/ofURLFileLoader.cpp
SOURCES += $$OF/libs/openFrameworks/utils/ofUtils.cpp
SOURCES += $$OF/libs/openFrameworks/utils/ofXml.cpp

HEADERS += $$OF/libs/openFrameworks/utils/ofConstants.h
HEADERS += $$OF/libs/openFrameworks/utils/ofFileUtils.h
HEADERS += $$OF/libs/openFrameworks/utils/ofLog.h
HEADERS += $$OF/libs/openFrameworks/utils/ofMatrixStack.h
HEADERS += $$OF/libs/openFrameworks/utils/ofSystemUtils.h
HEADERS += $$OF/libs/openFrameworks/utils/ofThread.h
HEADERS += $$OF/libs/openFrameworks/utils/ofURLFileLoader.h
HEADERS += $$OF/libs/openFrameworks/utils/ofUtils.h
HEADERS += $$OF/libs/openFrameworks/utils/ofXml.h
HEADERS += $$OF/libs/openFrameworks/utils/ofNoise.h

#video
SOURCES += $$OF/libs/openFrameworks/video/ofDirectShowGrabber.cpp
SOURCES += $$OF/libs/openFrameworks/video/ofQtUtils.cpp
SOURCES += $$OF/libs/openFrameworks/video/ofQuickTimeGrabber.cpp
SOURCES += $$OF/libs/openFrameworks/video/ofQuickTimePlayer.cpp
SOURCES += $$OF/libs/openFrameworks/video/ofVideoGrabber.cpp
SOURCES += $$OF/libs/openFrameworks/video/ofVideoPlayer.cpp

HEADERS += $$OF/libs/openFrameworks/video/ofDirectShowGrabber.h
HEADERS += $$OF/libs/openFrameworks/video/ofQtUtils.h
HEADERS += $$OF/libs/openFrameworks/video/ofQuickTimeGrabber.h
HEADERS += $$OF/libs/openFrameworks/video/ofQuickTimePlayer.h
HEADERS += $$OF/libs/openFrameworks/video/ofVideoGrabber.h
HEADERS += $$OF/libs/openFrameworks/video/ofVideoPlayer.h

HEADERS += $$OF/libs/openFrameworks/ofMain.h
}
#addons
