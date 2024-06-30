#include "alertmode.h"
#include <QDebug>
#include <QDateTime>
#include <QImage>
#include <QPainter>

//#include <opencv2/opencv.hpp> // 使用 OpenCV 进行人脸检测

const int WIDTH = 640;
const int HEIGHT = 480;

AlertMode::AlertMode(QWidget *parent) : QWidget(parent), timer(new QTimer(this)) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    label = new QLabel(this);
    layout->addWidget(label);
    setLayout(layout);

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &AlertMode::readData);
    connect(timer, &QTimer::timeout, this, &AlertMode::checkForFaces);

    socket->connectToHost("192.168.250.100", 8080); // 设置为虚拟机的 IP 地址和端口

    if (!socket->waitForConnected(5000)) {
        qDebug() << "Connection failed:" << socket->errorString();
    } else {
        qDebug() << "Connection successful";
    }

    timer->start(1000); // 每秒检查一次
    setWindowTitle("警戒模式");
    resize(WIDTH, HEIGHT);
}

void AlertMode::readData() {
    buffer.append(socket->readAll());
    int frameSize = WIDTH * HEIGHT * 2; // YUYV格式每个像素2字节
    while (buffer.size() >= frameSize) {
        QByteArray frameData = buffer.left(frameSize);
        buffer.remove(0, frameSize);

        uchar *yuyvData = reinterpret_cast<uchar*>(frameData.data());
        uchar *rgbData = new uchar[WIDTH * HEIGHT * 3];

        // 将YUYV数据转换为RGB888
        yuyvToRgb888(yuyvData, rgbData, WIDTH, HEIGHT);

        QImage image(rgbData, WIDTH, HEIGHT, QImage::Format_RGB888);

        // 在图像上绘制当前时间
        if (!image.isNull()) {
            QPainter painter(&image);
            painter.setPen(Qt::red);
            painter.setFont(QFont("Arial", 12));
            QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            painter.drawText(10, 20, currentTime);
        } else {
            qDebug() << "Failed to create image";
        }

        label->setPixmap(QPixmap::fromImage(image));
        label->show();

        delete[] rgbData;
    }
}

void AlertMode::yuyvToRgb888(const uchar *yuyv, uchar *rgb, int width, int height) {
    for (int i = 0; i < width * height; i += 2) {
        int y0 = yuyv[i * 2 + 0] & 0xff;
        int u  = yuyv[i * 2 + 1] & 0xff;
        int y1 = yuyv[i * 2 + 2] & 0xff;
        int v  = yuyv[i * 2 + 3] & 0xff;

        int c0 = y0 - 16;
        int c1 = y1 - 16;
        int d = u - 128;
        int e = v - 128;

        int r0 = (298 * c0 + 409 * e + 128) >> 8;
        int g0 = (298 * c0 - 100 * d - 208 * e + 128) >> 8;
        int b0 = (298 * c0 + 516 * d + 128) >> 8;

        int r1 = (298 * c1 + 409 * e + 128) >> 8;
        int g1 = (298 * c1 - 100 * d - 208 * e + 128) >> 8;
        int b1 = (298 * c1 + 516 * d + 128) >> 8;

        rgb[i * 3 + 0] = qBound(0, r0, 255);
        rgb[i * 3 + 1] = qBound(0, g0, 255);
        rgb[i * 3 + 2] = qBound(0, b0, 255);

        rgb[(i + 1) * 3 + 0] = qBound(0, r1, 255);
        rgb[(i + 1) * 3 + 1] = qBound(0, g1, 255);
        rgb[(i + 1) * 3 + 2] = qBound(0, b1, 255);
    }
}
void AlertMode::closeEvent(QCloseEvent *event) {
    // 处理窗口关闭事件，断开与服务器的连接
    if (socket->isOpen()) {
        socket->disconnectFromHost();
        if (socket->state() != QAbstractSocket::UnconnectedState) {
            socket->waitForDisconnected(3000); // 等待3秒断开
        }
        qDebug() << "Client disconnected";
    }
    socket->deleteLater();
    event->accept(); // 接受关闭事件
}

void AlertMode::checkForFaces() {
    // 这里应包含使用 OpenCV 等库进行人脸检测的代码
    // 以及当有人出现时保存视频和相关面部信息的逻辑
}
