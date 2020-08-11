#include "mainscreen.h"
#include "ui_mainscreen.h"

#include <QtNetworkAuth>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>

#include "Auth/authrequests.h"

MainScreen::MainScreen(std::shared_ptr<AuthRequests> auth, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainScreen),
    authRequest(std::move(auth))
{
    ui->setupUi(this);

    userIsReady.store(false);

    connect(ui->criarPlay, &QPushButton::clicked, this, &MainScreen::treatCriarPlay);
    connect(ui->addMusic, &QPushButton::clicked, this, &MainScreen::treatAddMusic);
    connect(ui->delMusic, &QPushButton::clicked, this, &MainScreen::treatDelMusic);
    connect(ui->execPlayList, &QPushButton::clicked, this, &MainScreen::treatExecPlayList);
    connect(ui->searchMusic, &QPushButton::clicked, this, &MainScreen::treatSearchMusic);

    getInfoUser();
}

MainScreen::~MainScreen()
{
    delete ui;
}

void MainScreen::treatCriarPlay()
{

}

void MainScreen::treatAddMusic()
{

}

void MainScreen::treatDelMusic()
{

}

void MainScreen::treatExecPlayList()
{

}

void MainScreen::treatSearchMusic()
{

}

void MainScreen::initList()
{
    if(userIsReady.load())
    {
        authRequest->getPlayListsWithUser([=](int ret, QByteArray data)
        {
            if(ret > 0)
            {

            }
        });
    }
}

void MainScreen::getInfoUser()
{
    authRequest->getInfoUser([=](int ret, QByteArray data)
    {
        Q_UNUSED(data)
        if(ret > 0)
        {
            userIsReady.store(true);
            initList();
        }
    });
}
