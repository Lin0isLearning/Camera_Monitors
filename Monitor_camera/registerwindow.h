#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>
#include "database.h"

namespace Ui {
class RegisterWindow;
}

class RegisterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWindow(Database *db, QWidget *parent = nullptr);
    ~RegisterWindow();

private slots:
    void on_registerButton_clicked();

private:
    Ui::RegisterWindow *ui;
    Database *db;
};

#endif // REGISTERWINDOW_H
