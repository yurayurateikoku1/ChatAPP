#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

     connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);

    ui->label_errTip->setProperty("state","normal");
    repolish(ui->label_errTip);

    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_pushButton_getCode_clicked()
{
     auto email = ui->lineEdit_email->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        //发送http请求获取验证码
        QJsonObject json_obj;
        json_obj["email"]=email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/get_varifycode"),json_obj,ReqId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);
    }else{
        //提示邮箱不正确
        showTip(tr("Email Address Erro"),false);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"),false);
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"),false);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }

    _handlers[id](jsonDoc.object());


    return;
}

void RegisterDialog::showTip(QString str,bool b_ok)
{
    if(b_ok){
            ui->label_errTip->setProperty("state","err");
    }else{
            ui->label_errTip->setProperty("state","normal");
    }
    ui->label_errTip->setText(str);
    repolish(ui->label_errTip);
}

void RegisterDialog::initHttpHandlers()
{
    //注册获取验证码回包逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug()<< "email is " << email ;
    });

    _handlers.insert(ReqId::ID_REG_USER,[this](QJsonObject jsonObj){
        int error=jsonObj["error"].toInt();
        if(error!=ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email=jsonObj["email"].toString();
        showTip(("注册成功"),true);
        qDebug()<<"uid is "<<jsonObj["uuid"].toString();
        qDebug()<< "email is " << email ;
    });
}





void RegisterDialog::on_pushButton_confirm_clicked()
{
    if(ui->lineEdit_User->text()==""){
        showTip(tr("Username cannot null"),false);
        return;
    }
    if(ui->lineEdit_email->text()==""){
        showTip(tr("Email cannot null"),false);
        return;
    }
    if(ui->lineEdit_Password->text()==""){
        showTip(tr("Password cannot null"),false);
        return;
    }
    if(ui->lineEdit_Confirm->text()==""){
        showTip(tr("Confirm Password cannot null"),false);
        return;
    }
    if(ui->lineEdit_varifycode->text()==""){
        showTip(tr("Varifycode cannot null"),false);
        return;
    }

    QJsonObject jsonObj;
    jsonObj["user"]=ui->lineEdit_User->text();
    jsonObj["email"]=ui->lineEdit_email->text();
    jsonObj["passwd"]=ui->lineEdit_Password->text();
    jsonObj["confirm"]=ui->lineEdit_Confirm->text();
    jsonObj["varifycode"]=ui->lineEdit_varifycode->text();

    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"),jsonObj,ReqId::ID_REG_USER,Modules::REGISTERMOD);

}

