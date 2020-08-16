#include <QtTest>

#include <QtNetworkAuth>
#include <QJsonDocument>
#include <QJsonObject>

class TesteAuth : public QObject
{
    Q_OBJECT

public:
    TesteAuth()
    {
        QOAuthHttpServerReplyHandler *ServerReply = new QOAuthHttpServerReplyHandler(8080, this);
        std::unique_ptr<QOAuth2AuthorizationCodeFlow> Auth = std::make_unique<QOAuth2AuthorizationCodeFlow>();

        Auth->setReplyHandler(ServerReply);
    }

    void getInfo()
    {
        QUrl url("https://api.spotify.com/v1/me");
        QOAuth2AuthorizationCodeFlow *Auth = new QOAuth2AuthorizationCodeFlow;
        auto reply = Auth->get(url);

        connect(reply, &QNetworkReply::finished, [=]()
        {
            const auto dados = reply->readAll();

            auto document = QJsonDocument::fromJson(dados);
            auto jObj = document.object();
            QString userName = jObj.value("id").toString();

            reply->deleteLater();
        });
    }

    void ConvertInfo(QByteArray data)
    {
        QJsonDocument document = QJsonDocument::fromJson(data);
        QJsonObject obj = document.object();
        QJsonArray array = obj.value("items").toArray();

        for(int i = 0; i < array.size(); i++)
        {
            QJsonObject track = array[i].toObject().value("track").toObject();

            QJsonArray artist = track.value("artists").toArray();
            QString nameArt = artist[0].toObject().value("name").toString();

            QString testename = track.value("name").toString() + " - " + nameArt;
            QString id = track.value("id").toString();
        }
    }

    void DelRequestTeste(QString playList, QString uriMusic)
    {
        QNetworkAccessManager *m_nam = new QNetworkAccessManager;
        QOAuth2AuthorizationCodeFlow *Auth = new QOAuth2AuthorizationCodeFlow;

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
            const auto data = reply->readAll();
            reply->deleteLater();
        });
    }
};

QTEST_APPLESS_MAIN(TesteAuth)

#include "tst_testeauth.moc"
