#ifndef INITAUTH_H
#define INITAUTH_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class InitAuth; }
QT_END_NAMESPACE

class InitAuth : public QDialog
{
    Q_OBJECT

public:
    InitAuth(QWidget *parent = nullptr);
    ~InitAuth();

private:
    Ui::InitAuth *ui;
};
#endif // INITAUTH_H
