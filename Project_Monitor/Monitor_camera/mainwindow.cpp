#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "registerwindow.h"
#include "operationwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    db(new Database("users.db"))
{
    ui->setupUi(this);

    // 数据库初始化检查
    if (!db->initialize()) {
        QMessageBox::critical(this, tr("Database Error"), tr("Failed to initialize the database."), QMessageBox::Ok);
        QCoreApplication::exit(-1); // 退出应用程序
    }
}

MainWindow::~MainWindow() {
    delete ui;
    delete db;
}

void MainWindow::on_login_clicked() {
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (db->authenticateUser(username, password)) {
        QMessageBox::information(this, tr("提示"), tr("登录成功"), QMessageBox::Ok);
        this->close();

        // 显示操作窗口
        OperationWindow *operationWindow = new OperationWindow();
        operationWindow->show();
    } else {
        QMessageBox::warning(this, tr("Login Failed"), tr("账号或密码错误"), QMessageBox::Ok);
    }
}

void MainWindow::on_registerbut_clicked() {
    RegisterWindow *regWin = new RegisterWindow(db); // 传递 db
    regWin->show();
}

void MainWindow::on_exitbut_clicked() {
    QCoreApplication::quit(); // 退出程序
}
