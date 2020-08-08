#include "initauth.h"
#include "ui_initauth.h"

#include "Auth/authrequests.h"
#include "mainscreen.h"

InitAuth::InitAuth(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InitAuth)
{
    ui->setupUi(this);

    authRequests = std::make_shared<AuthRequests>();

    connect(ui->Auth, &QPushButton::clicked, this, &InitAuth::treatButtonAuth);

    authRequests->setCallbackEventSetiDc([=](bool auth)
    {
        treatAnswer(auth);
    });
}

InitAuth::~InitAuth()
{
    delete ui;
}

void InitAuth::treatButtonAuth()
{
    QString clientID = ui->lineClientID->text();
    QString secretID = ui->lineSecretClient->text();

    authRequests->setClientAndSecret(clientID, secretID);
    authRequests->init();
}

void InitAuth::treatAnswer(bool auth)
{
    if(auth)
    {
        MainScreen *telaP = new MainScreen;
        telaP->show();
        close();
    }
}

