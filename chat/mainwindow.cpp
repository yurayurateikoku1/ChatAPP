#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _loginDlg(new LoginDialog(this))
    ,_regDlg(new RegisterDialog(this))
{
    ui->setupUi(this);
    setCentralWidget(_loginDlg);


    connect(_loginDlg, &LoginDialog::switchRegister,
            this, &MainWindow::SlotSwitchReg);
    _loginDlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    _regDlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    _regDlg->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchReg()
{
    setCentralWidget(_regDlg);
    _loginDlg->hide();
    _regDlg->show();
}
