#ifndef MAINSCREEN_H
#define MAINSCREEN_H

#include <QDialog>

namespace Ui {
class MainScreen;
}

class MainScreen : public QDialog
{
    Q_OBJECT

public:
    explicit MainScreen(QWidget *parent = nullptr);
    ~MainScreen();

private:
    Ui::MainScreen *ui;
};

#endif // MAINSCREEN_H
