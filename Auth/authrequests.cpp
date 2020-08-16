#include "authrequests.h"

#include <QDebug>
#include <QtNetworkAuth>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>

AuthRequests::AuthRequests(QObject *parent) :
    QObject(parent)
{
    QOAuthHttpServerReplyHandler *ServerReply = new QOAuthHttpServerReplyHandler(port, this);
    Auth = std::make_unique<QOAuth2AuthorizationCodeFlow>();

    connect(Auth.get(), &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);

    connect(Auth.get(), &QOAuth2AuthorizationCodeFlow::granted, this, &AuthRequests::callbackAuth);

    Auth->setReplyHandler(ServerReply);
}

void AuthRequests::setCallbackEventSetiDc(AuthRequests::funcPtrEvent func)
{
    funcReceived = func;
}

void AuthRequests::setClientAndSecret(QString idClient, QString secret)
{
    Auth->setClientIdentifier(idClient);
    Auth->setClientIdentifierSharedKey(secret);
}

bool AuthRequests::init()
{
    Auth->setAuthorizationUrl(QUrl(UrlAutorization));
    Auth->setAccessTokenUrl(QUrl(UrlToken));
    Auth->setScope(scope);
    Auth->grant();

    return true;
}

void AuthRequests::getInfoUser(funcReturn func)
{
    QUrl url("https://api.spotify.com/v1/me");
    auto reply = Auth->get(url);

    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            func(-1, {});
            return;
        }
        const auto dados = reply->readAll();

        auto document = QJsonDocument::fromJson(dados);
        auto jObj = document.object();
        userName = jObj.value("id").toString();
        func(0, {});

        reply->deleteLater();
    });
}

void AuthRequests::getPlayListsWithUser(funcReturn func)
{
    if(userName.isEmpty())
    {
        func(-1, {});
        return;
    }

    QUrl url("https://api.spotify.com/v1/users/" + userName + "/playlists");
    auto reply = Auth->get(url);

    connect (reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            func(-1, {});
            return;
        }
        const auto data = reply->readAll();
        func(0, data);

        reply->deleteLater();
    });
}

void AuthRequests::addMusicPlayList(QString uriMusic, QString playList, AuthRequests::funcReturn func)
{
    if(uriMusic.isEmpty() || playList.isEmpty())
    {
        func(-1, {});
        return;
    }

    QUrl url("https://api.spotify.com/v1/playlists/"+ playList +"/tracks?uris=spotify%3Atrack%3A"+uriMusic);
    auto reply = Auth->post(url);

    connect (reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            func(-1, {});
            return;
        }
        const auto data = reply->readAll();
        func(0, data);

        reply->deleteLater();
    });
}

void AuthRequests::delMusicPlayList(QString uriMusic, QString playList, AuthRequests::funcReturn func)
{
    if(uriMusic.isEmpty() || playList.isEmpty())
    {
        func(-1, {});
        return;
    }

    QNetworkAccessManager *m_nam = new QNetworkAccessManager;

    QNetworkRequest request(QUrl("https://api.spotify.com/v1/playlists/"+ playList +"/tracks"));

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QByteArray("Authorization"), QByteArray("Bearer "+ Auth->token().toUtf8()));

    QJsonObject obj1;
    obj1.insert("uri","spotify:track:"+uriMusic);

    QJsonArray array;
    array.append(obj1);

    QJsonObject obj;
    obj.insert("tracks", array);

    auto reply = m_nam->sendCustomRequest(request, "DELETE", QJsonDocument(obj).toJson());

    connect (reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << reply->readAll();
            func(-1, {});
            return;
        }
        const auto data = reply->readAll();
        func(0, data);

        reply->deleteLater();
    });
}

void AuthRequests::PlayMusic(QString uriMusic, AuthRequests::funcReturn func)
{
    if(uriMusic.isEmpty())
    {
        func(-1, {});
        return;
    }

    QJsonArray array;
    array.append("spotify:track:"+uriMusic);

    QJsonObject obj;
    obj.insert("uris", array);

    QUrl url("https://api.spotify.com/v1/me/player/play");
    auto reply = Auth->put(url, QJsonDocument(obj).toJson());

    connect (reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << reply->readAll();
            func(-1, {});
            return;
        }
        const auto data = reply->readAll();
        func(0, data);

        reply->deleteLater();
    });
}

void AuthRequests::getMusicWithName(QString nameMusic, AuthRequests::funcReturn func)
{
    if(userName.isEmpty())
    {
        func(-1, {});
        return;
    }

    QUrl url("https://api.spotify.com/v1/search?q="+ nameMusic +"&type=track&market=US&limit=5");
    auto reply = Auth->get(url);

    connect (reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            func(-1, {});
            return;
        }
        const auto data = reply->readAll();
        func(0, data);

        reply->deleteLater();
    });
}

void AuthRequests::getMusicWithPlayList(QString idPlayList, AuthRequests::funcReturn func)
{
    if(userName.isEmpty())
    {
        func(-1, {});
        return;
    }

    QUrl url("https://api.spotify.com/v1/playlists/"+idPlayList+"/tracks");
    auto reply = Auth->get(url);

    connect (reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            func(-1, {});
            return;
        }
        const auto data = reply->readAll();
        func(0, data);

        reply->deleteLater();
    });
}

void AuthRequests::createPlayList(Structs::PlayList playList, AuthRequests::funcReturn func)
{
    if(userName.isEmpty())
    {
        func(-1, {});
        return;
    }

    QJsonObject obj;
    obj.insert("name", playList.name);
    obj.insert("description", playList.descript);
    obj.insert("public", playList.isPublic);

    QUrl url("https://api.spotify.com/v1/users/" + userName + "/playlists");
    auto reply = Auth->post(url, QJsonDocument(obj).toJson());

    connect (reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            func(-1, {});
            return;
        }
        const auto data = reply->readAll();
        func(0, data);

        reply->deleteLater();
    });
}

void AuthRequests::callbackAuth()
{
    bool status = Auth->status() == QAbstractOAuth::Status::Granted ? true : false;

    callFuncEventReceived(status);
}
