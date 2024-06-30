#ifndef OPERATIONWINDOW_H
#define OPERATIONWINDOW_H

#include <QMainWindow>
#include "ui_operationwindow.h"
namespace Ui {
class OperationWindow;
}

class OperationWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OperationWindow(QWidget *parent = nullptr);
    ~OperationWindow();

private slots:
    void on_viewMonitorButton_clicked();
    void on_alertModeButton_clicked();

private:
    Ui::OperationWindow *ui;
};

#endif // OPERATIONWINDOW_H
