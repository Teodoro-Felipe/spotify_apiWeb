#ifndef MAINSCREEN_H
#define MAINSCREEN_H

#include <QDialog>
#include <memory>

namespace Ui {
class MainScreen;
}

class AuthRequests;

class MainScreen : public QDialog
{
    Q_OBJECT

public:
    explicit MainScreen(std::shared_ptr<AuthRequests> auth, QWidget *parent = nullptr);
    ~MainScreen();

private slots:
    void treatCriarPlay();

    void treatAddMusic();

    void treatDelMusic();

    void treatExecPlayList();

    void treatSearchMusic();

    void treatSelPlayList();

private:

    void initList();

    void getInfoUser();

    Ui::MainScreen *ui;

    std::shared_ptr<AuthRequests> authRequest;
    std::atomic_bool userIsReady;
};

#endif // MAINSCREEN_H
