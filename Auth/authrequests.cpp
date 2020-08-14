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

    QVariantMap mp;

    QJsonArray people;
     QJsonObject bob;
     bob.insert("uri", "spotify:track:"+uriMusic);

     people.push_back(bob);

     mp.insert("tracks", people);
    QByteArray array;

    qDebug() << QJsonDocument::fromVariant(mp);
    QUrl url("https://api.spotify.com/v1/playlists/"+ playList +"/tracks");
    QNetworkRequest req = QNetworkRequest(url);
    auto reply = deleteReimplement(req, QJsonDocument(people).toJson());

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

QNetworkReply *AuthRequests::deleteReimplement(const QNetworkRequest &request, const QByteArray &data)
{
    QBuffer *buffer = new QBuffer;
    buffer->setData(data);
    buffer->open(QIODevice::ReadOnly);
    QNetworkAccessManager manager;

    QNetworkReply *reply = manager.post(request, buffer);
    buffer->setParent(reply);
    return reply;
}

QNetworkRequest AuthRequests::createRequest(QUrl url, const QVariantMap *parameters)
{
//    QNetworkRequest req;
//     req.postProcess(createRequest(QNetworkAccessManager::PostOperation, request, data));

//    QUrlQuery query(url.query());
//    QNetworkRequest request;
//    if (parameters) {
//        for (auto it = parameters->begin(), end = parameters->end(); it != end; ++it)
//            query.addQueryItem(it.key(), it.value().toString());
//        url.setQuery(query);

//        qDebug() << url;
//    }
}

void AuthRequests::callbackAuth()
{
    bool status = Auth->status() == QAbstractOAuth::Status::Granted ? true : false;

    callFuncEventReceived(status);
}
