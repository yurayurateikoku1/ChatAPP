#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QRegularExpression>
#include "httpmgr.h"
namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_pushButton_getCode_clicked();
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);


    void on_pushButton_confirm_clicked();

private:
    void showTip(QString str,bool b_ok);
    void initHttpHandlers();
    Ui::RegisterDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;

};

#endif // REGISTERDIALOG_H
