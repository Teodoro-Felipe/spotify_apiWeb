#include "mainscreen.h"
#include "ui_mainscreen.h"

#include <QtNetworkAuth>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>
#include <QMessageBox>

#include "Auth/authrequests.h"
#include "logs/logshelpers.h"

MainScreen::MainScreen(std::shared_ptr<AuthRequests> auth, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainScreen),
    authRequest(std::move(auth))
{
    ui->setupUi(this);

    logs = new LogsHelpers;

    userIsReady.store(false);

    connect(ui->criarPlay, &QPushButton::clicked, this, &MainScreen::treatCriarPlay);
    connect(ui->addMusic, &QPushButton::clicked, this, &MainScreen::treatAddMusic);
    connect(ui->delMusic, &QPushButton::clicked, this, &MainScreen::treatDelMusic);
    connect(ui->execPlayList, &QPushButton::clicked, this, &MainScreen::treatExecPlayList);
    connect(ui->searchMusic, &QPushButton::clicked, this, &MainScreen::treatSearchMusic);
    connect(ui->tablePlayList, &QTableWidget::currentCellChanged, this, &MainScreen::treatSelPlayList);
    connect(ui->tableMusic, &QTableWidget::currentCellChanged, this, &MainScreen::treatSelMusic);

    getInfoUser();
}

MainScreen::~MainScreen()
{
    delete logs;
    delete ui;
}

void MainScreen::treatCriarPlay()
{
    if(userIsReady.load())
    {
        Structs::PlayList playList;
        if(ui->lineEdit->text().isEmpty())
        {
            logs->logsUserFail(this, "Nenhuma nome foi escolhido!");
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
                logs->logsUserFail(this, "Ocorreu um Problema ao tentar Criar a Playlist");
                return;
            }

            ui->lineEdit->clear();
            LoadList();
            logs->logsUserSucess(this, "PlayList Criada!");
        });
    }
}

void MainScreen::treatAddMusic()
{
    if(userIsReady.load())
    {
        if(idCurrentMusic.isEmpty() || idCurrentPlayList.isEmpty())
        {
            logs->logsUserFail(this, "Nenhuma musica ou playlist Selecionada!");
            return;
        }

        authRequest->addMusicPlayList(idCurrentMusic, idCurrentPlayList, [=](int ret, QByteArray data)
        {
            if(ret < 0)
            {
                Q_UNUSED(data)
                logs->logsUserFail(this, "Ocorreu um Problema ao tentar Adicionar a Musica");
                return;
            }

            ui->lineEdit->clear();
            idCurrentMusic.clear();

            logs->logsUserSucess(this, "Musica Adicionada!");
        });
    }
}

void MainScreen::treatDelMusic()
{
    if(userIsReady.load())
    {
        if(idCurrentMusic.isEmpty() || idCurrentPlayList.isEmpty())
        {
            logs->logsUserFail(this, "Nenhuma musica ou playlist Selecionada!");
            return;
        }

        authRequest->delMusicPlayList(idCurrentMusic, idCurrentPlayList, [=](int ret, QByteArray data)
        {
            if(ret < 0)
            {
                logs->logsUserFail(this, "Ocorreu um Problema ao tentar Deletar a Musica");
                return;
            }
            ui->lineEdit->clear();
            treatSelPlayList(ui->tableMusic->currentRow(),0,0,0);

            idCurrentMusic.clear();

            logs->logsUserSucess(this, "Musica Deletada!");
        });
    }
}

