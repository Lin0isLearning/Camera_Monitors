#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "database.h"
#include "client.h"
#include "ui_mainwindow.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_login_clicked();
    void on_registerbut_clicked();
    void on_exitbut_clicked();

private:
    Ui::MainWindow *ui;
    Database *db;
    Client *client;

    void showVideoStream();
};

#endif // MAINWINDOW_H
