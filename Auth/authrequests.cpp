#include "authrequests.h"
#include "defines.h"

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

void AuthRequests::callbackAuth()
{
    bool status = Auth->status() == QAbstractOAuth::Status::Granted ? true : false;

    callFuncEventReceived(status);
}
