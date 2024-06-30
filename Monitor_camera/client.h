#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QLabel>
#include <QVBoxLayout>

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);

private slots:
    void readData();

protected:
    void closeEvent(QCloseEvent *event) override;
private:
    QTcpSocket *socket;
    QLabel *label;
    QByteArray buffer;

    void yuyvToRgb888(const uchar *yuyv, uchar *rgb, int width, int height);
};

#endif // CLIENT_H
