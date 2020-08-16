#ifndef MAINSCREEN_H
#define MAINSCREEN_H

#include <QDialog>
#include <memory>

namespace Ui {
class MainScreen;
}

class AuthRequests;
class LogsHelpers;

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

    void treatSelPlayList(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void treatSelMusic(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void treatCheck();

private:

    void LoadList();

    void getInfoUser();

    Ui::MainScreen *ui;

    std::shared_ptr<AuthRequests> authRequest;
    LogsHelpers *logs;

    std::atomic_bool userIsReady;

    QString idCurrentMusic;
    QString idCurrentPlayList;
};

#endif // MAINSCREEN_H
