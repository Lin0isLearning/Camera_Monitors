#include "registerwindow.h"
#include "ui_registerwindow.h"
#include <QMessageBox>

RegisterWindow::RegisterWindow(Database *db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterWindow),
    db(db)
{
    ui->setupUi(this);
}

RegisterWindow::~RegisterWindow() {
    delete ui;
}

void RegisterWindow::on_registerButton_clicked() {
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (db->registerUser(username, password)) {
        QMessageBox::information(this, "Success", "User registered successfully.", QMessageBox::Ok);
        this->close();
    } else {
        QMessageBox::warning(this, "Register Failed", "User registration failed. Username might already exist.", QMessageBox::Ok);
    }
}
