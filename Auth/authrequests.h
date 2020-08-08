#ifndef AUTHREQUESTS_H
#define AUTHREQUESTS_H

#include <QObject>
#include <QOAuth2AuthorizationCodeFlow>
#include <memory>

class AuthRequests : public QObject
{
    Q_OBJECT
public:
    explicit AuthRequests(QObject *parent = nullptr);

    using funcPtrEvent = std::function<void (bool)>;
    void setCallbackEventSetiDc(funcPtrEvent func);


    void setClientAndSecret(QString idClient, QString secret);

    bool init();

private slots:
    void callbackAuth();

private:
    std::unique_ptr<QOAuth2AuthorizationCodeFlow> Auth;

    funcPtrEvent funcReceived;
    void callFuncEventReceived(bool ret)
    {
        funcReceived(ret);
    }

};

#endif // AUTHREQUESTS_H
