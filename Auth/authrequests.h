#ifndef AUTHREQUESTS_H
#define AUTHREQUESTS_H

#include <QObject>
#include <QOAuth2AuthorizationCodeFlow>
#include <memory>

#include "defines.h"

class AuthRequests : public QObject
{
    Q_OBJECT
public:
    explicit AuthRequests(QObject *parent = nullptr);

    using funcReturn = std::function<void (int, QByteArray)>;

    using funcPtrEvent = std::function<void (bool)>;
    void setCallbackEventSetiDc(funcPtrEvent func);

    void setClientAndSecret(QString idClient, QString secret);

    bool init();

    void getInfoUser(funcReturn func);

    void getPlayListsWithUser(funcReturn func);

    void getMusicWithName(QString nameMusic, funcReturn func);

    void createPlayList(Structs::PlayList playList, funcReturn func);

private slots:
    void callbackAuth();

private:
    std::unique_ptr<QOAuth2AuthorizationCodeFlow> Auth;

    QString userName;

    funcPtrEvent funcReceived;
    void callFuncEventReceived(bool ret)
    {
        funcReceived(ret);
    }

};

#endif // AUTHREQUESTS_H
