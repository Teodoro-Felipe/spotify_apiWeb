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
    if(userIsReady.load())
    {
        Structs::PlayList playList;
        if(ui->lineEdit->text().isEmpty())
        {
            return;
        }
        QString txtLine = ui->lineEdit->text();
        playList.name = txtLine;
        playList.descript = "Essa PlayList foi criada com o nome : " + txtLine;

        authRequest->createPlayList(playList, [=](int ret, QByteArray data)
        {
            if(ret < 0)
            {
                Q_UNUSED(data)
                qDebug() << "Erro ao criar play list";
            }
        });
    }
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
    if(userIsReady.load())
    {
        QString nameMusic = ui->lineEdit->text();

        authRequest->getMusicWithName(nameMusic, [=](int ret, QByteArray data)
        {
            if(ret < 0)
            {
                qDebug() << "Erro ao get Music";
            }
        });
    }
}

void MainScreen::initList()
{
    if(userIsReady.load())
    {
        authRequest->getPlayListsWithUser([=](int ret, QByteArray data)
        {
            if(ret > 0)
            {
                QJsonDocument document = QJsonDocument::fromJson(data);
                QJsonObject obj = document.object();
                QJsonArray array = obj.value("items").toArray();

                for(int i = 0; i < array.size(); i++)
                {
                    qDebug() << array[i].toObject().value("name").toString();
                }
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
