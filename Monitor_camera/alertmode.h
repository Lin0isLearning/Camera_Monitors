#ifndef ALERTMODE_H
#define ALERTMODE_H

#include <QWidget>
#include <QTcpSocket>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QCloseEvent>

class AlertMode : public QWidget
{
    Q_OBJECT

public:
    explicit AlertMode(QWidget *parent = nullptr);

private slots:
    void readData();
    void checkForFaces();
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    QTcpSocket *socket;
    QLabel *label;
    QByteArray buffer;
    QTimer *timer;

    void yuyvToRgb888(const uchar *yuyv, uchar *rgb, int width, int height);
};

#endif // ALERTMODE_H