void MainScreen::treatExecPlayList()
{
    if(userIsReady.load())
    {
        if(ui->lineEdit->text().isEmpty() || idCurrentMusic.isEmpty())
        {
            logs->logsUserFail(this, "Nenhuma musica Selecionada!");
            return;
        }

        authRequest->PlayMusic(idCurrentMusic, [=](int ret, QByteArray data)
        {
            if(ret < 0)
            {
                Q_UNUSED(data)
                logs->logsUserFail(this, "Ocorreu um Problema ao tentar Tocar a Musica");
                return;
            }
            logs->logsUserSucess(this, "Tocando Musica Solicitada!");
        });
    }
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
                logs->logsUserFail(this,"Ocorreu um Problema ao Buscar Musica");
                return;
            }

            QJsonDocument document = QJsonDocument::fromJson(data);
            QJsonObject obj = document.object();
            QJsonObject obj2 = obj.value("tracks").toObject();
            QJsonArray array = obj2.value("items").toArray();

            ui->tableMusic->clearContents();
            ui->tableMusic->setRowCount(array.size());
            for(int i = 0; i < array.size(); i++)
            {
                QJsonArray artist = array[i].toObject().value("artists").toArray();
                QString nameArt = artist[0].toObject().value("name").toString();

                QTableWidgetItem *item = new QTableWidgetItem(array[i].toObject().value("name").toString() + " - " + nameArt);
                item->setData(Qt::UserRole, array[i].toObject().value("id").toString());
                ui->tableMusic->setItem(i, 0, item);
            }
            logs->logsUserSucess(this,"Sucesso, Musicas Encontradas!");
        });
    }
}

void MainScreen::treatSelPlayList(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(previousRow)
    Q_UNUSED(previousColumn)

    if(currentRow < 0)
    {
        logs->logsInternos("Ocorreu um Problema ao Selecionar PlayList");
        return;
    }
    idCurrentPlayList = ui->tablePlayList->item(currentRow, currentColumn)->data(Qt::UserRole).toString();

    authRequest->getMusicWithPlayList(idCurrentPlayList,[=](int ret, QByteArray data)
    {
        if(ret < 0)
        {
            logs->logsUserFail(this, "Ocorreu um Problema ao Buscar Musicas da PlayList");
            return;
        }
        QJsonDocument document = QJsonDocument::fromJson(data);
        QJsonObject obj = document.object();
        QJsonArray array = obj.value("items").toArray();

        ui->tableMusic->clearContents();

        ui->tableMusic->setRowCount(array.size());
        for(int i = 0; i < array.size(); i++)
        {
            QJsonObject track = array[i].toObject().value("track").toObject();

            QJsonArray artist = track.value("artists").toArray();
            QString nameArt = artist[0].toObject().value("name").toString();

            QTableWidgetItem *item = new QTableWidgetItem(track.value("name").toString() + " - " + nameArt);
            item->setData(Qt::UserRole, track.value("id").toString());
            ui->tableMusic->setItem(i, 0, item);
        }
    });
}

void MainScreen::treatSelMusic(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(previousRow)
    Q_UNUSED(previousColumn)

    if(currentRow < 0)
    {
        logs->logsInternos("Ocorreu um Problema ao Selecionar Musica");
        return;
    }
    idCurrentMusic = ui->tableMusic->item(currentRow, currentColumn)->data(Qt::UserRole).toString();
    ui->lineEdit->setText(ui->tableMusic->item(currentRow, currentColumn)->text());
}

void MainScreen::LoadList()
{
    if(userIsReady.load())
    {
        authRequest->getPlayListsWithUser([=](int ret, QByteArray data)
        {
            if(ret >= 0)
            {
                QJsonDocument document = QJsonDocument::fromJson(data);
                QJsonObject obj = document.object();
                QJsonArray array = obj.value("items").toArray();

                ui->tablePlayList->clearContents();

                ui->tablePlayList->setRowCount(array.size());
                for(int i = 0; i < array.size(); i++)
                {
                    QTableWidgetItem *item = new QTableWidgetItem(array[i].toObject().value("name").toString());
                    item->setData(Qt::UserRole, array[i].toObject().value("id").toString());
                    ui->tablePlayList->setItem(i, 0, item);
                }
                return;
            }
            logs->logsUserFail(this, "Ocorreu um Problema ao Carregar suas Play Lists");
        });
    }
}

void MainScreen::getInfoUser()
{
    authRequest->getInfoUser([=](int ret, QByteArray data)
    {
        Q_UNUSED(data)
        if(ret < 0)
        {
            logs->logsUserFail(this, "Ocorreu um Problema ao Buscar informações do usuário");
            close();
            return;
        }
        userIsReady.store(true);
        LoadList();
    });
}
