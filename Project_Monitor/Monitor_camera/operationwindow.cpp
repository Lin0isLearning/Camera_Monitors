#include "operationwindow.h"
#include "ui_operationwindow.h"
#include "client.h"  // 用于监控视图
#include "alertmode.h"  // 警戒模式

OperationWindow::OperationWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OperationWindow)
{
    ui->setupUi(this);
}

OperationWindow::~OperationWindow()
{
    delete ui;
}

void OperationWindow::on_viewMonitorButton_clicked()
{
    Client *client = new Client();
    client->show();
}

void OperationWindow::on_alertModeButton_clicked()
{
    AlertMode *alertMode = new AlertMode();
    alertMode->show();
}
