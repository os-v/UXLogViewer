
QT += core gui widgets network

TARGET = UXLogViewer
TEMPLATE = app

DESTDIR = $$PWD/../Bin

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += \
    AppConfig.cpp \
    main.cpp \
    MainWindow.cpp \
    Utils.cpp \
    LogUI/ListView.cpp \
    LogUI/LogFileFlt.cpp \
    LogUI/LogFileRaw.cpp \
    LogUI/LogFilter.cpp \
    LogUI/LogModel.cpp \
    LogUI/LogTheme.cpp \
    LogUI/LogWidget.cpp \
    LogUI/TextFile.cpp \
    Dialogs/FileDialog.cpp \
    Dialogs/ThemeDialog.cpp \
    Dialogs/SyncDialog.cpp \
    Dialogs/AboutDialog.cpp \
    Dialogs/SettingsDialog.cpp \
    Dialogs/MessageDialog.cpp

HEADERS += \
    AppConfig.h \
    MainWindow.h \
    ProductInfo.h \
    Utils.h \
    LogUI/ListView.h \
    LogUI/LogFile.h \
    LogUI/LogFileFlt.h \
    LogUI/LogFileRaw.h \
    LogUI/LogFilter.h \
    LogUI/LogModel.h \
    LogUI/LogTheme.h \
    LogUI/LogWidget.h \
    LogUI/TextFile.h \
    Dialogs/FileDialog.h \
    Dialogs/ThemeDialog.h \
    Dialogs/SyncDialog.h \
    Dialogs/AboutDialog.h \
    Dialogs/SettingsDialog.h \
    Dialogs/MessageDialog.h

FORMS += \
        MainWindow.ui \
    LogUI/LogWidget.ui \
    Dialogs/FileDialog.ui \
    Dialogs/ThemeDialog.ui \
    Dialogs/SyncDialog.ui \
    Dialogs/AboutDialog.ui \
    Dialogs/SettingsDialog.ui \
    Dialogs/MessageDialog.ui

OTHER_FILES += \
    Platforms/Android/AndroidManifest.xml

DISTFILES +=

RESOURCES += \
    UXLogViewer.qrc

android {
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Platforms/Android
    ANDROID_ABIS = armeabi-v7a
    ANDROID_EXTRA_LIBS += \
        $$PWD/Platforms/Android/libs/libcrypto_1_1.so \
        $$PWD/Platforms/Android/libs/libssl_1_1.so
}

macx:!ios {
    ICON = $$PWD/Platforms/MacOS/AppIcon.icns
    QMAKE_INFO_PLIST = $$PWD/Platforms/MacOS/Info.plist
}

ios: {
    ios_icon.files = $$files($$PWD/Platforms/iOS/AppIcon.iconset/*.png)
    QMAKE_BUNDLE_DATA += ios_icon
    app_launch_images.files = $$PWD/Platforms/iOS/LauncherScreen.xib $$files($$PWD/Platforms/iOS/LaunchImage*.png) $$files($$PLATFORMS_DIR/iOS/SplashImage*.png)
    QMAKE_BUNDLE_DATA += app_launch_images
    QMAKE_INFO_PLIST = $$PWD/Platforms/iOS/Info.plist

}

win32 {
    RC_FILE = $$PWD/Platforms/Windows/AppRes.rc
}

unix:!macx {
}

