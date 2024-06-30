# 包含 OpenCV 头文件路径
INCLUDEPATH += G:/opencv/opencv3.4-qt-intall/install/include/opencv

# 链接 OpenCV 库文件路径
LIBS += -LG:/opencv/opencv3.4-qt-intall/install/include/opencv



# Qt 模块
QT += core gui sql network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# 警告编译器使用废弃的 Qt API
DEFINES += QT_DEPRECATED_WARNINGS

# 包含的源文件
SOURCES += \
    alertmode.cpp \
    client.cpp \
    database.cpp \
    main.cpp \
    mainwindow.cpp \
    operationwindow.cpp \
    registerwindow.cpp

# 包含的头文件
HEADERS += \
    alertmode.h \
    client.h \
    database.h \
    mainwindow.h \
    operationwindow.h \
    registerwindow.h

# 包含的 UI 文件
FORMS += \
    mainwindow.ui \
    operationwindow.ui \
    registerwindow.ui

# 默认部署规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
