#ifndef INITAUTH_H
#define INITAUTH_H

#include <QDialog>
#include <memory>

namespace Ui
{ class InitAuth;}

class AuthRequests;

class InitAuth : public QDialog
{
    Q_OBJECT

public:
    InitAuth(QWidget *parent = nullptr);
    ~InitAuth();

private slots:
    void treatButtonAuth();

    void treatAnswer(bool auth);

private:
    Ui::InitAuth *ui;

    std::shared_ptr<AuthRequests> authRequests;
};
#endif // INITAUTH_H
