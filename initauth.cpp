#include "initauth.h"
#include "ui_initauth.h"

InitAuth::InitAuth(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InitAuth)
{
    ui->setupUi(this);
}

InitAuth::~InitAuth()
{
    delete ui;
}

